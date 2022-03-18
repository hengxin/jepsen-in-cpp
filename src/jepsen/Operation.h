#ifndef OPERATION_H
#define OPERATION_H

#include <json/json.h>  // jsoncpp
#include <string>
using std::string;

namespace jepsen {

class Operation {
public:
    using OPInfo = Json::Value;
    enum Status { kInvoke, kSuccess, kFailed, kInfo };

    // Constructors
    Operation() = delete;
    explicit Operation(string type) : type(type) {}
    Operation(string type, OPInfo op, Status status) : type(type), op(op), status(status) {}
    Operation(const Operation& operation) = default;

    // Destructors
    ~Operation() = default;

    // Operators
    Operation& operator=(const Operation& rhs);

    string& getType();
    OPInfo& getOp();
    OPInfo& getOp(string key);
    Status getStatus();

    inline void setStatus(Status status) {
        this->status = status;
    }

private:
    string type;
    OPInfo op;
    Status status;
};
}  // namespace jepsen


#endif