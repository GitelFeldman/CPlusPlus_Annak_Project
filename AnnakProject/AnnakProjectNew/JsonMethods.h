#pragma once
#ifndef JSONMETHODS_H
#define JSONMETHODS_H

#include "json.hpp"
#include<string>
#include<iostream>
#include <fstream>

using namespace std;
using json = nlohmann::json;

class JsonMethods
{
private:
    string fileName;
public:
    json openConfiguration();
};

#endif // JSONMETHODS_H
