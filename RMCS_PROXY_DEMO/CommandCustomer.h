#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "LookUpManager.h"
#include "FeedBackManager.h"
class CommandCustomer:public CThread
{

	//这是一个线程类，
	//不停的消耗commandQueue队列中的内容
	//commandQueue队列中，每一个元素，都指定了要发给命令的group或者其中的module
	//以及命令的格式
	//元素的类型需要在这里定义
	//feedback会被feedbackcustomer处理

public:
	CommandCustomer();
	~CommandCustomer();
	void run() override; //重写run
	void init();//
	void getCommandStrut(string ct);
	bool customCommand();
private:

};
class CommandStruct{

	string cmd;
	GroupfeedbackCustomStruct gf;
	


};
#endif // !
