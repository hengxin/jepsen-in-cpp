#ifndef DATABASE_H
#define DATABASE_H

#include "SSHRemote.h"

#include "include/log4cplus.h"
#include <memory>
#include <vector>

namespace jepsen {
class DB;
using DBPtr = std::shared_ptr<DB>;

class DB {
public:
    DB(std::vector<string> nodes)
        : nodes(nodes), logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool setup(string node) = 0;
    virtual bool teardown(string node) = 0;

    void initRemotes(std::unordered_map<string, SSHRemotePtr>& remotes);

protected:
    std::unordered_map<string, SSHRemotePtr> remotes;
    std::vector<string> nodes;
    log4cplus::Logger logger;
};

class DBPauseInterface {
public:
    virtual bool pause(string node) = 0;
    virtual bool resume(string node) = 0;
};

class DBPrimaryInterface {
public:
    virtual std::vector<string> getPrimaries() = 0;
    virtual bool setupPrimary(string node) = 0;
};
}  // namespace jepsen


#endif