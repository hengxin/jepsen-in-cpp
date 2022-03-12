#ifndef REMOTE_H
#define REMOTE_H

#include "RemoteSpec.h"
#include "include.h"

class Remote {
public:
    virtual void connect(ConnSpec conn_spec) = 0;
    virtual void disconnect() = 0;
    // TODO: How to express stdout and stderr if execute failed
    virtual int execute(Context ctx, string cmd, string in) = 0;
    virtual int execute(Context ctx, string cmd, string in, string& _stdout, string& _stderr) = 0;
    virtual void upload(Context ctx, string local_path, string remote_path) = 0;
    virtual void download(Context ctx, string remote_path, string local_path) = 0;
};

#endif