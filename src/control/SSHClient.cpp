#include "SSHClient.h"
#include "SSHChannel.h"

bool SSHClient::connectTo(){

    fprintf(stderr, "Call SSHClient::connectTo\n");

    struct sockaddr_in sin;
    int rc;
    LIBSSH2_KNOWNHOSTS* nh;
    size_t len;
    int type;

    /***
     * Create a socket.
     * AF_INET: IP Protocol
     * SOCK_STREAM: Sequenced, reliable, connection-based byte streams.  
     **/
    sock = socket(AF_INET, SOCK_STREAM, 0); 
    
    sin.sin_family = AF_INET; // IP
    sin.sin_port = htons(port); // Port
    sin.sin_addr.s_addr = host_addr; // unsigned long for ip address
    
    if(connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "SSHClient::connectTo failed to connect by a socket!\n");
        return false;
    }

    /***
     * Create a session instance and start it up.
     * This will trade welcome banners, exchange keys, and setup crypto, compression, and MAC layers
     * */
    session = libssh2_session_init();
    if(session == nullptr) {
        fprintf(stderr, "SSHClient::connectTo failed to init session!\n");
        return false;
    }

    /* TODO: should we block here? tell libssh2 we want it all done non-blocking */ 
    libssh2_session_set_blocking(session, 0);

    /* sockert和session握手通信 */
    while((rc = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN);
    
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return false;
    }

    nh = libssh2_knownhost_init(session);
    if(nh == nullptr) {
        /* eeek, do cleanup here */ 
        return false;
    }

    /* TODO: read all hosts from here */ 
    libssh2_knownhost_readfile(nh, "/home/young/.ssh/known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);
 
    /* TODO: store all known hosts to here */ 
    libssh2_knownhost_writefile(nh, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* Get the remote key */
    const char *fingerprint = libssh2_session_hostkey(session, &len, &type);

    if(fingerprint) {
        struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */ 
        /* check a host+key against the list of known hosts */
        int check = libssh2_knownhost_checkp(nh, ip_addr.c_str(), port,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);
#else
        /* 1.2.5 or older */ 
        int check = libssh2_knownhost_check(nh, ip_addr.c_str(),
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
        fprintf(stderr, "Host check: %d, key: %s\n", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                host->key:"<none>");
        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/ 
    }else{
        /* eeek, do cleanup here */ 
        return false;
    }
    
    libssh2_knownhost_free(nh);

    if(password.size() != 0) {
        /* We could authenticate via password */ 
        while((rc = libssh2_userauth_password(session, username.c_str(), password.c_str())) == LIBSSH2_ERROR_EAGAIN);    
        if(rc) {
            fprintf(stderr, "\tAuthentication by password failed\n");
            disConnect();
            return false;    
        }
    }else {
        /* Or by public key */ 
        while((rc = libssh2_userauth_publickey_fromfile(session, username.c_str(),
                                                         "/home/young/"
                                                         ".ssh/id_rsa.pub",
                                                         "/home/young/"
                                                         ".ssh/id_rsa",
                                                         password.c_str())) ==
               LIBSSH2_ERROR_EAGAIN);
        if(rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            disConnect();
            return false;    
        }
    }

    return true;
}

bool SSHClient::disConnect(){
    fprintf(stderr, "SSHClient::disConnect\n");
    if(session != nullptr){
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        session = nullptr;
    }
    if(sock != kSocketInit){
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        sock = kSocketInit;
    }

    return true;
}

void SSHClient::execute(const string command){
    fprintf(stderr, "--- Executing %s\n", command.c_str());
    SSHChannel channel(session, sock);
    channel.exec(command, session);
}