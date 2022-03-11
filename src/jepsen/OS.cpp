#include "OS.h"

void OS::initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes) {
    this->remotes = remotes;  // TODO: Deep copy?
}

bool Ubuntu::setup(string node) {
    auto remote = remotes[node];
    LOG4CPLUS_INFO(logger, "Set up ubuntu system in " << node.c_str());
    return true;
}

bool Ubuntu::teardown(string node) {
    auto remote = remotes[node];
    LOG4CPLUS_INFO(logger, "Teardown ubuntu system in " << node.c_str());
    return true;
}