#include "Nemesis.h"

NemesisCreator Nemesis::cur_nemesis_type = NemesisConstructorHelper<NoopNemesis>();

bool Nemesis::open(string node) {
    return true;
}

bool Nemesis::close() {
    return true;
}

bool Nemesis::setup() {
    return true;
}

bool Nemesis::invoke(Operation op) {
    return true;
}

bool Nemesis::teardown() {
    return true;
}