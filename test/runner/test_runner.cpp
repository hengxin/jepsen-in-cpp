#include "Runner.h"

// public-cd-a{0-2}.disalg.cn
const string a0 = "47.108.158.63"; 
const string a1 = "47.108.172.24";
const string a2 = "47.108.30.249";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.config"));

    vector<string> nodes {a0, a1, a2};
    SSHInfo ssh("young", "", "", 22, "/home/young/.ssh/id_rsa", false);
    Runner runner(nodes, 5, ssh);
    runner.run();

    return 0;
}