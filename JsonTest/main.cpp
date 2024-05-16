#include <iostream>
#include <string>

#include "json.hpp"
using namespace std;
using nlohmann::json;

int main() {
    json js;
    js["chinese"] = "中文";

    string str = js.dump();

    json jsres = json::parse(str);

    cout << jsres["chinese"] << endl;
}