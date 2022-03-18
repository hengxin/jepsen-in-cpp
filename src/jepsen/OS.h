#ifndef OS_H
#define OS_H

#include "SSHRemote.h"

#include "include/log4cplus.h"
#include <memory>
#include <unordered_map>

namespace jepsen {
class OS;
using OSPtr = std::shared_ptr<OS>;

class OS {
public:
    OS() : logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool setup(string node) = 0;
    virtual bool teardown(string node) = 0;

    void initRemotes(std::unordered_map<string, SSHRemotePtr>& remotes);

protected:
    std::unordered_map<string, SSHRemotePtr> remotes;
    log4cplus::Logger logger;
};

class Ubuntu : public OS {
public:
    virtual bool setup(string node) override;
    virtual bool teardown(string node) override;
};

}  // namespace jepsen

#endif