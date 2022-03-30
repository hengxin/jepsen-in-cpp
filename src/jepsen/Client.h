#ifndef CLIENT_H
#define CLIENT_H

#include "Config.h"
#include "Operation.h"
#include "control/SSHRemote.h"

#include "include/log4cplus.h"
#include <memory>
#include <unordered_map>

namespace jepsen {

// Refer to https://www.zhihu.com/question/419982799/answer/1461745869
class Client;
using ClientPtr = unique_ptr<Client>;
using ClientCreator = std::function<ClientPtr()>;

template <class T>
struct ClientConstructorHelper {
    static_assert(std::is_base_of<Client, T>::value, "T must derived from Client!");
    ClientPtr operator()() {
        return ClientPtr(new T);
    }
};

class Client {
public:
    Client() : logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool open(string node) = 0;
    virtual bool close() = 0;
    virtual bool setup() = 0;
    virtual bool invoke(Operation& op) = 0;
    virtual bool teardown() = 0;

    template <class T>
    static void Register() {
        cur_client_type = ClientConstructorHelper<T>();
    }

    static ClientPtr createOne() {
        return cur_client_type();
    }

protected:
    std::unordered_map<string, SSHRemotePtr> remotes;
    log4cplus::Logger logger;

private:
    static ClientCreator cur_client_type;
};


class NoopClient : public Client {
public:
    NoopClient() {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    };
    virtual bool open(string node) override {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
        return true;
    };
    virtual bool close() override {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
        return true;
    };
    virtual bool setup() override {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
        return true;
    };
    virtual bool invoke(Operation& op) override {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
        return true;
    };
    virtual bool teardown() override {
        return true;
    };
};

}  // namespace jepsen


#endif