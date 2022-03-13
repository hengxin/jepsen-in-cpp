#include "Database.h"

namespace jepsen {
void DB::initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes) {
    this->remotes = remotes;  // TODO: Deep copy?
}
}  // namespace jepsen
