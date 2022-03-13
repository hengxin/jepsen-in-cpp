#include <iostream>
#include <json/json.h>
#include <string>

int main() {
    Json::Value root;  // 'root' will contain the root value after parsing.
    root["type"] = "write";
    root["key"] = 1;
    root["value"] = 20;

    std::cout << root << std::endl;
    int keyint = root["key"].asInt();
    std::string keystr = root["key"].asString();

    std::cout << keyint << std::endl;
    std::cout << keystr << std::endl;


    return 0;
}