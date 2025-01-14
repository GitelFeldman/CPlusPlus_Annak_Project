#ifndef TILE_H
#define TILE_H

#include <iostream>
#include <string>
#include <map>
#include <memory>

#include"Transportation.h"
#include"City.h"
#include"Road.h"
#include "Village.h"


class Cell {
public:
    Cell(std::string name, int position_x, int poition_y, int numPeople, const std::map<std::string, std::shared_ptr<int>>& generalResources);
    Cell();

    std::map<std::string, std::shared_ptr<int>>& getGeneralResources();
    void setNumPeople(int numPeople);
    int getNumPeople();
    void addNumPeople(int numPeople);
    void printGeneralResources();
    void printCellCategory();
    void setGeneralResources(const std::map<std::string, std::shared_ptr<int>>& generalResources);
    void zeroAllReesource();
    void setGeneralResources(const std::string& resourceName, int resourceValue);
    std::string getName();
    void setName(std::string name);
    bool getIsCity();
    void setIsCity(bool isCity);
    std::shared_ptr<City> getCityPtr();
    void setCityPtr(std::shared_ptr<City> cityPtr);
    bool getIsRoad();
    void setIsRoad(bool isRoad);
    std::shared_ptr<Road> getRoadPtr();
    void setRoadPtr(std::shared_ptr<Road> roadPtr);
    bool getIsVillage();
    void setIsVillage(bool isVillage);
    std::shared_ptr<Village> getVillagePtr();
    void setVillagePtr(std::shared_ptr<Village> villagePtr);
    shared_ptr<Transportation> getTransportation();
    void setTransportation(shared_ptr<Transportation> transportation);
    void changeResourceToSpecificCell(int value, std::string resourceToChange);
    void addResourceToSpecificCell(int value, std::string resourceToChange);
    void makeEmpty();
    bool checkIfCityOrVillage();
    //static json jsonData; // Declaration of jsonData
    //static JsonMethods* jsonMethods; // Declaration of jsonMethods
    int getPosition_x();
    int getPosition_y();
    bool checkIfCanManufactureTransportation(string transportationType);
    shared_ptr<Transportation> transportation;

private:
    std::string name;
    int numPeople;
    int position_x;
    int position_y;
    bool isCity;
    bool isRoad;
    bool isVillage;
    std::map<std::string, std::shared_ptr<int>> generalResources;
    std::shared_ptr<City> cityPtr;
    std::shared_ptr<Road> roadPtr;
    std::shared_ptr<Village> villagePtr;

};

#endif // TILE_H
