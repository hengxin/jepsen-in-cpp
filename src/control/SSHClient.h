#ifndef SSH_CLIENT_H
#define SSH_CLIENT_H

#include "SSHConfig.h"

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::mutex;
using std::lock_guard;
using std::cerr;
using std::endl;

static mutex init_mutex; // mutex for libssh2_init which is not thread safe

class SSHClient {
 public:
    const int kSocketInit = -1;
 public:
    SSHClient() = delete;
    SSHClient(string ip_addr, string username, string password, int port=22)
        :ip_addr(ip_addr), port(port), username(username), password(password) {
            fprintf(stderr, "SSHClient: Constructing\n");
            {
                // libssh2_init is not thread safe
                lock_guard<mutex> lck(init_mutex);
                int rc;
                if(rc = libssh2_init(0)){
                    fprintf(stderr, "SSHClient::libssh2 initialization failed (%d)\n", rc);
                    exit(-1); // TODO: in search of a good method to safely exit
                }
            }
            host_addr = inet_addr(ip_addr.c_str());
            sock = kSocketInit;
            session = nullptr;
            // TODO: In construction or separate?
            if(connectTo()) {             
                fprintf(stderr, "connecTo %s successfully\n", ip_addr.c_str());
            }else{
                fprintf(stderr, "connecTo %s failed\n", ip_addr.c_str());
            }
        };
    ~SSHClient(){
        fprintf(stderr, "SSHClient: Destructing\n");
        disConnect();
        libssh2_exit(); // TODO: When multi-thread, should be call how many times
    };

    void execute(const string command);
    bool connectTo();
    bool disConnect(); 

 private:
    string ip_addr; // 127.0.0.1 
    string hostname; // 
    unsigned long host_addr;  // ip地址的计算机内部表示

    int port;           
    string username;    
    string password;    

    int sock; // socket
    LIBSSH2_SESSION* session;
};

#endif