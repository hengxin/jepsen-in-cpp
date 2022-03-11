#ifndef ETCD_H
#define ETCD_H

#include "Control.h"
#include "Database.h"

const string kDir = "/opt/etcd";

class ETCD : public DB, public DBPrimaryInterface {
public:
    ETCD(vector<string> nodes) : DB(nodes), version("v3.1.5") {}
    virtual bool setup(string node) override;
    virtual bool teardown(string node) override;

    virtual vector<string> getPrimaries() override;
    virtual bool setupPrimary(string node) override;

private:
    string version;
};

#endif