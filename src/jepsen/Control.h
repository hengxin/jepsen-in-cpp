#ifndef CONTROL_H
#define CONTROL_H

#include "SSHRemote.h"
#include "base64.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

namespace jepsen {
using RemotePtr = shared_ptr<SSHRemote>;
using std::optional;

const string kTempDir = "/tmp/jepsen";
const string kWgetCacheDir = kTempDir + "/wget-cache";

class ControlHelper {
public:
    struct StartDaemonOptions {
        optional<string> env;  //
        optional<bool> background;
        optional<string> chdir;
        optional<string> logfile;
        optional<bool> make_pidfile;
        optional<bool> match_executable;
        optional<bool> match_process_name;
        optional<string> pidfile;
        optional<string> process_name;
    };

    static bool installArchive(RemotePtr& remote, string url, string dest, bool force = false);
    static string cachedWget(RemotePtr& remote, string url, bool force = false);
    static bool exists(RemotePtr& remote, string filename);
    static string tmpFile(RemotePtr& remote);
    static string tmpDir(RemotePtr& remote);
    static void startDaemon(RemotePtr& remote, StartDaemonOptions& opts, string bin, string args);
    static void stopDaemon(RemotePtr& remote, string cmd, string pidfile);

    static log4cplus::Logger logger;
};
}  // namespace jepsen

#endif