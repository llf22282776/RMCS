#ifndef	LOOKUPMANAGER_H
#define LOOKUPMANAGER_H
#include "CThread.h"
#include <map>
#include <vector>
#include <string>
#include "src/lookup.hpp"
using namespace std;
#include "CacheManager.h"
#include "queue_safe.h"
#include <list>
#include "FeedBackManager.h"
#include "ConfigManager.h"
#include "JsonObjectBase.h"
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
	*/

private:
	//------------------------
	hebi::Lookup& lookup; //hebi接口对象

	queue_safe<GroupfeedbackCustomStruct>& groupFeedbackQueue;//收到的异步groupfeedback都应该放到这里

	//group会增加和减少,会不会影响什么？估计不会
	map<string,unique_ptr<hebi::Group>> cacheGroupMap;//需要更新的map,从缓存里面取，如果key没有就取一下addFh,有就不管
	vector<FeedBackManager&> feedbackManagerVec;
	vector<GroupStruct> fixedGroup;
	CacheManager& cacheManager;
	ConfigManager& configManager;
	int sleep_time; //休眠时间，取默认值
	bool fixedAdded;
	//----------------------
	


	//----------------------
	void updateFamilyAndNamesMap(map<string,vector<string>> newFandNMap);//更新缓存中的map,指针在函数内进行销毁
	map<string,vector<string>> getNewestMapFromHibi();//获得最新的family map的引用,返回处的函数中进行销毁
	bool isFirstRun;//是否第一次运行，是的话为true，否则为false,初始化为false,第一次run的时候不去读，等sleep时间,然后getEntryList

	vector<GroupStruct> getGroupListFromCache();//从缓存里获取当前用户定义的group的信息,到map去查
	void updateGroupConncetState(vector<GroupStruct> groupInCache,int default_timeout=0);//刷新group的连接状态,这里面的
	bool updateGroupsStateInCache(vector<GroupStruct> groupStrut);//这个放到cacheManager的队列里面去，能放进去就是true
	vector<GroupStruct> getGroupsStateFromHeibi(vector<GroupStruct> groupVec,int default_int);//从heibi里面获得cahce里面定义的group的连接状态
	
	

	vector<GroupStruct> getGroupListFromConfig();//从配置文件里面获取
	
	void addHandlerFromGroups();//为fixed和缓存的group加处理函数
	void addHandlerForOneGroup(vector<string>* &familyVec,vector<string>* &nameVec,string groupName);//添加一个group的
	void getFamilyAndNamesFromGroupStruct(GroupStruct& thisGroup,vector<string>* &familysVec,vector<string>* &namesVec);

	//-----------------------
public:
	LookUpManager(
		CacheManager& cacheManager,
		queue_safe<GroupfeedbackCustomStruct>& groupFeedbackQueue,
		Lookup& lookup_,
		ConfigManager& configManager_,
		int sleep_time=DEAULT_SLEEP_TIME

		);
	//需要什么参数,缓存管理类，简单，不需要信息沟通,没必要生产者消费者模型,
	//传进来缓存管理对象，队列对象，
	//引用要在初始化列表中赋值
	//lookup在里面创建
	~LookUpManager();
	void run() override; // override run function,to find 
	void init() ;//init the lookupManager,比如跑起来
	hebi::Lookup getLookUp(){return this->lookup;}
	//-------------------
	static const int  DEAULT_SLEEP_TIME=50; //默认的休眠时间


}; 

class FamilyStruct :public CJsonObjectBase{
private:
	string name;
	vector<NameStruct> nameList;

public:
	FamilyStruct() {

	}
	FamilyStruct(string name) { this->name = name; }
	~FamilyStruct() {}
	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addName(string name) {
		NameStruct name_(name,false);
		nameList.push_back(name_);
		// Get all the names of members in a string, with ';' as spliter
	}
	void setNameList(vector<NameStruct> namelist) { 
		this->nameList = namelist;		
	}
	vector<NameStruct>   getNameList() { return nameList; }
	virtual void SetPropertys(){
		//
		this->SetProperty("name",CJsonObjectBase::asString,&(this->name));
		this->SetProperty("nameList",CJsonObjectBase::asVectorArray,&(this->nameList));
	}
};

class NameStruct:public CJsonObjectBase{

public:
	string name;
	bool connected;
	NameStruct(string name_,bool connect_):name(name_),connected(connect_){}
	~NameStruct(){}
	virtual void SetPropertys(){
		//
		this->SetProperty("name",CJsonObjectBase::asString,&(this->name));
		this->SetProperty("connected",CJsonObjectBase::asBool,&(this->connected));
	}
};
class GroupStruct:public CJsonObjectBase{
private:
	string name;
	vector<FamilyStruct> familyList;
	//初始表初始化

public:
	GroupStruct():familyList(),name(""),isConnected(false){}
	GroupStruct(string name_) :familyList(),name(name_),isConnected(false){}
	~GroupStruct() {}
	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addFamily(FamilyStruct family) {
		familyList.push_back(family);
	}
	vector<FamilyStruct> getFamilyList() { return familyList; }
	virtual void SetPropertys(){
		//
		this->SetProperty("name",CJsonObjectBase::asString,&(this->name));
		this->SetProperty("familyList",CJsonObjectBase::asVectorArray,&(this->familyList));
	}

};

#endif