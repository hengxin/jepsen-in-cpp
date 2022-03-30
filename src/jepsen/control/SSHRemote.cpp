#include "SSHRemote.h"

namespace jepsen {
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

int SSHRemote::execute(SSHContext ctx, string cmd, string in) {
    string out, err;
    return ssh_client.execute(cmd, out, err);
}

int SSHRemote::execute(SSHContext ctx, string cmd, string in, string& _stdout, string& _stderr) {
    return ssh_client.execute(cmd, _stdout, _stderr);
}

int SSHRemote::execute(string cmd, string& channel_stdout, string& channel_stderr) {
    return execute(ctx, cmd, "", channel_stdout, channel_stderr);
}

int SSHRemote::execute(string cmd) {
    return execute(ctx, cmd, "");
}

int SSHRemote::sudoExecute(string cmd, string& channel_stdout, string& channel_stderr) {
    /* method 1: echo $pwd | sudo -S cmd */
    // boost::format fmt("echo %s | sudo -S %s");
    // fmt % conn_spec.password % cmd;

    /* method 2: echo $pwd | sudo bash -c "cmd" */
    boost::format fmt("echo %s | sudo bash -c \"%s\"");
    fmt % conn_spec.password % cmd;
    return execute(fmt.str(), channel_stdout, channel_stderr);
}

int SSHRemote::sudoExecute(string cmd) {
    string out;
    string err;
    return sudoExecute(cmd, out, err);
}


void SSHRemote::upload(SSHContext ctx, string local_path, string remote_path) {}

void SSHRemote::download(SSHContext ctx, string remote_path, string local_path) {}
}  // namespace jepsen