#ifndef	LOOKUPMANAGER_H
#define LOOKUPMANAGER_H
#include "CThread.h"
#include <map>
#include <vector>
#include <string>
#include "src/lookup.hpp"
using namespace std;
#include "CacheManager.h"
#include <queue>
#include <list>

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

private:
	//------------------------
	hebi::Lookup& lookup; //hebi接口对象
	queue<hebi::GroupFeedback*>& groupFeedbackQueue;//收到的异步groupfeedback都应该放到这里
	//group会增加和减少,会不会影响什么？估计不会
	map<GroupStruct,unique_ptr<hebi::Group>> cacheGroupMap;//需要更新的map,从缓存里面取，如果key没有就取一下addFh,有就不管
	vector<FeedBackManager> feedbackManagerVec;
	vector<GroupStruct> fixedGroup;
	int sleep_time; //休眠时间，取默认值
	//----------------------
	


	//----------------------
	void updateFamilyAndNamesMap(map<string,vector<string>> newFandNMap);//更新缓存中的map,指针在函数内进行销毁
	map<string,vector<string>> getNewestMapFromHibi();//获得最新的family map的引用,返回处的函数中进行销毁
	bool isFirstRun;//是否第一次运行，是的话为true，否则为false,初始化为false,第一次run的时候不去读，等sleep时间,然后getEntryList

	vector<GroupStruct> getGroupListFromCache();//从缓存里获取当前用户定义的group的信息,到map去查
	void updateGroupConncetState(vector<GroupStruct> groupInCache);//刷新group的连接状态,这里面的
	bool updateOneGroupState(GroupStruct groupStrut);//这个放到cacheManager的队列里面去，能放进去就是true
	vector<GroupStruct> getGroupsStateFromHeibi(vector<GroupStruct> groupVec);//从heibi里面获得cahce里面定义的group的连接状态
	GroupStruct	getGroupStateFromHeibi(GroupStruct groupVec);
	
	

	vector<GroupStruct> getGroupListFromConfig();//从配置文件里面获取

	



	//-----------------------
public:
	LookUpManager(CacheManager& cacheManager,queue<hebi::GroupFeedback>& groupFeedbackQueue);
	//需要什么参数,缓存管理类，简单，不需要信息沟通,没必要生产者消费者模型,
	//传进来缓存管理对象，队列对象，
	//引用要在初始化列表中赋值
	//lookup在里面创建
	~LookUpManager();
	void run() override; // override run function,to find 
	void init() ;//init the lookupManager,比如跑起来
	//-------------`-`-`-`-`-`-`-----------
	static const int  DEAULT_SLEEP_TIME=100; //默认的休眠时间


};

class GroupStruct{
private:
	string family;
	string namesWithinFamily;
	//初始表初始化
	bool isConnected();
public:
	GroupStruct();
	~GroupStruct();
	string getFamily(){return family;}
	string getNamesWithinFamily(){return namesWithinFamily;}
	bool isConnected(){return isConnected;}
	void setFamily(string family_){this->family=family_;}
	void setNames(vector<string> names){
		string s("");
		for(int i=0;i<names.size();i++){
			s.append(names[i]+";");
		}
		this->namesWithinFamily.clear();
		this->namesWithinFamily.append(s);
	}

};


#endif