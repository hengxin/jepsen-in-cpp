#include "ETCDClient.h"

bool ETCDClient::open(string node) {
    return true;
}

bool ETCDClient::close() {
    return true;
}

bool ETCDClient::setup() {
    return true;
}

bool ETCDClient::invoke(Operation op) {
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