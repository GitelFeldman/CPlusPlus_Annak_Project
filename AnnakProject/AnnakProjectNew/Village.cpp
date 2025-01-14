#include "Village.h"


Village::Village(int numPeople) :numPeople(numPeople)
{
}

int Village::getNumPeople()
{
    return numPeople;
}

void Village::setNumPeople(int numPeople)
{
    this->numPeople = numPeople;
}

void Village::addPeople(int numPeople)
{
    this->numPeople += numPeople;
}
