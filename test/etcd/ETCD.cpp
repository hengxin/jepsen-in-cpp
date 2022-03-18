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
    string listen_peer_url = peerURL(node, false);
    string listen_client_url = clientURL(node, true);
    string initial_cluster = initialCluster();

    // refer to https://blog.csdn.net/weixin_44571270/article/details/104797656
    /***
     * --listen-peer-urls
     * "http://172.16.193.200:2380"
     * 定义本台etcd服务器与集群中其他etcd服务通信的端口
     *
     * --listen-client-urls
     * "http://127.0.0.1:2379,http://172.16.193.200:2379"
     * 定义本台etcd服务器的数据端口，监听本地是为了本地连接，承接命令带来的数据；监听真实ip，是为了接受集群其他服务器带来的数据。
     *
     * --advertise-client-urls
     * "http://127.0.0.1:2379,http://172.16.193.200:2379"
     * 这个参数和ETCD_LISTEN_CLIENT_URLS这个参数容易搞混，还是定义和宣告的区别，和上面那一对参数一样的区别。
     *
     * --initial-advertise-peer-urls
     * "172.16.193.200:2380"
     * 这个参数可能大家会和ETCD_LISTEN_PEER_URLS这个参数搞混，LISTEN参数是本地监听这个真实ip的端口，在本地定义这个端口。
     * 此处的ADVERTISE参数则是把这个listen参数值告诉给集群中其他人，告诉了，别人才能来找你。所以一般来说这两个参数值都是一样的。
     *
     * ----initial-cluster
     * "etcd1=http://172.16.193.200:2380,etcd2=172.16.193.201:2380,etcd3=172.16.193.202:2380"
     * 定义该etcd集群中的节点有哪些,并给出了集群通信端口,etcd1这是成员标示
     */
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
    fmt % name % listen_peer_url % listen_client_url % listen_client_url % listen_peer_url %
        initial_cluster;

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

std::vector<string> ETCD::getPrimaries() {
    return {nodes[0]};
}

bool ETCD::setupPrimary(string node) {
    LOG4CPLUS_INFO(logger, "Set up primary in " << node.c_str());
    return true;
}

string ETCD::nodeURL(string node, int port, bool local = false) {
    string base = "http://" + ip_map[node] + ":" + std::to_string(port);
    if (local) {
        return "http://127.0.0.1:" + std::to_string(port) + "," + base;
    } else {
        return base;
    }
}

string ETCD::peerURL(string node, bool local = false) {
    return nodeURL(node, 2380, local);
}

string ETCD::clientURL(string node, bool local = false) {
    return nodeURL(node, 2379, local);
}

string ETCD::initialCluster() {
    int n = nodes.size();
    string initial_cluster = "n0=" + peerURL(nodes[0], false);
    for (int i = 1; i < n; i++) {
        initial_cluster += ",n" + std::to_string(i) + "=" + peerURL(nodes[i], false);
    }
    return initial_cluster;
}