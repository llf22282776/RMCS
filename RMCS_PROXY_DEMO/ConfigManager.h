#pragma once
#ifndef ConfigManager_H
#define ConfigManager_H

#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "LookUpManager.h"

using namespace std;
using namespace boost::property_tree;

class ConfigManager {
private:
	vector<GroupStruct> groupList;
public:
	ConfigManager();
	ConfigManager(string filePath);
	~ConfigManager();
	vector<GroupStruct> getGroupList();
};

#endif // !ConfigManager_H
