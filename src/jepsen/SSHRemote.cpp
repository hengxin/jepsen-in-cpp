#include "SSHRemote.h"

SSHRemote& SSHRemote::operator=(const SSHRemote& rhs){
    if(this == &rhs){
        return *this;
    }
    this->concurrency_limit = rhs.concurrency_limit;
    this->conn_spec = rhs.conn_spec;
    this->ssh_client = rhs.ssh_client;
    return *this;
}

void SSHRemote::connect(ConnSpec conn_spec) {

}

void SSHRemote::disconnect() {

}

int SSHRemote::execute(Context ctx, string cmd, string in) {

} 

void SSHRemote::upload(Context ctx, string local_path, string remote_path) {

}

void SSHRemote::download(Context ctx, string remote_path, string local_path) {

}  