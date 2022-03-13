#ifndef OS_H
#define OS_H

#include "SSHRemote.h"
#include "include.h"

namespace jepsen {
class OS {
public:
    OS() : logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool setup(string node) = 0;
    virtual bool teardown(string node) = 0;

    void initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes);

protected:
    unordered_map<string, shared_ptr<SSHRemote>> remotes;
    log4cplus::Logger logger;
};

class Ubuntu : public OS {
public:
    virtual bool setup(string node) override;
    virtual bool teardown(string node) override;
};

}  // namespace jepsen

#endif