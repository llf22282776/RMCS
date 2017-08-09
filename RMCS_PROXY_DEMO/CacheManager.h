#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"
#include <queue>
#include <string>
#include <queue>
#include "LookUpManager.h"
#include <map>
#include <vector>
using namespace std;
using namespace hebi;
class CacheManager:public CThread
{
	/**
	这个类是个线程类，自身维护两个队列，
	让feedback消费者放到feedback待存队列里面
	lookup会更新缓存里面的familyAndNames map
	提供缓存中的列表或者map的操作接口
	*/
private:
	bool isConnect;//是否已经连接
	queue<map<string,vector<string>>> family_name_queue;
	queue<vector<GroupStruct>> group_struct_queue;//主要用来刷
public:
	CacheManager(string ip_,string port_);
	~CacheManager();
	void initCacheManager();//链接，跑线程等等操作
	
	vector<GroupStruct> getGroupInCache();//获取cache中的list
	bool updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap);
	//函数内部放入队列
	//如果是连接上的，那么返回的是true
	//这个函数通常在别的线程掉
	//加锁
	bool updateCacheGroupStateList(vector<GroupStruct> groupStructVec);
	//放入队列
	//锁
	bool isConnected();//是否正在连接上的
	void customfamilyMap();//消耗队列里面的familyMap，锁
	void customGroupStateMap(); // 消耗队列里面的groupStructVec，锁
	void setIpAndPort(string ip_,string port_);//重新设置
	void run() override;
private:

};
class CacheConnection{
	/*
	这个类管理底层对缓存的连接
	init连接缓存
	get/set 方法
	
	*/
private:
	string ip;
	string port;
public:
	CacheConnection(string ip_,string port_);
	~CacheConnection();
	bool connect();//连接
	bool reconnect();//重新连接
	bool disconnect();//断开
	bool isConnected();//是否连接
	void setIpAndPort(string ip_,string port_);//重新设置

};

#endif // !
