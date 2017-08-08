#ifndef	LOOKUPMANAGER_H
#define LOOKUPMANAGER_H
#include "CThread.h"
#include <map>
#include <vector>
#include <string>
#include "src/lookup.hpp"
using namespace std;
#include "CacheManager.h"

class LookUpManager:public CThread{
	//这里是lLookUpManager管理类
	//它是个线程类
	//需要周期性的更新family和names
	//更新缓存的family and name map
	//并且周期性的从缓存中读取用户定义的group
	//更新group的连接状态
	//需要缓冲管理类
	//需要定义缓存里面的gruopStateMap
	/*
	还有group
	key:{
		family:
		name:[]
	}
	value:{
		isConnected:
		list:[
			
		]



	}
	
	
	*/
	
	typedef struct GroupKeyInfo
	{
		//groupMap和groupStateMap的key
		string* family;

	};
private:
	//------------------------
	map<string,vector<string>>& familyAndNames_last; //上一次的map指针，初始化为null
	hebi::Lookup& lookup;
	//----------------------
	void updateFamilyAndNamesMap(map<string,vector<string>&>& newFandNMap);//更新缓存中的map
	map<string,vector<string>&>& getNewestMapFromHibi();//获得最新的family map的引用
	bool isFirstRun;//是否第一次运行，是的话为true，否则为false,初始化为false,第一次run的时候不去读，等sleep时间,然后getEntryList
	
	//-----------------------
public:
	LookUpManager(CacheManager& cacheManager);//需要什么参数,缓存管理类，简单，不需要信息沟通,没必要生产者消费者模型,
	~LookUpManager();
	void run() override; // override run function,to find 
	void init() ;//init the lookupManager
 


};
#endif