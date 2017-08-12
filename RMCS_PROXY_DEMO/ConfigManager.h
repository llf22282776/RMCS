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
#include <map>

using namespace std;
using namespace boost::property_tree;

class ConfigManager {
private:
	vector<GroupStruct> groupList;
	vector<RedisCofig> redisList;
	vector<DBconfig> dbList;
public:
	ConfigManager();
	ConfigManager(string filePath);
	
	~ConfigManager();
	vector<GroupStruct> getGroupList();
	vector<RedisCofig> getRedisList();
	vector<DBconfig> getDbConfig();
};
class RedisCofig{
	public:
		string ip;
		int port;
};
class DBconfig{
	public:
		string user;
		string pwd;
		string ip;
		int port;
		string dbSchme;
		map<string,string> parmMap;
		DBconfig():parmMap(){}
};
#endif // !ConfigManager_H
