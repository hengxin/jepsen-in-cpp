#include "SSHChannel.h"


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

    if (exitsignal)
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
    else
        fprintf(stderr, "\nEXIT: %d\n", exitcode);

    libssh2_channel_free(channel);
    return true;
}

bool SSHChannel::exec(const string command, LIBSSH2_SESSION* session) {
    int rc;
    while ((rc = libssh2_channel_exec(channel, command.c_str())) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    if (rc != 0) {
        fprintf(stderr, "Error\n");
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
                fprintf(stderr, "We read:\n");
                for (i = 0; i < rc; ++i)
                    fputc(buffer[i], stderr);
                fprintf(stderr, "\n");
            } else {
                if (rc != LIBSSH2_ERROR_EAGAIN)
                    /* no need to output this for the EAGAIN case */
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
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