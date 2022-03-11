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
}
void Runner::setClient(shared_ptr<Client>& client) {
    this->client = client;
}

void Runner::setNemesis(shared_ptr<Nemesis>& nemesis) {
    this->nemesis = nemesis;
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


    for (auto node : nodes) {
        os->teardown(node);
    }
    LOG4CPLUS_INFO(logger, "Runner end teardown remote operating system");
    LOG4CPLUS_INFO(logger, "Runner end running");
}