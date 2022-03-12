#ifndef SSH_CLIENT_CONFIG_H
#define SSH_CLIENT_CONFIG_H

#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_SYS_SELECT_H
#define HAVE_UNISTD_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_TIME_H
#define HAVE_STDLIB_H

#include <iostream>
#include <libssh2.h>
#include <memory>
#include <mutex>
#include <string>

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/initializer.h>
#include <log4cplus/layout.h>
#include <log4cplus/logger.h>  // log4cplus
#include <log4cplus/loggingmacros.h>

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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