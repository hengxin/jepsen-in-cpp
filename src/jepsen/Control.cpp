#include "Control.h"

log4cplus::Logger ControlHelper::logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"));

bool ControlHelper::install_archive(RemotePtr& remote, string url, string dir, bool force = false) {
    string encoded_url = Base64Coder::base64_encode(url);
    string dest_file = kWgetCacheDir + "/" + encoded_url;

    LOG4CPLUS_INFO(logger, dest_file.c_str());

    Context ctx(dir, "young", "guazi13110");
    int res = remote->execute(ctx, "stat ~/nodes", "");
    LOG4CPLUS_INFO(logger, res);
    return true;
}
