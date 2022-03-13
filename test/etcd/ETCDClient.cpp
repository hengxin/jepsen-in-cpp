#include "ETCDClient.h"

bool ETCDClient::open(string node) {
    string url = clientURL(node);
    client = shared_ptr<etcd::Client>(new etcd::Client(url));
    return true;
}

bool ETCDClient::close() {
    return true;
}

bool ETCDClient::setup() {
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
    }

    return true;
}

bool ETCDClient::teardown() {
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