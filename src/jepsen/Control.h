#ifndef CONTROL_H
#define CONTROL_H

#include "SSHRemote.h"
#include "base64.h"
#include <boost/algorithm/string.hpp>

using RemotePtr = shared_ptr<SSHRemote>;

const string kTempDir = "/tmp/jepsen";
const string kWgetCacheDir = kTempDir + "/wget-cache";

class ControlHelper {
public:
    static bool installArchive(RemotePtr& remote, string url, string dest, bool force = false);
    static string cachedWget(RemotePtr& remote, string url, bool force = false);
    static bool exists(RemotePtr& remote, string filename);
    static string tmpFile(RemotePtr& remote);
    static string tmpDir(RemotePtr& remote);


    static log4cplus::Logger logger;
};

#endif