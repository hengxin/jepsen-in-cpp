#include "SSHChannel.h"

namespace jepsen {
LIBSSH2_CHANNEL* SSHChannel::createChannel(LIBSSH2_SESSION* session) {
    LIBSSH2_CHANNEL* temp;
    while ((temp = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    return temp;
}

bool SSHChannel::close(LIBSSH2_SESSION* session) {
    int exitcode = 127;
    int rc;
    char* exitsignal = (char*)"none";
    while ((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN)

        waitsocket(sock, session);

    if (rc == 0) {
        exitcode = libssh2_channel_get_exit_status(channel);

        libssh2_channel_get_exit_signal(channel,
                                        &exitsignal,

                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
    }

    bool flag = true;
    if (exitsignal) {
        LOG4CPLUS_DEBUG(logger, "Got signal: " << exitsignal);
        flag = false;
    } else {
        if (exitcode != 0) {
            LOG4CPLUS_DEBUG(logger, "EXIT: " << exitcode);
        }
        flag = exitcode == 0;
    }

    libssh2_channel_free(channel);
    return flag;
}

bool SSHChannel::exec(const string command,
                      LIBSSH2_SESSION* session,
                      string& channel_stdout,
                      string& channel_stderr) {
    int rc;
    while ((rc = libssh2_channel_exec(channel, command.c_str())) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    if (rc != 0) {
        LOG4CPLUS_ERROR(logger, "Error in SSHChannel::exec, rc != 0 \n");
        exit(1);
    }
    for (;;) {
        /* loop until we block */
        int rc;
        int bytecount = 0;
        do {
            char buffer[0x4000];
            rc = libssh2_channel_read(channel, buffer, sizeof(buffer));

            if (rc > 0) {
                int i;
                bytecount += rc;
                string buf = string(buffer, buffer + rc);
                channel_stdout += buf;
                LOG4CPLUS_DEBUG(logger, "STDOUT:" << std::endl << buf.c_str());
            } else {
                if (rc != LIBSSH2_ERROR_EAGAIN)
                    /* no need to output this for the EAGAIN case */
                    if (rc != 0) {
                        LOG4CPLUS_DEBUG(logger, "libssh2_channel_read returned " << rc);
                    }
            }
        } while (rc > 0);

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            waitsocket(sock, session);
        } else
            break;
    }

    for (;;) {
        /* loop until we block */
        int rc;
        int bytecount = 0;
        do {
            char buffer[0x4000];
            rc = libssh2_channel_read_stderr(channel, buffer, sizeof(buffer));

            if (rc > 0) {
                int i;
                bytecount += rc;
                string buf = string(buffer, buffer + rc);
                channel_stderr += buf;
                LOG4CPLUS_DEBUG(logger, "STDERR:" << std::endl << buf.c_str());
            } else {
                if (rc != LIBSSH2_ERROR_EAGAIN) {
                    /* no need to output this for the EAGAIN case */
                    if (rc != 0) {
                        LOG4CPLUS_DEBUG(logger, "libssh2_channel_read_stderr returned " << rc);
                    }
                }
            }
        } while (rc > 0);

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            waitsocket(sock, session);
        } else
            break;
    }

    return true;
}
}  // namespace jepsen