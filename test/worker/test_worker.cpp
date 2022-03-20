#include "ETCDClient.h"
#include "Generator.h"
#include "Worker.h"
#include <future>

using namespace jepsen;
using std::vector;

// public-cd-b{0-2}.disalg.cn
const string b0 = "47.108.193.81";
const string b1 = "47.108.227.187";
const string b2 = "47.108.208.93";

vector<Operation> randomOperations() {
    std::vector<Operation> ops;
    ops.push_back(OperationFactory::read("x"));
    ops.push_back(OperationFactory::write("x", "1"));
    ops.push_back(OperationFactory::read("x"));
    ops.push_back(OperationFactory::write("x", "3"));
    ops.push_back(OperationFactory::read("x"));
    return ops;
}

void testETCDWorker() {
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));

    vector<string> nodes{b0, b1, b2};

    Client::Register<ETCDClient>();  // Register ETCDClient
    Nemesis::Register<NoopNemesis>();
    shared_ptr<ClientWorker> p1 = std::make_shared<ClientWorker>(1, b0);
    auto ops = randomOperations();
    GeneratorPtr gen = std::make_shared<VectorGenerator>(ops);
    JepsenContext ctx(1);
    p1->open(0);

    while (true) {
        auto op = gen->op(ctx);
        LOG4CPLUS_INFO(logger, op.toString().c_str());
        if (op.type == Operation::kExit) {
            break;
        }
        p1->invoke(op);
        LOG4CPLUS_INFO(logger, op.toString().c_str());
    }

    {
        auto w1 = OperationFactory::write("x", 1);
        p1->invoke(w1);
        LOG4CPLUS_INFO(logger, w1.toString().c_str());
    }

    {
        auto r1 = OperationFactory::read("x");
        p1->invoke(r1);
        LOG4CPLUS_INFO(logger, r1.toString().c_str());
    }

    {
        auto r1 = OperationFactory::read("x");
        p1->invoke(r1);
        LOG4CPLUS_INFO(logger, r1.toString().c_str());
    }
}

void testNemesisFuture() {
    std::future<bool> nf;
    nf = std::async(&Nemesis::setup, std::move(Nemesis::createOne()));  // nemesis future
    nf.get();

    nf = std::async(&Nemesis::teardown, std::move(Nemesis::createOne()));
    nf.get();
}

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.cfg"));

    // testNemesisFuture();
    testETCDWorker();

    return 0;
}