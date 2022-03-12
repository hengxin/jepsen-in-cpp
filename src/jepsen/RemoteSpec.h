#ifndef REMOTE_SPEC_H
#define REMOTE_SPEC_H

#include "include.h"

struct ConnSpec {
    string host;
    int port;
    string username;
    string password;
    string private_key_path;
    bool strict_host_key_checking;
    ConnSpec(string host,
             int port,
             string username,
             string password,
             string private_key_path,
             bool strict_host_key_checking)
        : host(host),
          port(port),
          username(username),
          password(password),
          private_key_path(private_key_path),
          strict_host_key_checking(strict_host_key_checking) {}
};

struct Context {
    string dir;
    string sudo;
    string password;
    Context() = default;
    Context(string dir, string sudo, string password) : dir(dir), sudo(sudo), password(password) {}
};

#endif REMOTE_SPEC_H