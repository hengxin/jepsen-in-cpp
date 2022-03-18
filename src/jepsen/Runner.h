#ifndef RUNNER_H
#define RUNNER_H

#include "Checker.h"
#include "Database.h"
#include "Generator.h"
#include "OS.h"
#include "Operation.h"
#include "SSHRemote.h"
#include "Worker.h"
#include "include/log4cplus.h"
#include <chrono>
#include <execution>
#include <future>

namespace jepsen {
struct SSHInfo {
    string username;
    string password;
    string sudo_password;
    int port;
    string private_key_path;
    bool strict_host_key_checking;
    SSHInfo(string username,
            string password,
            string sudo_password,
            int port,
            string private_key_path,
            bool strict_host_key_checking)
        : username(username),
          password(password),
          sudo_password(sudo_password),
          port(port),
          private_key_path(private_key_path),
          strict_host_key_checking(strict_host_key_checking) {}
};

class Runner {
public:
    Runner() = delete;
    Runner(std::vector<string>& nodes, int concurrency, SSHInfo& ssh, bool leave_db_running = false)
        : nodes(nodes),
          concurrency(concurrency),
          ssh(ssh),
          logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))),
          leave_db_running(leave_db_running) {
        this->initLogger();
        this->initRemotes();
        this->setClientAndNemesis();
    }
    void run();
    void initLogger();
    void initRemotes();
    void setOS(OSPtr& os);
    void setDB(DBPtr& db);
    void setClientAndNemesis();
    void setGenerator(shared_ptr<Generator>& generator);
    void setChecker(shared_ptr<Checker>& checker);

    // OS
    void setupOS();
    void teardownOS();
    // DB
    void setupDB();
    void teardownDB();
    // Client or Nemesis
    void setupClientNemesis();
    void teardownClientNemesis();

    // Parallel
    void withLoggerNDC(string node, std::function<void()> f);

private:
    std::vector<string> nodes;
    int concurrency;
    SSHInfo ssh;
    OSPtr os;
    DBPtr db;

    shared_ptr<Generator> generator;
    shared_ptr<Checker> checker;

    std::vector<shared_ptr<Worker>> workers;
    std::vector<Operation> history;

    // TODO: shared a shared_ptr<unordered_map> ?
    std::unordered_map<string, SSHRemotePtr> remotes;
    //
    std::future<bool> nf;
    std::vector<ClientPtr> clients;
    std::vector<int> indexs;


    log4cplus::Logger logger;

    bool leave_db_running;
};
}  // namespace jepsen

#endif