#include "ETCD.h"

bool ETCD::setup(string node) {
    LOG4CPLUS_INFO(logger, "Set up etcd " << version.c_str() << " in " << node.c_str());
    string url = "https://storage.googleapis.com/etcd/" + version + "/etcd-" + version +
        "-linux-amd64.tar.gz";
    auto remote = remotes[node];
    ControlHelper::installArchive(remote, url, kDir);

    ControlHelper::StartDaemonOptions opts;
    opts.logfile = kLogFile;
    opts.pidfile = kPidFile;
    opts.chdir = kDir;

    // find the node
    int idx = 0;
    for (; idx < nodes.size(); idx++) {
        if (nodes[idx] == node) {
            break;
        }
    }

    string name = "n" + std::to_string(idx);
    string peer_url = peerURL(node);
    string client_url = clientURL(node);
    string initial_cluster = initialCluster();

    string args_template =
        " --log-output stderr"
        " --name %s"                   // 0
        " --listen-peer-urls %s"       // 1
        " --listen-client-urls %s"     // 2
        " --advertise-client-urls %s"  // 3
        " --initial-cluster-state new"
        " --initial-advertise-peer-urls %s"  // 4
        " --initial-cluster %s";             // 5

    boost::format fmt(args_template);
    fmt % name % peer_url % client_url % client_url % peer_url % initial_cluster;

    ControlHelper::startDaemon(remote, opts, kBinary, fmt.str());

    return true;
}

bool ETCD::teardown(string node) {
    LOG4CPLUS_INFO(logger, "Teardown etcd database system in " << node.c_str());

    auto remote = remotes[node];
    ControlHelper::stopDaemon(remote, kBinary, kPidFile);
    remote->sudoExecute("rm -rf " + kDir);
    return true;
}

vector<string> ETCD::getPrimaries() {
    return {nodes[0]};
}

bool ETCD::setupPrimary(string node) {
    LOG4CPLUS_INFO(logger, "Set up primary in " << node.c_str());
    return true;
}

string ETCD::nodeURL(string node, int port) {
    return "http://" + node + ":" + std::to_string(port);
}

string ETCD::peerURL(string node) {
    return nodeURL(node, 2380);
}

string ETCD::clientURL(string node) {
    return nodeURL(node, 2379);
}

string ETCD::initialCluster() {
    int n = nodes.size();
    string initial_cluster = "n0=" + peerURL(nodes[0]);
    for (int i = 1; i < n; i++) {
        initial_cluster += ",n" + std::to_string(i) + "=" + peerURL(nodes[i]);
    }
    return initial_cluster;
}