#ifndef	FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H
#include "src/group_feedback.hpp"
#include "src/feedback.hpp"
#include <queue>
using namespace std;
using namespace hebi;
class FeedBackManager{
	//所有的feedback已经被管理起来了
	//认为这个类的对象运行在线程中,即允许在生产者线程之中
	//需要定义可读化形式
	//1.需要feedBackQueue
	//2.需要记得上锁
	//3.队列传进来

private:
	queue<GroupFeedback*>& group_feedback_queue;
	
public:
	 FeedBackManager(queue<GroupFeedback*>& group_feedback_queue_);
	 //往进来放
	~FeedBackManager();
	GroupfeedbackCustomStruct toGroupFbCustomStruct(const GroupFeedback* group_fdb);//转换函数
	FeedbackCustomStruct toFeedBackCustomStruct(Feedback* fdb); //吧每个模块的fb转换成可读化的
	void putToQueue(GroupfeedbackCustomStruct group_fdb_custom);//放进队列，这里面就要锁些东西了
	


};
//groupFeedBack的重置格式
class GroupfeedbackCustomStruct{
	


public:
	vector<float> positionsVec;
	vector<float> velocitysVec;
	vector<float> torqueVec;
	vector<FeedbackCustomStruct> moduleFeedBackVec;
	GroupfeedbackCustomStruct(vector<float> pVec,vector<float> vVec,vector<float> tVec):positionsVec(pVec),velocitysVec(vVec),torqueVec(tVec),moduleFeedBackVec(){
		
	}
	~GroupfeedbackCustomStruct();
	bool putIntoModuleFeedBackVec(FeedbackCustomStruct fd_custom);//把模块的feedback放进去





};
//group里面module的重置格式
//反正只要我们需要的就好
class FeedbackCustomStruct{

	public:
		Led_field led_field;
		Actuator_field actuator_field;
		FeedbackCustomStruct(Led_field l_f,Actuator_field a_f):led_field(l_f.led_R,l_f.led_G,l_f.led_B),actuator_field(a_f.position,a_f.velocity,a_f.torque){

		}
		~FeedbackCustomStruct();
};
class Led_field{
public:
	uint8_t led_R;
	uint8_t led_G;
	uint8_t led_B;
	Led_field(uint8_t R,uint8_t G,uint8_t B):led_R(R),led_G(G),led_B(B){

	}
	~Led_field();
};
class Actuator_field{

public:
	float position;
	float velocity;
	float torque;
	Actuator_field(float p,float v ,float t):position(p),velocity(v),torque(t){}
	~Actuator_field();

};


#endif