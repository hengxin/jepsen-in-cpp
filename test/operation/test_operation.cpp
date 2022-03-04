#include "include.h"
#include "Operation.h"

using std::cout;
using std::endl;

int main() {
    Operation::OPInfo info;
    info["key"] = 1;
    info["value"] = 2;
    Operation op("write", info, Operation::kInvoke);
    
    cout << "type: " << op.getType() << endl;
    cout << "op: " << op.getOp() << endl;
    cout << "status: " << op.getStatus() << endl;

    return 0;
}