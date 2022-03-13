#include "Client.h"

namespace jepsen {
ClientCreator Client::cur_client_type = ClientConstructorHelper<NoopClient>();
}
