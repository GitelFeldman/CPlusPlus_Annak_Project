#include "City.h"
#include <memory>
using namespace std;
City::City(int numPeople):numPeople(numPeople)
{
}

int City::getNumPeople()
{
    return numPeople;
}

void City::setNumPeople(int numPeople)
{
    this->numPeople = numPeople;
}

void City::addPeople(int numPeople)
{
    this->numPeople += numPeople;
}
