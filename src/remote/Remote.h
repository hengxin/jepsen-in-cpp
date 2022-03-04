#ifndef REMOTE_H
#define REMOTE_H

class Remote{
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void execute() = 0;
    virtual void upload() = 0;
    virtual void download() = 0;
};

#endif