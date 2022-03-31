#include "ETCD.h"
#ifndef WIN32
#include "ETCDClient.h"
#endif
#include "Runner.h"

using namespace jepsen;
using std::vector;

// public-cd-b{0-2}.disalg.cn
const string b0 = "47.108.193.81";
const string b1 = "47.108.227.187";
const string b2 = "47.108.208.93";

int main() {
    log4cplus::Initializer initializer;
#ifdef LOG4CPLUS_CONFIG
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(LOG4CPLUS_CONFIG));
#endif

#ifdef WIN32
    Client::Register<NoopClient>();
#else
    Client::Register<ETCDClient>();
#endif
    Nemesis::Register<NoopNemesis>();

    vector<string> nodes{b0, b1, b2};
    SSHInfo ssh("young", "guazi13110", "guazi13110", 22, "/home/young/.ssh/id_rsa", false);
    Runner runner(nodes, 5, ssh, true);
    // Initialize Operating System
    OSPtr os = std::make_shared<Ubuntu>();
    runner.setOS(os);
    // Initialize Database
    DBPtr db = std::make_shared<ETCD>(ETCD(nodes));
    runner.setDB(db);
    // Initialize Generator
    std::list<Operation> ops;

    string keys = "abcdefghijklmn";
    for (int i = 0; i < 100; i++) {
        for (auto key : keys) {
            ops.push_back(OperationFactory::read(key));
            ops.push_back(OperationFactory::write(key, "1"));
            ops.push_back(OperationFactory::read(key));
            ops.push_back(OperationFactory::write(key, "2"));
            ops.push_back(OperationFactory::read(key));
        }
    }

    auto r = [&keys]() { return OperationFactory::read(keys[rand() % keys.size()]); };

    auto w = [&keys]() {
        return OperationFactory::write(keys[rand() % keys.size()], rand() % keys.size());
    };

    auto n = []() { return OperationFactory::fakePartition(); };

    vector<generator::GeneratorPtr> gens;
    gens.push_back(generator::GeneratorFactory::createGenerator(r));
    gens.push_back(generator::GeneratorFactory::createGenerator(w));
    auto partition = generator::GeneratorFactory::createGenerator(n);

    generator::GeneratorPtr gen = mix(gens);
    gen = generator::stagger(1, gen);
    gen = generator::nemesis(partition, gen);
    gen = generator::timeLimit(5, gen);

    runner.setGenerator(gen);

    runner.run();

    return 0;
}