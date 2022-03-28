#include "libssh2_config.h"
#include <iostream>

int main() {
#ifdef HAVE_WINSOCK2_H
    std::cout << "Windows Platform" << std::endl;
#else
    std::cout << "Unit Platform" << std::endl;
#endif
    return 0;
}