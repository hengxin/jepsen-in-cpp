#include "SSHRemote.h"

SSHRemote& SSHRemote::operator=(const SSHRemote& rhs) {
    if (this == &rhs) {
        return *this;
    }
    this->concurrency_limit = rhs.concurrency_limit;
    this->conn_spec = rhs.conn_spec;
    this->ssh_client = rhs.ssh_client;
    return *this;
}

void SSHRemote::connect(ConnSpec conn_spec) {}

void SSHRemote::disconnect() {}

int SSHRemote::execute(Context ctx, string cmd, string in) {
    return ssh_client.execute(cmd);
}

int SSHRemote::execute(Context ctx, string cmd, string in, string& _stdout, string& _stderr) {
    return ssh_client.execute(cmd, _stdout, _stderr);
}

int SSHRemote::execute(string cmd, string& channel_stdout, string& channel_stderr) {
    return execute(ctx, cmd, "", channel_stdout, channel_stderr);
}

int SSHRemote::execute(string cmd) {
    return execute(ctx, cmd, "");
}

int SSHRemote::sudoExecute(string cmd, string& channel_stdout, string& channel_stderr) {
    boost::format fmt("echo %s | sudo -S %s");
    fmt % conn_spec.password % cmd;
    return execute(fmt.str(), channel_stdout, channel_stderr);
}

int SSHRemote::sudoExecute(string cmd) {
    boost::format fmt("echo %s | sudo -S %s");
    fmt % conn_spec.password % cmd;
    return execute(fmt.str());
}


void SSHRemote::upload(Context ctx, string local_path, string remote_path) {}

void SSHRemote::download(Context ctx, string remote_path, string local_path) {}