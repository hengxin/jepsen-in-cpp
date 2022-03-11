#ifndef CONTROL_H
#define CONTROL_H

#include "SSHRemote.h"
#include "base64.h"

using RemotePtr = shared_ptr<SSHRemote>;

const string kTempDir = "/tmp/jepsen";
const string kWgetCacheDir = kTempDir + "/wget-cache";

class ControlHelper {
public:
    static bool install_archive(RemotePtr& remote, string url, string dir, bool force);
    static log4cplus::Logger logger;
};

#endif