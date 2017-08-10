#include "ConfigManager.h"

ConfigManager::ConfigManager() {}

ConfigManager::ConfigManager(string filePath) {
	ptree pt;
	read_xml(filePath, pt);

	//loop for every node under <config>
	BOOST_FOREACH(ptree::value_type &v1, pt.get_child("config")) {
		if (v1.first == "groups") {
			
			BOOST_FOREACH(ptree::value_type &v2, v1.second) {
				if (v2.first == "group") {  //for each group
					GroupStruct* group = new GroupStruct();
					cout << "Group is created" << endl;
					BOOST_FOREACH(ptree::value_type &v3, v2.second) {
						if (v3.first == "<xmlattr>") {  //it's an attribute
							(*group).setName(v3.second.get<string>("name"));
							cout << "    Group name is " << (*group).getName() << endl;
				
						}
						else if (v3.first == "family") {
							FamilyStruct* family = new FamilyStruct();
							cout << "Family is created" << endl;

							BOOST_FOREACH(ptree::value_type &v4, v3.second) {
								if (v4.first == "<xmlattr>") {  //it's an attribut
									(*family).setName(v4.second.get<string>("name"));
									cout << "        Family name is " << (*family).getName() << endl;
								}
								else
								{
									BOOST_FOREACH(ptree::value_type &v5, v4.second) {
										(*family).addName(v5.second.data());
										cout << "      family add name:" << v5.second.data() << endl;
									}
								}
							}
							// Add new family to tmp group
							group->addFamily((*family));
						}


					}
					// Add whole group to groupList
					groupList.push_back((*group));
				}
				else {
					cout << "Error! Check your config.xml format!!" << endl;
				}
				cout << endl;
				
			}
		}

	}
}

ConfigManager::~ConfigManager() {}


vector<GroupStruct> ConfigManager::getGroupList() {
	return groupList;
}

/*
// Test Demo
int main(void) {
	ConfigManager("resource/config.xml");
	system("pause");
}
*/

