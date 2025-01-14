#pragma once
class Road
{
private:
	int numPeople;
	int numBlocks;
public:
	Road();
	int getNumPeople();
	int getNumBlocks();
	void setNumPeople(int numPeople);
	void setNumBlocks(int numBlocks);
	void addPeople(int numPeople);
	void addBlocks(int numBlocks);
};

