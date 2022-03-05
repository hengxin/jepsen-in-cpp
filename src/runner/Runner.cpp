#include "Runner.h"

using namespace log4cplus;

/***
 * https://sourceforge.net/p/log4cplus/wiki/CodeExamples/
*/
void Runner::initLogger() {
    try {
        log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);
        logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));
        Logger::getRoot().setLogLevel(INFO_LOG_LEVEL);
        log4cplus::helpers::Properties props;
        props.setProperty(LOG4CPLUS_TEXT ("AsyncAppend"), LOG4CPLUS_TEXT ("true"));
        log4cplus::helpers::SharedObjectPtr<Appender> append(new ConsoleAppender(props));
        append->setLayout(std::unique_ptr<Layout>(new log4cplus::TTCCLayout));
        Logger::getRoot().addAppender(append);
        append->setName(LOG4CPLUS_TEXT("cout-append"));
    }catch(std::exception &e) {
        LOG4CPLUS_FATAL(Logger::getRoot(), "runner()- Exception occurred: " << e.what());
    }catch(...) {
        LOG4CPLUS_FATAL(Logger::getRoot(), "runner()- Exception occurred");
    }

}

void Runner::initSSHClients() {
    LOG4CPLUS_INFO(logger, "Start Initialize SSHClients");
    ssh_clients.reserve(nodes.size());
    for(auto ip_addr: nodes){
        ssh_clients.emplace_back(ip_addr, ssh.username, ssh.password, ssh.port);
    }
    LOG4CPLUS_INFO(logger, "End Initialize SSHClients");
}

void Runner::run() {
    LOG4CPLUS_INFO(logger, "Runner start running");
    // 获取到各个节点的链接
    for(auto& client: ssh_clients){
        client.execute("uptime");
    }
    
    LOG4CPLUS_INFO(logger, "Runner end running");
}