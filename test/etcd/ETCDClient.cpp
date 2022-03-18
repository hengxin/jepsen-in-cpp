#include "ETCDClient.h"

bool ETCDClient::open(string node) {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    string url = clientURL(node);
    client = std::shared_ptr<etcd::Client>(new etcd::Client(url));
    return true;
}

bool ETCDClient::close() {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    return true;
}

bool ETCDClient::setup() {
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    return true;
}

bool ETCDClient::invoke(Operation& op) {
    if (op.getStatus() != Operation::kInvoke) {
        return false;
    }

    if (op.getType() == "w") {
        string key = op.getOp("key").asString();
        string val = op.getOp("value").asString();
        etcd::Response response = client->put(key, val).get();
        if (response.is_ok()) {
            op.setStatus(Operation::kSuccess);
        } else {
            op.setStatus(Operation::kFailed);
        }
    } else if (op.getType() == "r") {
        string key = op.getOp("key").asString();
        etcd::Response response = client->get(key).get();
        if (response.is_ok()) {
            op.setStatus(Operation::kSuccess);
            op.getOp("value") = response.value().as_string();
        } else {
            op.setStatus(Operation::kFailed);
        }
    } else if (op.getType() == "cas") {
        string key = op.getOp("key").asString();
        string val = op.getOp("value").asString();
        string old = op.getOp("old-value").asString();
        etcd::Response response = client->modify_if(key, val, old).get();
        if (response.is_ok()) {
            op.setStatus(Operation::kSuccess);
        } else {
            op.setStatus(Operation::kFailed);
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