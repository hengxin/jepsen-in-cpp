#include "Generator.h"

namespace jepsen {
void Generator::fillIn(const JepsenContext& context, Operation& op) {
    int process = context.randomFreeProcess();
    if (process != kInvalidProcess) {
        if (op.process == kInvalidProcess) {
            op.process = process;
        }
        if (op.time == 0) {
            op.time = context.time;
        }
        if (op.type == Operation::kInit) {
            op.type = Operation::kInvoke;
        }
    } else {
        op.type = Operation::kPending;
    }
}
}  // namespace jepsen