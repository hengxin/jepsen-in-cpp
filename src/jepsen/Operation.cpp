#include "Operation.h"

namespace jepsen {
Operation& Operation::operator=(const Operation& rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->type = rhs.type;
    this->op = rhs.op;
    this->status = rhs.status;
    this->process = rhs.process;

    return *this;
}

string& Operation::getType() {
    return this->type;
}

Operation::OPInfo& Operation::getOp() {
    return this->op;
}

Operation::OPInfo& Operation::getOp(string key) {
    return this->op[key];
}


Operation::Status Operation::getStatus() {
    return this->status;
}
}  // namespace jepsen