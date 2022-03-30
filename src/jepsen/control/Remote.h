#ifndef REMOTE_H
#define REMOTE_H

#include "RemoteSpec.h"


namespace jepsen {
class Remote {
public:
    virtual void connect(ConnSpec conn_spec) = 0;
    virtual void disconnect() = 0;
    // TODO: How to express stdout and stderr if execute failed
    virtual int execute(SSHContext ctx, string cmd, string in) = 0;
    virtual int execute(
        SSHContext ctx, string cmd, string in, string& _stdout, string& _stderr) = 0;
    virtual void upload(SSHContext ctx, string local_path, string remote_path) = 0;
    virtual void download(SSHContext ctx, string remote_path, string local_path) = 0;
};
}  // namespace jepsen

#endif