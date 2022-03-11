#include "Client.h"

ClientCreator Client::cur_client_type = ClientConstructorHelper<NoopClient>();
