#pragma once
#include <memory>
using namespace std;
class City
{
private:
	int numPeople = 0;
public:
	City(int numPeople);
	int getNumPeople();
	void setNumPeople(int numPeople);
	void addPeople(int numPeople);
};

