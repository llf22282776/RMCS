#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"
class CacheManager:public CThread
{
	/**
	这个类是个线程类，自身维护两个队列，
	让feedback消费者放到feedback待存队列里面
	lookup会更新缓存里面的familyAndNames map
	提供缓存中的列表或者map的操作接口
	*/
public:
	CacheManager();
	~CacheManager();
	void run() override;
private:

};
class CacheConnection{
	/*
	这个类管理底层对缓存的连接
	init连接缓存
	get/set 方法
	
	*/




};






#endif // !
