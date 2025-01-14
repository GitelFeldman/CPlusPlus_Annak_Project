#include "Transportation.h"

#include "json.hpp"
#include"JsonMethods.h"

using namespace std;
using json = nlohmann::json;

JsonMethods* jsonMethods1 = new JsonMethods();
json jsonData1 = jsonMethods1->openConfiguration();
Transportation::Transportation(string name) : name(name) {
    try {
        // ודא שהמפתח קיים במבנה JSON
        if (jsonData1["Capacities"].contains(name)) {
            capacities = jsonData1["Capacities"][name].get<vector<int>>();
        }
        else {
            throw runtime_error("Key not found in JSON: " + name);
        }
        currentResources = { 0, 0, 0, 0 };
    }
    catch (json::exception& e) {
        cout << "Error accessing JSON data: " << e.what() << endl;
    }
}

Transportation::Transportation() {}

string Transportation::getName() {
    return name;
}

void Transportation::setName(string name) {
    this->name = name;
}

vector<int> Transportation::getCapacities() {
    return capacities;
}

vector<int> Transportation::getCurrentResources() {
    return currentResources;
}

void Transportation::setACurrentResource(int resource, int countToSet) {
    if (countToSet <= capacities[resource]) {
        currentResources[resource] = countToSet;
    }
}

void Transportation::setAllResources(vector<int> currentCapacities) {
    for (int i = 0; i < currentCapacities.size(); i++) {
        this->currentResources[i] = min(currentCapacities[i], capacities[i]);
    }
}
