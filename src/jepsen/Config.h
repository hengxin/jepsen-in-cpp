
#include <json/json.h>  // jsoncpp

namespace jepsen {
class Config {
public:
    using config = Json::Value;
};
}  // namespace jepsen