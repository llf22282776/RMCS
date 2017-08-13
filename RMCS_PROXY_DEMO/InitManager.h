#ifndef INITMANAGER_H
#define INITMANAGER_H

#include "queue_safe.h"
#include "LookUpManager.h"
#include "CacheManager.h"
#include "ConfigManager.h"
#include "CommandCustomer.h"
#include "FeedBackCustomer.h"
#include "FeedBackManager.h"

#include "src/lookup.hpp"
#include <thread>
#include "common.h"
class InitManager
	/*
	这个类是一开始初始化的
	需要从配置文件读取一系列参数
	比如本地数据库/web数据库地址
		默认需要监视的group
	初始化其他manager/customer，让线程类运行起来（如果初始化就有运行操作的话，就不必手动start）
	暂定
	*/


{
public:



	InitManager() {
	
	
	
	}
	static int main(){
		cout << "----init queue!----" << endl;
	
		queue_safe<GroupfeedbackCustomStruct> gfd_queue;
		queue_safe<GroupStruct> gs_queue;
		queue_safe<CommandStruct> cmd_queue;
		
		hebi::Lookup lookup;
		//加载配置
		cout << "----loader config!" << endl;
		ConfigManager cfgManager("resource/config.xml");
		//运行缓存管理
		cout << " config loaded ! init cacheManager" << endl;
		CacheManager cacheManger(cfgManager.getRedisList().at(0).ip,cfgManager.getRedisList().at(0).port);
		//加载数据库管理
		cout << " ----cacheManager inited ! init databaseManager" << endl;
		DataBaseManager db;
		cout << " ----databaseManager inited ! init lookupManager" << endl;
		//运行lookup线程
		LookUpManager lkManager(cacheManger,gfd_queue,lookup,cfgManager);
		cout << " ----lookupManager inited ! init CommandCustomer" << endl;
		//运行命令消费者
		CommandCustomer cmdCusrom(cmd_queue,cfgManager,lkManager);
		cout << " ----CommandCustomer inited ! init FeedBackCustomer" << endl;
		//运行回馈消费者
		FeedBackCustomer fdCustomer(gfd_queue,cacheManger,db);
		//``````````````````````````````````
		cacheManger.initCacheManager();
		lkManager.init();
		cmdCusrom.init();
		fdCustomer.init();
		cout<<"all run !!!"<<endl;
		cacheManger.join();
		lkManager.join();
		cmdCusrom.join();
		fdCustomer.join();
		return 0;
		
	}
	~InitManager() {
	
	
	
	}

private:

};




#endif