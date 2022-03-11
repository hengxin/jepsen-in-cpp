#include "Database.h"

void DB::initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes) {
    this->remotes = remotes;  // TODO: Deep copy?
}

bool ETCD::setup(string node) {
    auto remote = remotes[node];
    LOG4CPLUS_INFO(logger, "Set up etcd database in " << node.c_str());
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