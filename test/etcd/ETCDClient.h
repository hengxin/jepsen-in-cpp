#ifndef ETCD_CLIENT_H
#define ETCD_CLIENT_H

#include "Client.h"

class ETCDClient : public Client {
public:
    ETCDClient() {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    };
    virtual bool open(string node) override;
    virtual bool close() override;
    virtual bool setup() override;
    virtual bool invoke(Operation op) override;
    virtual bool teardown() override;

protected:
    string nodeURL(string node, int port);
    string peerURL(string node);
    string clientURL(string node);
};


#endif