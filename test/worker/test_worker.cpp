#include "Worker.h"

// public-cd-a{0-2}.disalg.cn
const string a0 = "47.108.158.63";
const string a1 = "47.108.172.24";
const string a2 = "47.108.30.249";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.config"));

    vector<string> nodes{a0, a1, a2};

    Client::Register<NoopClient>();  // Register ETCDClient
    Nemesis::Register<NoopNemesis>();
    shared_ptr<ClientWorker> p1 = std::make_shared<ClientWorker>(1, a0);


    return 0;
}