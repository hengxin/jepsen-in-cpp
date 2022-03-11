#include "Database.h"

void DB::initRemotes(unordered_map<string, shared_ptr<SSHRemote>>& remotes) {
    this->remotes = remotes;  // TODO: Deep copy?
}
