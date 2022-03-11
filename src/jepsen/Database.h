#ifndef DATABASE_H
#define DATABASE_H

#include "SSHRemote.h"
#include "include.h"

class DB {
public:
    DB(vector<string> nodes)
        : nodes(nodes), logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool setup(string node) = 0;
    virtual bool teardown(string node) = 0;

    void initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes);

protected:
    unordered_map<string, shared_ptr<SSHRemote>> remotes;
    vector<string> nodes;
    log4cplus::Logger logger;
};

class DBPauseInterface {
public:
    virtual bool pause(string node) = 0;
    virtual bool resume(string node) = 0;
};

class DBPrimaryInterface {
public:
    virtual vector<string> getPrimaries() = 0;
    virtual bool setupPrimary(string node) = 0;
};

class ETCD : public DB, public DBPrimaryInterface {
public:
    ETCD(vector<string> nodes) : DB(nodes) {}
    virtual bool setup(string node) override;
    virtual bool teardown(string node) override;

    virtual vector<string> getPrimaries() override;
    virtual bool setupPrimary(string node) override;
};

#endif