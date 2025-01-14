#include <fstream>
#include <iostream>
#include "World.h"
#include "Input.h"
#include <vector>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;


int main() {

    Input in;
    World w(in);
    return 0;
}
