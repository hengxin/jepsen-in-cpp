#ifndef ETCD_H
#define ETCD_H

#include "Control.h"
#include "Database.h"

using jepsen::ControlHelper;
using jepsen::DB;
using jepsen::DBPrimaryInterface;

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
    string nodeURL(string node, int port, bool local);
    string peerURL(string node, bool local);
    string clientURL(string node, bool local);
    string initialCluster();

private:
    string version;
    unordered_map<string, string> ip_map{{"47.108.193.81", "172.24.234.239"},
                                         {"47.108.227.187", "172.24.234.240"},
                                         {"47.108.208.93", "172.24.234.241"}};
};

#endif