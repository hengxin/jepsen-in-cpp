#ifndef SSH_CLIENT_H
#define SSH_CLIENT_H

#include "RemoteSpec.h"
#include "SSHConfig.h"

using std::cerr;
using std::endl;
using std::lock_guard;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

static mutex init_mutex;  // mutex for libssh2_init which is not thread safe


class SSHClient {
public:
    const int kSocketInit = -1;

public:
    SSHClient() = delete;
    SSHClient(ConnSpec conn) : SSHClient(conn.host, conn.username, conn.password, conn.port) {}
    SSHClient(string ip_addr, string username, string password, int port = 22)
        : ip_addr(ip_addr),
          port(port),
          username(username),
          password(password),
          logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {
        LOG4CPLUS_DEBUG(logger, "SSHClient: Constructing");
        {
            // libssh2_init is not thread safe
            lock_guard<mutex> lck(init_mutex);
            int rc;
            if (rc = libssh2_init(0)) {
                fprintf(stderr, "SSHClient::libssh2 initialization failed (%d)\n", rc);
                exit(-1);  // TODO: in search of a good method to safely exit
            }
        }
        host_addr = inet_addr(ip_addr.c_str());
        sock = kSocketInit;
        session = nullptr;
    };
    SSHClient(const SSHClient& sshClient);
    ~SSHClient() {
        LOG4CPLUS_DEBUG(logger, "~SSHClient: Destructing");
        disConnect();
        libssh2_exit();  // TODO: When multi-thread, should be call how many times
    };

    SSHClient& operator=(const SSHClient& rhs) {
        LOG4CPLUS_DEBUG(logger, "SSHClient: Assign Operator");
        if (this == &rhs) {
            return *this;
        }

        SSHClient(rhs.ip_addr, rhs.username, rhs.password, rhs.port);
        return *this;
    }

    bool execute(const string command);
    bool execute(const string command, string& channel_stdout, string& channel_stderr);
    void getConnection();
    bool connectTo();
    bool disConnect();

private:
    string ip_addr;           // 127.0.0.1
    string hostname;          //
    unsigned long host_addr;  // ip地址的计算机内部表示

    int port;
    string username;
    string password;

    int sock;  // socket
    LIBSSH2_SESSION* session;

    log4cplus::Logger logger;
};

#endif