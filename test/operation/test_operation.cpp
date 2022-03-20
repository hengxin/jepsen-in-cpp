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
        if (i % 2 == 0) {
            op["type"] = "r";
        } else {
            op["type"] = "w";
            info["value"] = i;
        }
        op["key"] = "x";
        value.append(op);
    }
    info["value"] = value;

    Operation op("txn", info, Operation::kInvoke);
    cout << op.toString() << endl;

    Operation::OPInfo agg;
    agg.append(":w");
    agg.append("x");
    agg.append(1);

    cout << Operation::toString(agg) << endl;

    return 0;
}