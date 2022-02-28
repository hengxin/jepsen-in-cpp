#include <iostream>
#include "control/SSHClient.h"

using std::cout;
using std::endl;

int main(int argc, char** argv){
    cout << "Testing for control in c++ version of "  << __cplusplus << endl;

    SSHClient client("47.108.158.63", "young", "", 22);

    client.execute("uptime");
    client.execute("cat /proc/cpuinfo");
    client.execute("ls /tmp/");

    cout << "Testing for control end" << endl;
    return 0;
}
