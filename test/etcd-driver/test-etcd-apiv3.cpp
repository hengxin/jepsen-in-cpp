#include <etcd/Client.hpp>
#include <string>

int main() {
    std::string url = "http://47.108.193.81:2379";
    etcd::Client etcd(url);
    etcd::Response response = etcd.get("key1").get();
    std::cout << "key1 : " << response.value().as_string() << std::endl;
    return 0;
}