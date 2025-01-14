#include "Road.h"

Road::Road():numPeople(0),numBlocks(0)
{
}




int Road::getNumPeople()
{
	return numPeople;
}
int Road::getNumBlocks()
{
	return numBlocks;
}
void Road::setNumPeople(int numPeople)
{
	this->numPeople = numPeople;
}
void Road::setNumBlocks(int numBlocks)
{
	this->numBlocks = numBlocks;
}
void Road::addPeople(int numPeople)
{
	this->numPeople += numPeople;
}
void Road::addBlocks(int numBlocks)
{
	this->numBlocks += numBlocks;
}

