#include "Client.h"

ClientCreator Client::cur_client_type = ClientConstructorHelper<NoopClient>();

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
