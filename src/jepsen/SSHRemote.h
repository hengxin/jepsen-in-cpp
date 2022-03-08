#ifndef SSH_REMOTE_H
#define SSH_REMOTE_H

#include "Remote.h"
#include "SSHClient.h"


class SSHRemote : public Remote{
public:
    SSHRemote(int concurrency_limit, ConnSpec conn_spec)
        :concurrency_limit(concurrency_limit), conn_spec(conn_spec), ssh_client(conn_spec), logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("SSHRemote to ") << conn_spec.host.c_str());
            ssh_client.getConnection();
        }
    ~SSHRemote() {
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("~SSHRemote to ") << conn_spec.host.c_str());
    }
    SSHRemote& operator=(const SSHRemote& rhs);
    virtual void connect(ConnSpec conn_spec) override;
    virtual void disconnect() override;
    virtual int execute(Context ctx, string cmd, string in) override; 
    virtual void upload(Context ctx, string local_path, string remote_path) override;
    virtual void download(Context ctx, string remote_path, string local_path) override;    
private:
    int concurrency_limit;
    ConnSpec conn_spec;
    SSHClient ssh_client;
    // Semaphore
    log4cplus::Logger logger;
};



#endif