#include "ETCD.h"
#include "ETCDClient.h"
#include "Runner.h"

// public-cd-a{0-2}.disalg.cn
const string a0 = "47.108.158.63";
const string a1 = "47.108.172.24";
const string a2 = "47.108.30.249";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.config"));

    Client::Register<ETCDClient>();
    Nemesis::Register<NoopNemesis>();

    vector<string> nodes{a0, a1, a2};
    SSHInfo ssh("young", "", "", 22, "/home/young/.ssh/id_rsa", false);
    Runner runner(nodes, 5, ssh);
    // Initialize Operating System
    shared_ptr<OS> os = std::make_shared<Ubuntu>();
    runner.setOS(os);
    // Initialize Database
    shared_ptr<DB> db = std::make_shared<ETCD>(ETCD(nodes));
    runner.setDB(db);
    // Initialize Client
    runner.run();

    return 0;
}