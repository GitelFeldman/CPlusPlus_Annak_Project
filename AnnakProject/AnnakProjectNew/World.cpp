#include "World.h"
#include <stdexcept>
#include <memory>
#include <string>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

JsonMethods* jsonMethods = new JsonMethods();
json jsonData = jsonMethods->openConfiguration();
World::World(Input in) :input(in)
{

	input.parse_and_store();
	Command wrldCmd = *(input.world);
	vector<shared_ptr<Command>> resourceCmd = input.start;

	vector<vector<string>> worldStr = wrldCmd.data;
	vector<vector<int>> worldInt;
	for (const auto& row : worldStr) {
		vector<int> intRow;
		for (const auto& str : row) {
			intRow.push_back(stoi(str));
		}
		worldInt.push_back(intRow);
	}


	int width = worldInt.size() * 5;
	int height = worldInt[0].size() * 5;

	worldBoard = vector<vector<Cell>>(width, vector<Cell>(height));

	for (int i = 0; i < worldInt.size(); ++i) {
		for (int j = 0; j < worldInt[0].size(); ++j) {
			string name = jsonData["TilesTypes"][worldInt[i][j] - 1];

			// Initialize general resources map
			map<string, shared_ptr<int>> generalResources = {
				{"Wood", make_shared<int>(0)},
				{"Wool", make_shared<int>(0)},
				{"Iron", make_shared<int>(0)},
				{"Blocks", make_shared<int>(0)}
			};

			// Initialize resources based on tile type
			if (name == "Forest") {
				*generalResources["Wood"] = jsonData["StartingResources"][name].get<int>();
			}
			else if (name == "Field") {
				*generalResources["Wool"] = jsonData["StartingResources"][name].get<int>();
			}
			else if (name == "IronMine") {
				*generalResources["Iron"] = jsonData["StartingResources"][name].get<int>();
			}
			else if (name == "BlocksMine") {
				*generalResources["Blocks"] = jsonData["StartingResources"][name].get<int>();
			}
			else {
				// Default case
				*generalResources["Wood"] = 0;
				*generalResources["Wool"] = 0;
				*generalResources["Iron"] = 0;
				*generalResources["Blocks"] = 0;
			}

			// Create 5x5 tiles for the current world tile
			for (int k = 0; k < 5; ++k) {
				for (int l = 0; l < 5; ++l) {
					Cell t(name, j * 5 + l, i * 5 + k, 0, generalResources);
					worldBoard[i * 5 + k][j * 5 + l] = t;
				}
			}
		}
	}

	startCommands();
}

// Other member functions are unchanged for brevity

