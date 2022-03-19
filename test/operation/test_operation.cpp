#include "Operation.h"

#include <iostream>
using std::cout;
using std::endl;
using namespace jepsen;

int main() {
    Operation::OPInfo info;
    Operation::OPInfo value;

    for (int i = 0; i < 5; i++) {
        Operation::OPInfo op;
        op["key"] = "x";
        if (i % 2 == 0) {
            op["type"] = "r";
        } else {
            op["type"] = "w";
            info["value"] = i;
        }
        value.append(op);
    }
    info["value"] = value;

    Operation op("txn", info, Operation::kInvoke);

    cout << "type: " << op.getType() << endl;
    cout << "op: " << op.getOp() << endl;
    cout << "status: " << op.getStatus() << endl;

    cout << op.toString() << endl;

    return 0;
}