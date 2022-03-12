#ifndef ETCD_H
#define ETCD_H

#include "Control.h"
#include "Database.h"

const string kDir = "/opt/etcd";
const string kBinary = "etcd";
const string kLogFile = kDir + "/etcd.log";
const string kPidFile = kDir + "/etcd.pid";

class ETCD : public DB, public DBPrimaryInterface {
public:
    ETCD(vector<string> nodes) : DB(nodes), version("v3.1.5") {}
    virtual bool setup(string node) override;
    virtual bool teardown(string node) override;

    virtual vector<string> getPrimaries() override;
    virtual bool setupPrimary(string node) override;

protected:
    string nodeURL(string node, int port);
    string peerURL(string node);
    string clientURL(string node);
    string initialCluster();

private:
    string version;
};

#endif