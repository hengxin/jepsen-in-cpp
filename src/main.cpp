#include <iostream>
#include "control/SSHClient.h"
#include <thread>

using std::cout;
using std::endl;
using std::thread;

void remoteWorks(int tid){

    printf("[Thread %d] Testing in c++ version of %l", tid, __cplusplus);

    SSHClient client("47.108.158.63", "young", "", 22);

    client.execute("uptime");
    client.execute("cat /proc/cpuinfo");
    client.execute("ls /tmp/");

    cout << "Testing for control end" << endl;
}

void testMultiThreadSSH(){
    thread t1(remoteWorks, 0);
    thread t2(remoteWorks, 1);
    t1.join();
    t2.join();
}

int main(int argc, char** argv){
    return 0;
}
