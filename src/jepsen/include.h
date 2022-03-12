#ifndef INCLUDE_H
#define INCLUDE_H

#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/initializer.h>
#include <log4cplus/layout.h>
#include <log4cplus/logger.h>  // log4cplus
#include <log4cplus/loggingmacros.h>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include <json/json.h>  // jsoncpp

#include <boost/format.hpp>  // boost

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

#endif