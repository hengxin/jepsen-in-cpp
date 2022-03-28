#ifndef SSH_CLIENT_CONFIG_H
#define SSH_CLIENT_CONFIG_H

#include "include/log4cplus.h"
#include "libssh2_config.h"
#include <iostream>
#include <libssh2.h>
#include <memory>
#include <mutex>
#include <string>

static int waitsocket(int socket_fd, LIBSSH2_SESSION* session) {
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set* writefd = NULL;
    fd_set* readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);


    if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

#endif