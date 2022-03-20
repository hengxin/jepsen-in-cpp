#include "Runner.h"

namespace jepsen {
using namespace log4cplus;

/***
 * https://sourceforge.net/p/log4cplus/wiki/CodeExamples/
 */
void Runner::initLogger() {
    try {
        // log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);
        // Logger::getRoot().setLogLevel(INFO_LOG_LEVEL);
        // log4cplus::helpers::Properties props;
        // props.setProperty(LOG4CPLUS_TEXT ("AsyncAppend"), LOG4CPLUS_TEXT ("true"));
        // log4cplus::helpers::SharedObjectPtr<Appender> append(new ConsoleAppender(props));
        // append->setLayout(std::unique_ptr<Layout>(new log4cplus::TTCCLayout));
        // Logger::getRoot().addAppender(append);
        // append->setName(LOG4CPLUS_TEXT("cout-append"));
    } catch (std::exception& e) {
        LOG4CPLUS_FATAL(Logger::getRoot(), "runner()- Exception occurred: " << e.what());
    } catch (...) {
        LOG4CPLUS_FATAL(Logger::getRoot(), "runner()- Exception occurred");
    }
}

void Runner::initRemotes() {
    LOG4CPLUS_INFO(logger, "Start Initialize Remotes");
    for (auto ip_addr : nodes) {
        ConnSpec conn_spec(ip_addr,
                           ssh.port,
                           ssh.username,
                           ssh.password,
                           ssh.private_key_path,
                           ssh.strict_host_key_checking);
        remotes.emplace(ip_addr, SSHRemotePtr(new SSHRemote(10, conn_spec)));
    }
    LOG4CPLUS_INFO(logger, "End Initialize Remotes");
}

void Runner::setOS(OSPtr& os) {
    LOG4CPLUS_INFO(logger, "Set Operating System");
    this->os = os;
    this->os->initRemotes(remotes);
}

void Runner::setDB(DBPtr& db) {
    this->db = db;
    this->db->initRemotes(remotes);
}


void Runner::setGenerator(GeneratorPtr& generator) {
    this->generator = generator;
}

void Runner::setChecker(shared_ptr<Checker>& checker) {
    this->checker = checker;
}

void Runner::withLoggerNDC(string node, std::function<void()> f) {
    log4cplus::NDC& ndc = log4cplus::getNDC();
    log4cplus::tstring name(node.begin(), node.end());
    log4cplus::NDCContextCreator _first_ndc(name);
    f();
    ndc.remove();
}

void Runner::setupOS() {
    LOG4CPLUS_INFO(logger, "Start setup remote operating system");
    {
        std::for_each(std::execution::par, nodes.begin(), nodes.end(), [this](auto node) {
            withLoggerNDC(node, [this, &node] { os->setup(node); });
        });
    }
    LOG4CPLUS_INFO(logger, "Finish setup remote operating system");
}

void Runner::teardownOS() {
    LOG4CPLUS_INFO(logger, "Start teardown remote operating system");
    {
        std::for_each(std::execution::par, nodes.begin(), nodes.end(), [this](auto node) {
            withLoggerNDC(node, [this, &node] { os->teardown(node); });
        });
    }
    LOG4CPLUS_INFO(logger, "Finish teardown remote operating system");
}
// DB
void Runner::setupDB() {
    LOG4CPLUS_INFO(logger, "Start setup remote database");
    {
        std::for_each(std::execution::par, nodes.begin(), nodes.end(), [this](auto node) {
            withLoggerNDC(node, [this, &node] { db->setup(node); });
        });
    }
    // Setup Primary if it exists
    {
        DBPrimaryInterface* primary_db;
        if ((primary_db = dynamic_cast<DBPrimaryInterface*>(db.get())) != nullptr) {
            LOG4CPLUS_INFO(logger, "Runner set up primaries of databases");
            auto primaries = primary_db->getPrimaries();

            for (auto primary : primaries) {
                primary_db->setupPrimary(primary);
            }
        } else {
            LOG4CPLUS_INFO(logger, "No Primary");
        }
    }
    LOG4CPLUS_INFO(logger, "Finish setup remote database");
}

void Runner::teardownDB() {
    LOG4CPLUS_INFO(logger, "Start teardown remote database");
    {
        std::for_each(std::execution::par, nodes.begin(), nodes.end(), [this](auto node) {
            withLoggerNDC(node, [this, &node] { db->teardown(node); });
        });
    }
    LOG4CPLUS_INFO(logger, "Finish teardown remote database");
}

void Runner::setupClientNemesis() {
    nf = std::async(&Nemesis::setup, std::move(Nemesis::createOne()));  // nemesis future

    for (int i = 0; i < nodes.size(); i++) {
        clients.emplace_back(Client::createOne());
        indexs.push_back(i);
    }
    std::for_each(std::execution::par, indexs.begin(), indexs.end(), [this](auto i) {
        auto& client = clients[i];
        auto& node = nodes[i];
        withLoggerNDC(node, [&node, &client] {
            client->open(node);
            client->setup();
        });
    });
    nf.get();
}

void Runner::teardownClientNemesis() {
    nf = std::async(&Nemesis::teardown, std::move(Nemesis::createOne()));
    std::for_each(std::execution::par, indexs.begin(), indexs.end(), [this](auto i) {
        auto& client = clients[i];
        auto& node = nodes[i];
        withLoggerNDC(node, [&node, &client] {
            client->teardown();
            client->close();
        });
    });
    nf.get();
}

void Runner::saveHistory() {
    for (auto& op : history) {
        LOG4CPLUS_INFO(logger, op.toString().c_str());
    }
}

void Runner::runCases() {
    JepsenContext ctx(concurrency);
    OperationQueuePtr completions = std::make_shared<OperationQueue>(concurrency + 1);

    // workers[concurrency] is nemesis and others are clients
    workers.reserve(concurrency + 1);
    for (int i = 0; i < concurrency; i++) {
        auto node = nodes[i % nodes.size()];
        workers.emplace_back(std::make_shared<ClientWorker>(i, completions, node));
        process_to_thread[i] = i;
    }
    workers.emplace_back(std::make_shared<NemesisWorker>(0, completions));
    process_to_thread[kNemesisProcess] = kNemesisProcess;

    for (auto& worker : workers) {
        worker->run();
    }

    int outstanding = 0;
    auto poll_timeout = microseconds(0);  // milliseconds

    while (true) {
        Operation op;
        if (completions->wait_dequeue_timed(op, poll_timeout)) {
            auto thread = process_to_thread[op.process];
            auto& worker = workers[thread];  // TODO: process and thread , mod?
            auto time = std::chrono::system_clock::now().time_since_epoch().count();
            op.time = time;
            ctx.time = time;
            ctx.free_threads.insert(thread);
            generator->update(ctx, op);
            // it is not nemesis
            if (op.process != kNemesisProcess && op.type == Operation::kInfo) {
                auto next_process = op.process + concurrency;
                process_to_thread[next_process] = process_to_thread[op.process];
                process_to_thread.erase(op.process);
            }
            history.emplace_back(op);
            outstanding--;
            poll_timeout = microseconds(0);
        } else {
            auto time = std::chrono::system_clock::now().time_since_epoch().count();
            ctx.time = time;
            auto op = generator->op(ctx);
            switch (op.type) {
                case Operation::kNil:
                case Operation::kExit: {
                    if (outstanding > 0) {
                        poll_timeout = kMaxPendingInterval;
                        continue;
                    } else {
                        std::for_each(
                            std::execution::par, workers.begin(), workers.end(), [](auto& worker) {
                                worker->putOp(OperationFactory::exit());
                                worker->exit();
                            });
                        saveHistory();
                    }
                } break;
                case Operation::kPending: {
                    poll_timeout = kMaxPendingInterval;
                    continue;
                } break;
                default: {
                    if (time < op.time) {
                        poll_timeout = microseconds((op.time - time) / 1000);
                        continue;
                    } else {
                        auto thread = process_to_thread[op.process];
                        auto& worker = workers[thread];
                        worker->putOp(op);
                        ctx.time = op.time;
                        ctx.free_threads.erase(thread);
                        generator->update(ctx, op);
                        history.emplace_back(op);
                        outstanding++;
                        poll_timeout = microseconds(0);
                    }
                };
            }
        }
    }
}

void Runner::run() {
    LOG4CPLUS_INFO(logger, "Jepsen-Runner start running");

    setupOS();
    teardownDB();
    setupDB();

    relative_time = std::chrono::system_clock::now().time_since_epoch().count();
    setupClientNemesis();
    runCases();
    teardownClientNemesis();
    if (!leave_db_running) {
        teardownDB();
    } else {
        LOG4CPLUS_INFO(logger, "Skip teardown remote database(leave db running)");
    }

    teardownOS();
    LOG4CPLUS_INFO(logger, "Jepsen-Runner finish running");
}

}  // namespace jepsen