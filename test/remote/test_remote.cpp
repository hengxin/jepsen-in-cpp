#include "SSHRemote.h"
#include <boost/algorithm/string.hpp>
// public-cd-b{0-2}.disalg.cn
const string b0 = "47.108.193.81";
const string b1 = "47.108.227.187";
const string b2 = "47.108.208.93";

int main() {
    log4cplus::Initializer initializer;
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_TEXT("/home/young/github-projects/jepsen-in-cpp/src/log4cplus.cfg"));

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));

    vector<string> nodes{b0, b1, b2};
    ConnSpec conn_spec(b1, 22, "young", "guazi13110", "~/.ssh/id_rsa", false);
    SSHRemote remote(10, conn_spec);

    string out;
    string err;
    remote.sudoExecute("mkdir -p /tmp/jepsen/cpp", out, err);

    vector<string> result;
    boost::split(result, out, boost::is_any_of("\n"));
    LOG4CPLUS_INFO(logger, result[0].c_str());
    LOG4CPLUS_INFO(logger, result.size());

    return 0;
}