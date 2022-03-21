#include "Operation.h"

namespace jepsen {
Operation& Operation::operator=(const Operation& rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->type = rhs.type;
    this->func = rhs.func;
    this->op = rhs.op;
    this->helper = rhs.helper;
    this->process = rhs.process;
    this->time = rhs.time;

    return *this;
}

Operation& Operation::operator=(Operation&& rhs) {
    this->type = rhs.type;
    this->func = rhs.func;
    this->op = std::move(rhs.op);
    this->helper = std::move(rhs.helper);
    this->process = rhs.process;
    this->time = rhs.time;

    return *this;
}

Operation::OPInfo& Operation::getOp(string key) {
    return this->op[key];
}

string Operation::toString(OPInfo& jop) {
    string res;
    switch (jop.type()) {
        case Json::ValueType::nullValue:
            res = "nil";
            break;
        case Json::ValueType::intValue:
            res = std::to_string(jop.asInt());
            break;
        case Json::ValueType::uintValue:
            res = std::to_string(jop.asUInt());
            break;
        case Json::ValueType::realValue:
            res = std::to_string(jop.asDouble());
            break;
        case Json::ValueType::stringValue:
            res = jop.asString();
            break;
        case Json::ValueType::booleanValue:
            res = jop.asBool() ? "true" : "false";
            break;
        case Json::ValueType::objectValue: {
            const auto& keys = jop.getMemberNames();
            int n = keys.size();
            for (int i = 0; i < n; i++) {
                auto key = keys[i];
                res += key + ":" + toString(jop[key]);
                res += i == (n - 1) ? "" : ", ";
            }
            res = '{' + res + '}';
        } break;
        case Json::ValueType::arrayValue: {
            int n = jop.size();
            for (int i = 0; i < n; i++) {
                res += toString(jop[i]);
                res += i == (n - 1) ? "" : " ";
            }
            res = '[' + res + ']';
        } break;

        default:
            break;
    }
    return res;
}


string Operation::toString() {
    boost::format fmt("{:type %s, :f %s, :value %s, :process %d, :time %lld}");
    fmt % TypeStr[type] % func % toString(op) % process % time;
    return fmt.str();
}

}  // namespace jepsen