#ifndef RUNNER_H
#define RUNNER_H

#include "include.h"
#include "OS.h"
#include "Database.h"
#include "Operation.h"
#include "Client.h"
#include "Nemesis.h"
#include "Generator.h"
#include "Checker.h"
#include "SSHRemote.h"

struct SSHInfo {
    string username;
    string password;
    string sudo_password;
    int port;
    string private_key_path;
    bool strict_host_key_checking;
    SSHInfo(string username, string password, string sudo_password, int port, string private_key_path, bool strict_host_key_checking)
        :username(username), password(password), sudo_password(sudo_password), port(port), private_key_path(private_key_path), strict_host_key_checking(strict_host_key_checking) {}
};

class Runner {
public:
    Runner() = delete;
    Runner(vector<string>& nodes, int concurrency, SSHInfo& ssh)
        : nodes(nodes), concurrency(concurrency), ssh(ssh), logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {
        this->initLogger();
        this->initRemotes();
    }
    void run();
    void initLogger();
    void initRemotes();
    
private:
    vector<string> nodes;
    int concurrency;
    SSHInfo ssh;
    OS os;
    DB db;
    Client client;
    Nemesis nemesis;
    Generator generator;
    Checker checker;
    vector<Operation> history;

    // vector<SSHClient> ssh_clients;
    unordered_map<string, unique_ptr<SSHRemote>> remotes;

    log4cplus::Logger logger;
    bool leave_db_running;
};

#endif