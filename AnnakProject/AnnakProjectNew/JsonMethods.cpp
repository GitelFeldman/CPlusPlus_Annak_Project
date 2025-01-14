#include "json.hpp"
#include "JsonMethods.h"
#include <vector>
#include<iostream>
#include<fstream>

using namespace std;
using json = nlohmann::json;

json JsonMethods::openConfiguration()
{
    ifstream file("configuration.json");
    if (!file.is_open()) {
        throw runtime_error("Could not open  configuration file");
    }

    json jsonData;
    file >> jsonData;
    file.close();
    return jsonData;
}
