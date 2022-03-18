#ifndef OPERATION_H
#define OPERATION_H

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
    enum Status { kInit, kInvoke, kSuccess, kFailed, kInfo };

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

private:
    string type;
    OPInfo op;
    Status status;
};
}  // namespace jepsen


#endif