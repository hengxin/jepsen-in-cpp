#include "Runner.h"

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
        remotes.emplace(ip_addr, shared_ptr<SSHRemote>(new SSHRemote(10, conn_spec)));
    }
    LOG4CPLUS_INFO(logger, "End Initialize Remotes");
}

void Runner::setOS(shared_ptr<OS>& os) {
    LOG4CPLUS_INFO(logger, "Set Operating System");
    this->os = os;
    this->os->initRemotes(remotes);
}

void Runner::setDB(shared_ptr<DB>& db) {
    this->db = db;
    this->db->initRemotes(remotes);
}

void Runner::setClientAndNemesis() {
    workers.reserve(concurrency + 1);  // workers[0] is nemesis and others are clients
    workers.emplace_back(std::make_shared<NemesisWorker>(0));
    for (int i = 1; i <= concurrency; i++) {
        auto node = nodes[(i - 1) % nodes.size()];
        workers.emplace_back(std::make_shared<ClientWorker>(i, node));
    }
}


void Runner::setGenerator(shared_ptr<Generator>& generator) {
    this->generator = generator;
}

void Runner::setChecker(shared_ptr<Checker>& checker) {
    this->checker = checker;
}


void Runner::run() {
    LOG4CPLUS_INFO(logger, "Runner start running");

    LOG4CPLUS_INFO(logger, "Runner start setup remote operating system");
    for (auto node : nodes) {
        os->setup(node);
    }

    LOG4CPLUS_INFO(logger, "Runner start teardown remote database");
    for (auto node : nodes) {
        os->teardown(node);
    }

    LOG4CPLUS_INFO(logger, "Runner start setup remote database");
    for (auto node : nodes) {
        db->setup(node);
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
            LOG4CPLUS_INFO(logger, "NO Primary");
        }
    }


    if (!leave_db_running) {
        for (auto node : nodes) {
            db->teardown(node);
        }
        LOG4CPLUS_INFO(logger, "Runner finish teardown remote database");
    } else {
        LOG4CPLUS_INFO(logger, "Runner skip teardown remote database(leave db running)");
    }

    for (auto node : nodes) {
        os->teardown(node);
    }
    LOG4CPLUS_INFO(logger, "Runner finish teardown remote operating system");
    LOG4CPLUS_INFO(logger, "Runner finish running");
}