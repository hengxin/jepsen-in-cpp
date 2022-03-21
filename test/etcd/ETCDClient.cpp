#include "ETCDClient.h"

bool ETCDClient::open(string node) {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__ << " in " << node.c_str());
    this->node = node;
    string url = clientURL(node);
    client = std::shared_ptr<etcd::Client>(new etcd::Client(url));
    return true;
}

bool ETCDClient::close() {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__ << " in " << node.c_str());
    return true;
}

bool ETCDClient::setup() {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__ << " in " << node.c_str());
    return true;
}

bool ETCDClient::invoke(Operation& op) {
    if (op.type != Operation::kInvoke) {
        return false;
    }

    if (op.func == "write") {
        string key = op.op[0].asString();
        string val = op.op[1].asString();
        etcd::Response response = client->put(key, val).get();
        if (response.is_ok()) {
            op.type = Operation::kSuccess;
        } else {
            op.type = Operation::kFailed;
        }
    } else if (op.func == "read") {
        string key = op.op[0].asString();
        etcd::Response response = client->get(key).get();
        if (response.is_ok()) {
            op.type = Operation::kSuccess;
            op.op[1] = std::stoi(response.value().as_string());
        } else {
            op.type = Operation::kFailed;
        }
    } else if (op.func == "cas") {
        string key = op.op[0].asString();
        string val = op.op[1].asString();
        string old = op.op[2].asString();
        etcd::Response response = client->modify_if(key, val, old).get();
        if (response.is_ok()) {
            op.type = Operation::kSuccess;
        } else {
            op.type = Operation::kFailed;
        }
    }

    return true;
}

bool ETCDClient::teardown() {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    return true;
}

string ETCDClient::nodeURL(string node, int port) {
    return "http://" + node + ":" + std::to_string(port);
}

string ETCDClient::peerURL(string node) {
    return nodeURL(node, 2380);
}

string ETCDClient::clientURL(string node) {
    return nodeURL(node, 2379);
}