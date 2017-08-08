#ifndef	FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H
#include "src/group_feedback.hpp"

class FeedBackManager{
	//所有的feedback已经被管理起来了
	//认为这个类的对象运行在线程中,即允许在生产者线程之中
	//这里需要定义feedback的可读化形式结构
	//1.需要feedBackQueue
	//2.需要记得上锁
	//
	typedef struct feedback_ele{



	};





};
#endif