void World::startCommands()
{

	vector<shared_ptr<Command>> startCmd = input.start;
	for (const auto& cmd : startCmd)
	{

		if (cmd->name == "Resource") {

			int position_x = stoi(cmd->arguments[2]) - 1;
			int position_y = stoi(cmd->arguments[3]) - 1;
			string resourceName = cmd->arguments[1];
			string tileName = worldBoard[position_y][position_x].getName();
			int valueToAddToResource = stoi(cmd->arguments[0]);

			if (worldBoard[position_y][position_x].getTransportation() != nullptr)
			{

				int resourceNumber = getResourceNumber(resourceName);

				int currentResource = worldBoard[position_y][position_x].getTransportation()->getCurrentResources()[resourceNumber];

				worldBoard[position_y][position_x].getTransportation()->setACurrentResource(resourceNumber, currentResource + valueToAddToResource);


			}
			else if (jsonData["Capacities"].contains(tileName))
			{

				int capacity = jsonData["Capacities"][tileName][getResourceNumber(resourceName)];
				int currentValueInResource = *worldBoard[position_y][position_x].getGeneralResources()[resourceName].get();
				worldBoard[position_y][position_x].changeResourceToSpecificCell(min(
					capacity, currentValueInResource + valueToAddToResource), resourceName);


			}
			else
			{

				worldBoard[position_y][position_x].changeResourceToSpecificCell(valueToAddToResource, resourceName);


			}

		}
		else if (cmd->name == "People")
		{
			createPeople(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "City")
		{
			buildCity(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "Road")
		{
			buildRoad(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "Village")
		{
			buildVillage(*cmd);
		}
		else if (cmd->name == "MakeEmpty")
		{
			int position_x = stoi(cmd->arguments[0]) - 1;
			int position_y = stoi(cmd->arguments[1]) - 1;
			worldBoard[position_y][position_x].makeEmpty();
		}
		else if (cmd->name == Command::MANUFACTURE)
		{
			createTransportation(*cmd);
		}
		else if (cmd->name == "Manufacture")
		{
			createTransportation(*cmd);
		}
	}
	inputCommands();
}




void World::inputCommands()
{

	vector<shared_ptr<Command>> inputCmd = input.steps;
	for (const auto& cmd : inputCmd)
	{
		if (cmd->name == "Select")
		{

			int position_x = stoi(cmd->arguments[0]) - 1;
			int position_y = stoi(cmd->arguments[1]) - 1;
			selected = worldBoard[position_y][position_x];
		}
		else if (cmd->name == "Wait")
		{
			// ��� �-Wait
		}
		else if (cmd->name == "Work") {
			int x = stoi(cmd->arguments[0]);
			int y = stoi(cmd->arguments[1]);
			cout << x << " " << y << endl;
			worldBoard[y - 1][x - 1].zeroAllReesource();
		}
		else if (cmd->name == "Rain")
		{
			makeRain(*cmd);
		}
		else if (cmd->name == "People")
		{
			createPeople(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "City")
		{
			int position_x_start_city = stoi(cmd->arguments[1]) - 1;
			int position_y_start_city = stoi(cmd->arguments[2]) - 1;
			bool isRoad = ifRoadForCityOrVillage(position_x_start_city, position_y_start_city, "City");
			if (isRoad)
				buildCity(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "Road")
		{
			buildRoad(*cmd);
		}
		else if (cmd->name == "Build" && cmd->arguments[0] == "Village")
		{
			int position_x_start_village = stoi(cmd->arguments[1]) - 1;
			int position_y_start_village = stoi(cmd->arguments[2]) - 1;
			bool isRoad = ifRoadForCityOrVillage(position_x_start_village, position_y_start_village, "Village");
			if (isRoad)
				buildVillage(*cmd);
		}
		else if (cmd->name == "Manufacture")
		{
			string transportationType = cmd->arguments[0];
			int position_x = stoi(cmd->arguments[1]) - 1;
			int position_y = stoi(cmd->arguments[2]) - 1;
			if (worldBoard[position_y][position_x].checkIfCanManufactureTransportation(transportationType))
				createTransportation(*cmd);
		}
		else if (cmd->name == "Deposit")
		{
			makeDeposit(*cmd);
		}
		else if (cmd->name == "TakeResources")
		{
			int position_x = stoi(cmd->arguments[0]) - 1;
			int position_y = stoi(cmd->arguments[1]) - 1;
			if (selected.getTransportation() != nullptr)
			{
				vector<int>currentResources = selected.getTransportation()->getCurrentResources();
				vector<int> selectedNewdResources(currentResources.size());//מערך לשמירה זמנית של המשאבים החדשים שיהיו בכל תחבורה
				map<string, shared_ptr<int>> otherResources = worldBoard[position_y][position_x].getGeneralResources();
				//חישוב הוספת המשאבים לכלי התחבורה
				for (int i = 0; i < currentResources.size(); i++) {
					int current = currentResources[i];
					selectedNewdResources[i] = currentResources[i] + *otherResources[jsonData["ResourceTypes"][i]];
				}
				selected.getTransportation()->setAllResources(selectedNewdResources);
				//מערך לשמירה כמה צריך להוריד מהמשאבים בתא שלקחתי ממנו
				vector<int> selectedTransportationResourcesAfterChanging = selected.getTransportation()->getCurrentResources();
				vector<int> resourcesToRemove(selectedTransportationResourcesAfterChanging.size());
				for (int i = 0; i < selectedTransportationResourcesAfterChanging.size(); i++) {
					resourcesToRemove[i] = selectedTransportationResourcesAfterChanging[i] - selectedNewdResources[i];
				}
				//לולאה לעדכון המשאבים בסל שלקחנו ממנו משאבים
				for (int i = 0; i < otherResources.size(); i++) {
					if (currentResources[i] != selectedTransportationResourcesAfterChanging[i])
					{
						int diffrence = selectedTransportationResourcesAfterChanging[i] - currentResources[i];
						int currentResourceToChange = *worldBoard[position_y][position_x].getGeneralResources()[jsonData["ResourceTypes"][i]];
						worldBoard[position_y][position_x].setGeneralResources(jsonData["ResourceTypes"][i], currentResourceToChange - diffrence);
					}
				}

			}


		}
	}
	// cout << "input";
	assertCommands();
}

void World::assertCommands()
{
	vector<string> assertCmd = input.asserts;
	for (const string& assert : assertCmd)
	{
		if (assert == "SelectedResource")
			selected.printGeneralResources();
		else if (assert == "SelectedCategory")
			selected.printCellCategory();
		else if (assert == "SelectedComplete")
			cout << "SelectedComplete " << "False";
		else if (assert == "CityCount")
			cout << "CityCount " << cityCount << endl;
		else if (assert == "RoadCount")
			cout << "RoadCount " << roadCount << endl;
		else if (assert == "VillageCount")
			cout << "VillageCount " << villageCount << endl;
		else if (assert == "SelectedPeople")
		{
			cout << "SelectedPeople ";
			if (selected.getIsCity())
				cout << selected.getCityPtr()->getNumPeople();
			else if (selected.getIsRoad())
				cout << selected.getRoadPtr()->getNumPeople();
			else if (selected.getIsVillage())
				cout << selected.getVillagePtr()->getNumPeople();
		}
		else if (assert == "SelectedCar")
		{
			if (selected.getTransportation() == nullptr)
				cout << "SelectedCar " << 0;
			else if (selected.getTransportation()->getName() == "Car")
				cout << "SelectedCar " << 1;
		}
		else if (assert == "SelectedTruck")
		{
			if (selected.getTransportation() == nullptr)
				cout << "SelectedTruck " << 0;
			else if (selected.getTransportation()->getName() == "Truck")
				cout << "SelectedTruck " << 1;
		}
		else if (assert == "SelectedHelicopter")
		{
			if (selected.getTransportation() == nullptr)
				cout << "SelectedHelicopter " << 0;
			else if (selected.getTransportation()->getName() == "Helicopter")
				cout << "SelectedHelicopter " << 1;
		}
		else if (assert == "CarCount")
			cout << "CarCount " << carCount << endl;
		else if (assert == "TruckCount")
			cout << "TruckCount " << truckCount << endl;
		else if (assert == "HelicopterCount")
			cout << "HelicopterCount " << helicopterCount << endl;
	}
}

void World::setSelected(int position_x, int position_y) {
	selected = worldBoard[position_x][position_y];
}

Cell World::getSelected() {
	return selected;
}



void World::createPeople(Command cmd)
{
	int numPeople = stoi(cmd.arguments[0]);
	int position_x = stoi(cmd.arguments[1]) - 1;
	int position_y = stoi(cmd.arguments[2]) - 1;
	if (worldBoard[position_x][position_y].getIsCity())
	{
		worldBoard[position_x][position_y].getCityPtr()->addPeople(min(numPeople, 20));
	}
	else if (worldBoard[position_x][position_y].getIsVillage())
	{
		worldBoard[position_x][position_y].getVillagePtr()->addPeople(min(numPeople, 10));
	}
	else if (worldBoard[position_x][position_y].getIsRoad())
	{
		worldBoard[position_x][position_y].getRoadPtr()->addPeople(numPeople);
	}
}



void World::createTransportation(Command cmd)
{
	int position_x = stoi(cmd.arguments[1]) - 1;
	int position_y = stoi(cmd.arguments[2]) - 1;
	int transportationSizeInt = jsonData["Sizes"][cmd.arguments[0]][0];
	shared_ptr<Transportation> transportation = make_shared<Transportation>(cmd.arguments[0]);

	for (int i = position_y; i < position_y + transportationSizeInt; i++) {
		for (int j = position_x; j < position_x + transportationSizeInt; j++) {
			worldBoard[i][j].setTransportation(transportation);
		}
	}
	if (cmd.arguments[0] == "Car")
		carCount++;
	if (cmd.arguments[0] == "Truck")
		truckCount++;
	if (cmd.arguments[0] == "Helicopter")
		helicopterCount++;
}


void World::buildVillage(Command cmd) {
	int numPeopleInt = jsonData["StartingResources"]["Village"];
	int villageSizeInt = jsonData["Sizes"]["Village"][0];
	shared_ptr<Village> v = make_shared<Village>(numPeopleInt); // יצירת מופע של כפר על ה-heap
	int position_x_start_village = stoi(cmd.arguments[1]) - 1;
	int position_y_start_village = stoi(cmd.arguments[2]) - 1;
	for (int i = position_y_start_village; i < position_y_start_village + villageSizeInt; i++) {
		for (int j = position_x_start_village; j < position_x_start_village + villageSizeInt; j++) {
			worldBoard[i][j].setNumPeople(0);
			worldBoard[i][j].setIsVillage(true);
			worldBoard[i][j].setVillagePtr(v); // שימוש ב-shared_ptr כדי להבטיח שכל האריחים מצביעים לאותו כפר
			worldBoard[i][j].setName("Village");
		}
	}
	villageCount++;
}


void World::buildCity(Command cmd) {
	int numPeopleInt = jsonData["StartingResources"]["City"];
	int citySizeInt = jsonData["Sizes"]["City"][0];
	shared_ptr<City> c = make_shared<City>(numPeopleInt); // יצירת מופע של עיר על ה-heap
	int position_x_start_city = stoi(cmd.arguments[1]) - 1;
	int position_y_start_city = stoi(cmd.arguments[2]) - 1;
	for (int i = position_y_start_city; i < position_y_start_city + citySizeInt; i++) {
		for (int j = position_x_start_city; j < position_x_start_city + citySizeInt; j++) {
			worldBoard[i][j].setNumPeople(0);
			worldBoard[i][j].setIsCity(true);
			worldBoard[i][j].setCityPtr(c); // שימוש ב-shared_ptr כדי להבטיח שכל האריחים מצביעים לאותה עיר
			worldBoard[i][j].setName("City");
		}
	}
	cityCount++;
}



void World::buildRoad(Command cmd) {
	int roadSizeInt = jsonData["Sizes"]["Road"][0];
	int position_x_start_road = stoi(cmd.arguments[1]) - 1;
	int position_y_start_road = stoi(cmd.arguments[2]) - 1;
	shared_ptr<Road> r = make_shared<Road>(); // יצירת מופע של כביש על ה-heap
	for (int i = position_y_start_road; i < position_y_start_road + roadSizeInt; i++) {
		for (int j = position_x_start_road; j < position_x_start_road + roadSizeInt; j++) {
			worldBoard[i][j].setNumPeople(0);
			worldBoard[i][j].setIsRoad(true);
			worldBoard[i][j].setRoadPtr(r); // שימוש ב-shared_ptr כדי להבטיח שכל האריחים מצביעים לאותו כביש
			worldBoard[i][j].setName("Road");
		}
	}
	roadCount++;
}


void World::makeRain(Command cmd)
{
	for (int i = 0; i < worldBoard.size(); i = i + 5)
	{
		for (int j = 0; j < worldBoard[i].size(); j = j + 5)
		{
			if (worldBoard[i][j].getName() == "Forest" && stoi(cmd.arguments[0]) >= 2000)
				worldBoard[i][j].addResourceToSpecificCell(1, "Wood");
			if (worldBoard[i][j].getName() == "Field" && stoi(cmd.arguments[0]) >= 1000)
				worldBoard[i][j].addResourceToSpecificCell(1, "Wool");
		}
	}
}


bool World::ifRoadForCityOrVillage(int position_x_start_city, int position_y_start_city, string city_or_village)
{
	//check if there is a road  to build a city

	bool isRoad = false;
	int villageSizeInt = jsonData["Sizes"]["Village"][0];
	int citySizeInt = jsonData["Sizes"]["City"][0];
	int size = (city_or_village == "City") ? citySizeInt : villageSizeInt;
	int roadSize = jsonData["Sizes"]["Road"][0];
	//check North of the city
	if (position_y_start_city > 0)
	{
		for (int i = position_x_start_city; i < position_x_start_city + size - roadSize; i++)
		{
			if (worldBoard[position_y_start_city - 1][i].getIsRoad() && worldBoard[position_y_start_city - 1][i + roadSize - 1].getIsRoad())
				return true;
		}
	}
	//check south of the city
	if (position_y_start_city + size < worldBoard.size())
	{
		for (int i = position_x_start_city; i < position_x_start_city + size - roadSize; i++)
		{
			if (worldBoard[position_y_start_city + size][i].getIsRoad() && worldBoard[position_y_start_city + size][i + roadSize - 1].getIsRoad())
				return true;
		}
	}

	//check east of the city
	if (position_x_start_city + size < worldBoard[position_y_start_city].size())
	{
		for (int i = position_y_start_city; i < position_y_start_city + size - roadSize; i++)
		{
			if (worldBoard[i][position_x_start_city + size].getIsRoad() && worldBoard[i + roadSize - 1][position_x_start_city + size].getIsRoad())
				return true;
		}
	}


	//check west of the city
	if (position_x_start_city > 0)
	{
		for (int i = position_y_start_city; i < position_y_start_city + size - roadSize; i++)
		{
			if (worldBoard[i][position_x_start_city - 1].getIsRoad() && worldBoard[i + roadSize - 1][position_x_start_city - 1].getIsRoad())
				return true;
		}
	}
	return isRoad;
}

void World::makeDeposit(Command cmd)
{
	int position_x = stoi(cmd.arguments[0]) - 1;
	int position_y = stoi(cmd.arguments[1]) - 1;
	string tileName = worldBoard[position_y][position_x].getName();
	int valueToAddToResource;
	int resourceValue, capacity;
	string resourceName;
	for (const auto& resource : worldBoard[position_y][position_x].getGeneralResources())
	{
		resourceValue = *resource.second;
		resourceName = resource.first;
		capacity = jsonData["Capacities"][tileName][getResourceNumber(resourceName)];
		valueToAddToResource = *selected.getGeneralResources()[resourceName];
		*resource.second = min(resourceValue + valueToAddToResource, capacity);
	}
	for (const auto& resource : selected.getGeneralResources())
	{
		*resource.second = 0;
	}
}


int World::getResourceNumber(string resourceName)
{
	// ודא שהמערך קיים
	if (!jsonData.contains("ResourceTypes") || !jsonData["ResourceTypes"].is_array()) {
		return -1; // במקרה שאין מערך של ResourceTypes
	}

	const auto& resourceArray = jsonData["ResourceTypes"];

	// עבור על המערך ומצא את האינדקס של המחרוזת
	for (size_t i = 0; i < resourceArray.size(); ++i) {
		if (resourceArray[i].is_string() && resourceArray[i] == resourceName) {
			return static_cast<int>(i);
		}
	}

	return -1; // במקרה שלא נמצא המשאב
}

void World::moveCommand(Cell targetCell) {
	if (selected.getTransportation()) {
		int transportationSize = jsonData["Sizes"][selected.getTransportation()->getName()];

		//loop to edit the new position for the transportation
		for (int x = targetCell.getPosition_x(); x < targetCell.getPosition_x() + transportationSize; x++) {
			for (int y = targetCell.getPosition_y(); y < targetCell.getPosition_y() + transportationSize; y++) {
				worldBoard[y][x].setTransportation(selected.getTransportation());
			}
		}
		//loop to remove the old position of the transportation
		for (int x = selected.getPosition_x() - transportationSize; x < selected.getPosition_x() + transportationSize; x++) {
			for (int y = selected.getPosition_y() - transportationSize; y < selected.getPosition_y() + transportationSize; y++) {
				if (worldBoard[y][x].getTransportation() != nullptr && worldBoard[y][x].getTransportation() == selected.getTransportation()) {
					worldBoard[y][x].setTransportation(nullptr);
				}
			}
		}
	}
}