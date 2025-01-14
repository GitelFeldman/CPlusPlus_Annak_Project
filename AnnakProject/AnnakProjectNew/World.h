#pragma once
#include <vector>
#include "Cell.h"
#include "Input.h"
#include "json.hpp"
#include "JsonMethods.h"

using namespace std;


class World
{
private:
    vector<vector<Cell>> worldBoard;
    int cityCount = 0;
    int roadCount = 0;
    int villageCount = 0;
    int carCount = 0;
    int truckCount = 0;
    int helicopterCount = 0;
    Input input;
public:
    World(Input in);
    void startCommands();
    void inputCommands();
    void assertCommands();
    Cell selected;
    Cell getSelected();
    void setSelected(int position_x, int position_y);
    vector<vector<Cell>> getWorldBoard() { return worldBoard; }
    void createPeople(Command cmd);
    void createTransportation(Command cmd);
    void buildVillage(Command cmd);
    void buildCity(Command cmd);
    void buildRoad(Command cmd);
    void makeRain(Command cmd);
    bool ifRoadForCityOrVillage(int position_x_start_city, int position_y_start_city, string city_or_village);
    void makeDeposit(Command cmd);
    int getResourceNumber(string resourceName);
    void moveCommand(Cell targetCell);
};
