#ifndef OPERATION_H
#define OPERATION_H

#include "include/constant.h"
#include <boost/format.hpp>  // boost
#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
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
    enum Type { kInit, kInvoke, kSuccess, kFailed, kInfo, kExit, kNil, kPending };
    const string TypeStr[8] = {
        ":init", ":invoke", ":ok", ":fail", ":info", ":exit", ":nil", ":pending"};
    // Constructors
    Operation() : type(kInit){};
    explicit Operation(Type type) : type(type) {}
    Operation(string func, OPInfo op, Type type) : type(type), op(op), func(func) {}
    Operation(const Operation& rhs) : Operation(rhs.func, rhs.op, rhs.type){};
    Operation(Operation&& rhs) : Operation(rhs.func, rhs.op, rhs.type){};


    // Destructors
    ~Operation() = default;

    // Operators
    Operation& operator=(const Operation& rhs);
    Operation& operator=(Operation&& rhs);


    OPInfo& getOp(string key);

    static string toString(OPInfo& jop);
    string toString();

    Type type;    // type of this operation, like :init, :invoke, :ok, :fail, etc.
    string func;  // function of the operation, like write, read, compare-and-set, etc.
    OPInfo op;
    OPInfo helper;  // Help to store other informations
    long long time = 0;
    int process = kInvalidProcess;
};

class OperationFactory {
public:
    template <typename K, typename V>
    static Operation write(K key, V value) {
        Operation::OPInfo op;
        op.append(key);
        op.append(value);
        return Operation("write", op, Operation::kInvoke);
    }

    template <typename K>
    static Operation read(K key) {
        Operation::OPInfo op;
        op.append(key);
        op.append(Operation::OPInfo());
        return Operation("read", op, Operation::kInvoke);
    }

    template <typename K, typename V>
    static Operation cas(K key, V value, V old) {
        Operation::OPInfo op;
        op.append(key);
        op.append(value);
        op.append(old);
        return Operation("cas", op, Operation::kInvoke);
    }

    static Operation exit() {
        Operation::OPInfo op;
        return Operation("exit", op, Operation::kExit);
    }
};

}  // namespace jepsen


#endif