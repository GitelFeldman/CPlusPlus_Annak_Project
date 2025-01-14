#include "Cell.h"
#include <iostream>
#include "json.hpp"
#include "JsonMethods.h"
using json = nlohmann::json;
using namespace std;

JsonMethods* jsonMethods2 = new JsonMethods(); // Definition of jsonMethods
json jsonData2 = jsonMethods2->openConfiguration(); // Definition of jsonData

Cell::Cell(string name, int position_x, int poition_y, int numPeople, const map<string, shared_ptr<int>>& generalResources)
    : name(name), position_x(position_x), position_y(position_y), numPeople(numPeople), generalResources(generalResources) {}

Cell::Cell() {}

map<string, shared_ptr<int>>& Cell::getGeneralResources() {
    return generalResources;
}

void Cell::setNumPeople(int numPeople) {
    this->numPeople = numPeople;
}

int Cell::getNumPeople() {
    return numPeople;
}

void Cell::addNumPeople(int numPeople) {
    this->numPeople += numPeople;
}

void Cell::printGeneralResources() {
    vector<string> resourceTypes = jsonData2["ResourceTypes"];
    cout << "SelectedResource ";
    for (const auto& resource : resourceTypes) {
        if (resource != "People")
            cout << *generalResources[resource] << " ";
    }
}
bool Cell::checkIfCityOrVillage() {
    return this->getIsCity() || this->getIsVillage();
}

void Cell::printCellCategory() {
    cout << "selectedCategory " << name << endl;
}

void Cell::setGeneralResources(const map<string, shared_ptr<int>>& generalResources) {
    this->generalResources = generalResources;
}

void Cell::zeroAllReesource()
{
    for (auto it = generalResources.begin(); it != generalResources.end(); it++) {
        *it->second = 0;
    }
}

void Cell::setGeneralResources(const string& resourceName, int resourceValue) {
    *generalResources[resourceName] = resourceValue;
}

string Cell::getName() {
    return name;
}

void Cell::setName(string name) {
    this->name = name;
}

bool Cell::getIsCity() {
    return isCity;
}

void Cell::setIsCity(bool isCity) {
    this->isCity = isCity;
}

shared_ptr<City> Cell::getCityPtr() {
    return cityPtr;
}

void Cell::setCityPtr(shared_ptr<City> cityPtr) {
    this->cityPtr = cityPtr;
}

bool Cell::getIsRoad() {
    return isRoad;
}

void Cell::setIsRoad(bool isRoad) {
    this->isRoad = isRoad;
}

shared_ptr<Road> Cell::getRoadPtr() {
    return roadPtr;
}

void Cell::setRoadPtr(shared_ptr<Road> roadPtr) {
    this->roadPtr = roadPtr;
}

bool Cell::getIsVillage() {
    return isVillage;
}

void Cell::setIsVillage(bool isVillage) {
    this->isVillage = isVillage;
}

shared_ptr<Village> Cell::getVillagePtr() {
    return villagePtr;
}

void Cell::setVillagePtr(shared_ptr<Village> villagePtr) {
    this->villagePtr = villagePtr;
}

shared_ptr<Transportation> Cell::getTransportation() {
    if (this->transportation)

        return this->transportation;
    else
        return nullptr;

}

void Cell::setTransportation(shared_ptr<Transportation> transportation) {
    this->transportation = transportation;
}

void Cell::changeResourceToSpecificCell(int value, string resourceToChange) {
    this->setGeneralResources({
         {"Wood", make_shared<int>(0)},
         {"Wool", make_shared<int>(0)},
         {"Iron", make_shared<int>(0)},
         {"Blocks", make_shared<int>(0)}
        });
    *(this->getGeneralResources()[resourceToChange]) = value;
}

void Cell::addResourceToSpecificCell(int value, string resourceToChange) {
    *(this->getGeneralResources())[resourceToChange] += value;
}


int Cell::getPosition_x() {
    return position_x;
}
int Cell::getPosition_y() {
    return position_y;
}

void Cell::makeEmpty()
{
    this->setNumPeople(0);
    this->setGeneralResources({
         {"Wood", make_shared<int>(0)},
         {"Wool", make_shared<int>(0)},
         {"Iron", make_shared<int>(0)},
         {"Blocks", make_shared<int>(0)}
        });
    if (this->getIsCity())
        this->getCityPtr()->setNumPeople(0);
    if (this->getIsVillage())
        this->getVillagePtr()->setNumPeople(0);
    if (this->getIsRoad())
    {
        this->getRoadPtr()->setNumPeople(0);
        this->getRoadPtr()->setNumBlocks(0);
    }
}

bool Cell::checkIfCanManufactureTransportation(string transportationType) {
    if (this->getIsCity() || this->getIsVillage()) {
        vector<int> resourcesCosts = jsonData2["Costs"][transportationType];
        map<string, shared_ptr<int>> currentCellResources = this->getGeneralResources();
        for (int i = 0; i < resourcesCosts.size(); i++) {
            string resourceName = jsonData2["ResourceTypes"][i];
            if (*currentCellResources[resourceName] < resourcesCosts[i])
                return false;
        }
    }
    return true;
}