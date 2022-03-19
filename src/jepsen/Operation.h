#ifndef OPERATION_H
#define OPERATION_H

#include <boost/format.hpp>  // boost
#include <concurrentqueue/blockingconcurrentqueue.h>
#include <json/json.h>  // jsoncpp
#include <memory>
#include <string>
using std::string;

namespace jepsen {
class Operation;
using OperationQueue = moodycamel::BlockingConcurrentQueue<Operation>;
using OperationQueuePtr = std::shared_ptr<OperationQueue>;

class Operation {
public:
    using OPInfo = Json::Value;
    enum Status { kInit, kInvoke, kSuccess, kFailed, kInfo, kExit };
    const string StatusStr[6] = {":init", ":invoke", ":ok", ":fail", ":info", ":exit"};

    // Constructors
    Operation() : type(""), status(kInit){};
    explicit Operation(string type) : type(type) {}
    Operation(string type, OPInfo op, Status status) : type(type), op(op), status(status) {}
    Operation(const Operation& operation) {
        Operation(operation.type, operation.op, operation.status);
    };

    // Destructors
    ~Operation(){

    };

    // Operators
    Operation& operator=(const Operation& rhs);

    string& getType();
    OPInfo& getOp();
    OPInfo& getOp(string key);
    Status getStatus();

    inline void setStatus(Status status) {
        this->status = status;
    }
    string toString(OPInfo& jop);
    string toString();

private:
    int process;
    string type;
    OPInfo op;
    Status status;
};

class OperationFactory {
public:
    static Operation write(string key, string value) {
        Operation::OPInfo op;
        op["key"] = key;
        op["value"] = value;
        return Operation("w", op, Operation::kInvoke);
    }

    static Operation read(string key) {
        Operation::OPInfo op;
        op["key"] = "x";
        return Operation("r", op, Operation::kInvoke);
    }


    static Operation cas(string key, string value, string old) {
        Operation::OPInfo op;
        op["key"] = key;
        op["value"] = value;
        op["old-value"] = old;
        return Operation("cas", op, Operation::kInvoke);
    }

    static Operation exit() {
        Operation::OPInfo op;
        return Operation("exit", op, Operation::kExit);
    }
};

}  // namespace jepsen


#endif