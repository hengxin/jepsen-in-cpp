#ifndef SSH_CHANNEL_H
#define SSH_CHANNEL_H

#include "SSHConfig.h"

using std::string;

class SSHChannel {
public:
    SSHChannel() = delete;
    explicit SSHChannel(LIBSSH2_SESSION* session, int socket) : sock(socket), session(session) {
        if (session == nullptr) {
            fprintf(stderr, "SSHChannel::SSHChannel: session is nullptr\n");
        }

        channel = createChannel(session);
        if (channel == nullptr) {
            fprintf(stderr, "SSHChannel::SSHChannel Error to initial channel\n");
            exit(-1);  // TODO:
        }
    }
    ~SSHChannel() {
        // close(session);
    }


    LIBSSH2_CHANNEL* createChannel(LIBSSH2_SESSION* session);
    bool close(LIBSSH2_SESSION* session);

    bool exec(const string command, LIBSSH2_SESSION* session);

private:
    int sock;
    LIBSSH2_CHANNEL* channel;
    LIBSSH2_SESSION* session;
};


#endif