#include "include.h"
#include "Runner.h"

int main() {
    log4cplus::Initializer initializer;

    vector<string> nodes {"47.108.158.63"};
    SSHInfo ssh("young", "guazi13110", "guazi13110", 22, "/home/young/.ssh/id_rsa", false);
    Runner runner(nodes, 5, ssh);
    runner.run();

    return 0;
}