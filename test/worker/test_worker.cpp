#include "ETCDClient.h"
#include "Worker.h"

using namespace jepsen;

// public-cd-b{0-2}.disalg.cn
const string b0 = "47.108.193.81";
const string b1 = "47.108.227.187";
const string b2 = "47.108.208.93";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.cfg"));
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));

    vector<string> nodes{b0, b1, b2};

    Client::Register<ETCDClient>();  // Register ETCDClient
    Nemesis::Register<NoopNemesis>();
    shared_ptr<ClientWorker> p1 = std::make_shared<ClientWorker>(1, b0);
    p1->open(0);
    {
        Operation::OPInfo op;
        op["key"] = "x";
        op["value"] = "2";
        Operation w1("w", op, Operation::kInvoke);
        p1->invoke(w1);
    }

    {
        Operation::OPInfo op;
        op["key"] = "x";
        Operation r1("r", op, Operation::kInvoke);
        p1->invoke(r1);
        string key = r1.getOp("key").asString();
        string val = r1.getOp("value").asString();
        LOG4CPLUS_INFO(logger, "read from key " << key.c_str() << " with value " << val.c_str());
    }

    {
        Operation::OPInfo op;
        op["key"] = "y";
        Operation r1("r", op, Operation::kInvoke);
        p1->invoke(r1);
        string key = r1.getOp("key").asString();
        string val = r1.getOp("value").asString();
        LOG4CPLUS_INFO(logger, "read from key " << key.c_str() << " with value " << val.c_str());
    }

    return 0;
}