#include <json/json.h>
#include <iostream>

int main() {
    Json::Value root;   // 'root' will contain the root value after parsing.
    root["type"] = "write";
    root["key"] = 1;
    root["value"] = 20;

    std::cout << root << std::endl;

    return 0;
}