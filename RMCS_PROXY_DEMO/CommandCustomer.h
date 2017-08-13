#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "queue_safe.h"
#include "JsonObjectBase.h"
#include "common.h"
#include "group.hpp"
#include "group_command.hpp"
#include "group_feedback.hpp"
#include "src/lookup.hpp"
#include "src/feedback.hpp"
#include "src/command.hpp"

#include "ConfigManager.h"
#include "LookUpManager.h"
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
	CommandCustomer(queue_safe<CommandStruct>& command_struct_queue_,ConfigManager& cfg,LookUpManager& lm,int sleepTime_=LookUpManager::DEAULT_SLEEP_TIME):command_struct_queue(command_struct_queue_),cfgManager(cfg),lookupManager(lm),sleeptime(sleepTime_){


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
		cout<<"send led command"<<endl;
		shared_ptr<CommandStruct> mapPtr  = this->command_struct_queue.try_pop();
		//if(!mapPtr)return false;//没有取到需要消耗的gfd
		unique_ptr<hebi::Group> g= this->lookupManager.getLookUp().getGroupFromNames(mapPtr->names,mapPtr->familys);
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
			std::cout << "already sent!" << std::endl;
		}
		else
		{
			std::cout << "send time out" << std::endl;
		}
		return true;
	}
private:
	queue_safe<CommandStruct>& command_struct_queue;
	ConfigManager& cfgManager;
	LookUpManager& lookupManager;
	int sleeptime;
};

void CommandCustomer::run(){
	while (true)
	{

		customCommand();
		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}

	}

};


#endif // !
