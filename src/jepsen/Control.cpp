#include "Control.h"

namespace jepsen {

log4cplus::Logger ControlHelper::logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));

string ControlHelper::tmpDir(RemotePtr& remote) {
    string dir = kTempDir + "/" + std::to_string(rand());
    while (exists(remote, dir)) {
        dir = kTempDir + "/" + std::to_string(rand());
    }
    remote->sudoExecute("mkdir -p " + dir);
    return dir;
}

bool ControlHelper::exists(RemotePtr& remote, string filename) {
    return remote->execute("stat " + filename);
}

string ControlHelper::cachedWget(RemotePtr& remote, string url, bool force) {
    string encoded_url = Base64Coder::base64_encode(url);
    string dest_file = kWgetCacheDir + "/" + encoded_url;
    string wget_opts = "-O " + dest_file;
    if (force) {
        LOG4CPLUS_INFO(logger, "Clearing caced copy of " << url.c_str());
        remote->sudoExecute("rm -rf " + dest_file);
    }
    if (!exists(remote, dest_file)) {
        LOG4CPLUS_INFO(logger, "Downloading " << url.c_str());
        remote->sudoExecute("mkdir -p " + kWgetCacheDir);
        // TODO: the information of cd
        remote->sudoExecute("wget " + wget_opts + " " + url);
    }
    return dest_file;
}

bool ControlHelper::installArchive(RemotePtr& remote, string url, string dest, bool force) {
    string file = cachedWget(remote, url, force);
    string tmp_dir = tmpDir(remote);

    if (std::regex_match(url, std::regex(".*\\.zip$"))) {
        remote->sudoExecute("unzip " + file);  // TODO: with target
    } else {
        // TODO: insuffient
        remote->sudoExecute("tar --no-same-owner --no-same-permissions --extract --file " + file +
                            " -C " + tmp_dir);
    }

    string out;
    string err;
    remote->execute("ls -A " + tmp_dir, out, err);
    vector<string> result;
    boost::split(result, out, boost::is_any_of("\n"));

    LOG4CPLUS_ASSERT(logger, result.size() == 0);
    if (result.size() == 1 || (result.size() == 2 && result[1] == "")) {
        string tmp = tmp_dir + "/" + result[0];
        remote->sudoExecute("mv " + tmp + " " + dest);
    } else {
        remote->sudoExecute("mv " + tmp_dir + " " + dest);
    }

    remote->sudoExecute("rm -rf " + tmp_dir);
    return true;
}


void ControlHelper::startDaemon(RemotePtr& remote,
                                StartDaemonOptions& opts,
                                string bin,
                                string args) {
    LOG4CPLUS_INFO(logger, "starting " << bin.c_str());

    boost::filesystem::path pbin(bin);
    auto bin_name = pbin.stem().string();
    string echo = "echo `date +'%Y-%m-%d %H:%M:%S'`";
    echo += " Jepsen starting";
    if (opts.env.has_value()) {
        echo += " " + opts.env.value();
    }
    echo += " " + bin_name + args + " >> " + opts.logfile.value();

    remote->sudoExecute(echo);

    string command;
    if (opts.env.has_value()) {
        command += opts.env.value();
    }

    command += " start-stop-daemon --start ";
    if (!opts.background.has_value() || opts.background.value()) {
        command += " --background --no-close ";
    }
    if (!opts.make_pidfile.has_value() || opts.make_pidfile.value()) {
        command += " --make-pidfile";
    }
    if (!opts.match_executable.has_value() || opts.match_executable.value()) {
        command += " --exec " + bin + " ";
    }
    if (opts.match_process_name.has_value() && opts.match_process_name.value()) {
        if (opts.process_name.has_value()) {
            command += " --name " + opts.process_name.value();
        } else {
            command += " --name " + bin_name;
        }
    }

    command += " --pidfile " + opts.pidfile.value();
    command += " --chdir " + opts.chdir.value();
    command += " --oknodo ";
    command += " --startas " + bin;
    command += " -- " + args;
    command += " >> " + opts.logfile.value() + " 2>&1";

    remote->sudoExecute(command);
}

void ControlHelper::stopDaemon(RemotePtr& remote, string cmd, string pidfile) {
    LOG4CPLUS_INFO(logger, "starting " << cmd.c_str());
    remote->sudoExecute("killall -9 -w " + cmd);
    remote->sudoExecute("rm -rf " + pidfile);
}
}  // namespace jepsen