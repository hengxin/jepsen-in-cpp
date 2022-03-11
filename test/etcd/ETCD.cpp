#include "ETCD.h"

bool ETCD::setup(string node) {
    LOG4CPLUS_INFO(logger, "Set up etcd " << version.c_str() << " in " << node.c_str());
    string url = "https://storage.googleapis.com/etcd/" + version + "/etcd-" + version +
        "-linux-amd64.tar.gz";
    auto remote = remotes[node];
    ControlHelper::install_archive(remote, url, kDir, false);
    return true;
}

bool ETCD::teardown(string node) {
    auto remote = remotes[node];
    LOG4CPLUS_INFO(logger, "Teardown etcd database system in " << node.c_str());
    return true;
}

vector<string> ETCD::getPrimaries() {
    return {nodes[0]};
}

bool ETCD::setupPrimary(string node) {
    LOG4CPLUS_INFO(logger, "Set up primary in " << node.c_str());
    return true;
}