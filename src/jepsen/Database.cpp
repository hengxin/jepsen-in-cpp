#include "Database.h"

namespace jepsen {
void DB::initRemotes(std::unordered_map<string, SSHRemotePtr>& remotes) {
    this->remotes = remotes;  // TODO: Deep copy?
}
}  // namespace jepsen
