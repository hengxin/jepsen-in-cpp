#ifndef RUNNER_H
#define RUNNER_H

#include "Checker.h"
#include "Client.h"
#include "Database.h"
#include "Generator.h"
#include "Nemesis.h"
#include "OS.h"
#include "Operation.h"
#include "SSHRemote.h"
#include "include.h"

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
    Runner(vector<string>& nodes, int concurrency, SSHInfo& ssh)
        : nodes(nodes),
          concurrency(concurrency),
          ssh(ssh),
          logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {
        this->initLogger();
        this->initRemotes();
    }
    void run();
    void initLogger();
    void initRemotes();
    void setOS(shared_ptr<OS>& os);
    void setDB(shared_ptr<DB>& db);
    void setClient(shared_ptr<Client>& client);
    void setNemesis(shared_ptr<Nemesis>& nemesis);
    void setGenerator(shared_ptr<Generator>& generator);
    void setChecker(shared_ptr<Checker>& checker);

private:
    vector<string> nodes;
    int concurrency;
    SSHInfo ssh;
    shared_ptr<OS> os;
    shared_ptr<DB> db;
    shared_ptr<Client> client;
    shared_ptr<Nemesis> nemesis;
    shared_ptr<Generator> generator;
    shared_ptr<Checker> checker;
    vector<Operation> history;

    // vector<SSHClient> ssh_clients;
    unordered_map<string, shared_ptr<SSHRemote>> remotes;

    log4cplus::Logger logger;
    bool leave_db_running;
};

#endif