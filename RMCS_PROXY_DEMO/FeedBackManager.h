#ifndef	FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H
#include "src/group_feedback.hpp"
#include "src/feedback.hpp"
#include "queue_safe.h"
#include <vector>
#include "JsonObjectBase.h"
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
	queue_safe<GroupfeedbackCustomStruct>& group_feedback_queue;
	
public:
	 FeedBackManager(queue_safe<GroupfeedbackCustomStruct>& group_feedback_queue_);
	 //往进来放
	~FeedBackManager();
	GroupfeedbackCustomStruct toGroupFbCustomStruct(const GroupFeedback* group_fdb,string groupName);//转换函数
	FeedbackCustomStruct toFeedBackCustomStruct(const Feedback& fdb); //吧每个模块的fb转换成可读化的
	void putToQueue(GroupfeedbackCustomStruct group_fdb_custom);//放进队列，这里面就要锁些东西了
	


};
//groupFeedBack的重置格式
class GroupfeedbackCustomStruct:public CJsonObjectBase{
	


public:
	vector<double> positionsVec;
	vector<double> velocitysVec;
	vector<double> torqueVec;
	string groupName;
	vector<FeedbackCustomStruct> moduleFeedBackVec;

	GroupfeedbackCustomStruct(vector<double> pVec,
		vector<double> vVec,vector<double> tVec,string groupName_):
		positionsVec(pVec),
		velocitysVec(vVec),
		torqueVec(tVec),
		moduleFeedBackVec(),
		groupName(groupName_)
		{
		
	}
	virtual void SetPropertys(){
			this->SetProperty("positionsVec",CJsonObjectBase::asVectorArray,&positionsVec);
			this->SetProperty("velocitysVec",CJsonObjectBase::asVectorArray,&velocitysVec);
			this->SetProperty("torqueVec",CJsonObjectBase::asVectorArray,&torqueVec);
			this->SetProperty("groupName",CJsonObjectBase::asString,&groupName);
			this->SetProperty("moduleFeedBackVec",CJsonObjectBase::asVectorArray,&moduleFeedBackVec);

		}
	~GroupfeedbackCustomStruct();
	bool putIntoModuleFeedBackVec(vector<FeedbackCustomStruct>& fd_custom);//把模块的feedback放进去





};
//group里面module的重置格式
//反正只要我们需要的就好
class FeedbackCustomStruct:public CJsonObjectBase{

	public:
	
		Led_field led_field;
		Actuator_field actuator_field;
		FeedbackCustomStruct(Led_field l_f,Actuator_field a_f):led_field(l_f.led_R,l_f.led_G,l_f.led_B),actuator_field(a_f.position,a_f.velocity,a_f.torque,a_f.voltage){

		}
		virtual void SetPropertys(){
			this->SetProperty("led_field",asJsonObj,&led_field);
			this->SetProperty("actuator_field",asJsonObj,&actuator_field);
			
		}
		~FeedbackCustomStruct();
};
class Led_field:public CJsonObjectBase{
public:
	int led_R;
	int led_G;
	int led_B;
	Led_field(uint8_t R,uint8_t G,uint8_t B):led_R(R),led_G(G),led_B(B){

	}
	virtual void SetPropertys(){
		this->SetProperty("led_R",asInt,&led_R);
		this->SetProperty("led_G",asInt,&led_G);
		this->SetProperty("led_B",asInt,&led_B);
	}
	~Led_field();
};
class Actuator_field:public CJsonObjectBase{

public:
	double position;
	double velocity;
	double torque;
	double voltage;
	Actuator_field(double p,double v ,double t,double vol):position(p),velocity(v),torque(t),voltage(vol){}
	~Actuator_field();
	virtual void SetPropertys(){
		this->SetProperty("position",asDouble,&position);
		this->SetProperty("velocity",asDouble,&velocity);
		this->SetProperty("torque",asDouble,&torque);
		this->SetProperty("voltage",asDouble,&voltage);
	}

};


#endif