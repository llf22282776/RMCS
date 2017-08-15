#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "queue_safe.h"
//#include "JsonObjectBase.h"
#include "common.h"


#include "ConfigManager.h"
#include "LookUpManager.h"
#include "lookup.hpp"
using namespace  std;
class CommandCustomer:public CThread
{
	//这是一个线程类，
	//不停的消耗commandQueue队列中的内容
	//commandQueue队列中，每一个元素，都指定了要发给命令的group或者其中的module
	//以及命令的格式
	//元素的类型需要在这里定义
	//feedback会被feedbackcustomer处理

public:
	CommandCustomer(queue_safe<CommandStruct>& command_struct_queue_,ConfigManager& cfg,Lookup& lm,int sleepTime_=DEFAULT_SLEEP_TIME):command_struct_queue(command_struct_queue_),cfgManager(cfg), lookup(lm),sleeptime(sleepTime_){


	}
	~CommandCustomer() {
	
	
	
	
	}
	void run() override; //重写run
	void init(){
		this->start();


	}//
	hebi::GroupCommand getGroupCommandStrut(CommandStruct ct){




	}
	hebi::Command getCommandStrut(CommandStruct ct){




	}
	bool customCommand(){
		
		printf("COMMAND_CUSTOMER : send led command\n");
		shared_ptr<CommandStruct> mapPtr  = this->command_struct_queue.wait_and_pop();
		if(!mapPtr)return false;//没有取到需要消耗的gfd
		unique_ptr<hebi::Group> g= this->lookup.getGroupFromNames(mapPtr->names,mapPtr->familys);
		printf("COMMAND_CUSTOMER : g is not null:%d\n",g!=NULL);
		if (!g){
			printf("COMMAND_CUSTOMER : g is  null\n");
			return false;
		}
		hebi::GroupCommand command(g->size());
		uint8_t r=254;
		uint8_t g1=0;
		uint8_t b=0;
		for(int i=0;i<g->size();i++){
			hebi::Color c(r,g1,b);
			command[i].led().setOverrideColor(c);
		}
		int timeout=100;
		if (g->sendCommandWithAcknowledgement(command, timeout))
		{
			
			printf("COMMAND_CUSTOMER : already sent!");
		}
		else
		{
		
			printf("COMMAND_CUSTOMER : send time out\n");
		}
		return true;
	}
private:
	queue_safe<CommandStruct>& command_struct_queue;
	ConfigManager& cfgManager;
	Lookup& lookup;
	int sleeptime;
};

void CommandCustomer::run(){
	while (true)
	{
		printf("COMMAND_CUSTOMER : command customer working!!!!!\n");
		this->customCommand();
		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}
		printf("COMMAND_CUSTOMER : command customer is ready for next!!!!!\n");
	}

};


#endif // !
