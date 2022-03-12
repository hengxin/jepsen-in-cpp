#include "ETCD.h"
#include "ETCDClient.h"
#include "Runner.h"

// public-cd-b{0-2}.disalg.cn
const string b0 = "47.108.193.81";
const string b1 = "47.108.227.187";
const string b2 = "47.108.208.93";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.cfg"));

    Client::Register<ETCDClient>();
    Nemesis::Register<NoopNemesis>();

    vector<string> nodes{b0, b1, b2};
    SSHInfo ssh("young", "guazi13110", "guazi13110", 22, "/home/young/.ssh/id_rsa", false);
    Runner runner(nodes, 5, ssh, true);
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