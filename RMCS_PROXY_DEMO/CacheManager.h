#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"

#include <string>
#include "queue_safe.h"
#include "LookUpManager.h"
#include <map>
#include <vector>
#include "hiredis/hiredis.h"
#include <mutex>
#include <condition_variable>
#define PUT_LIST "put_list"
#define GET_LIST "get_list"
#define GET_STR  "get_str"
#define ADD_VALUE_TO_SET  "add_value_to_set"
#define DELETE_VALUE_TO_SET "delete_value_to_set"
#define SET_STR "set_str"
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
	queue_safe<map<string,vector<string>>> family_name_queue;
	queue_safe<vector<GroupStruct>> group_struct_queue;//主要用来刷
	queue_safe<GroupfeedbackCustomStruct> group_feedback_queue;
	mutable std::mutex mut;   
	bool canUseRedis;

	std::condition_variable data_cond;  
public:
	CacheManager(string ip_,int port_,int sleep_time=LookUpManager::DEAULT_SLEEP_TIME);
	~CacheManager();
	void initCacheManager();//链接，跑线程等等操作
	CacheConnection cacheConnect;
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
	void customGroupFeedBack(); //消耗队列里面的groupFeedBack
	void setIpAndPort(string ip_,string port_);//重新设置
	void run() override;//重置运行函数
	bool reconnect();//重新连接
	bool isContainKey(char*  key);
	bool deleteKey();//删除键
	bool pushNewFamily_list_ToCache_mtx(const string& key ,vector<string>& list);//把东西放进缓存里面
	bool updateGroupFeedBack(GroupfeedbackCustomStruct gfd);//更新缓存feedbacklist,只增不减
	 
	int sleep_time;
private:
	vector<GroupStruct> getGroupInCache_pri(string des);//获取cache中的list
	bool flushCacheGroupState(GroupStruct gst);//刷新缓存里面的groupstrut
	bool flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd);//刷新远端缓存里面的groupFeedback
};
class CacheConnection{
	/*
	这个类管理底层对缓存的连接
	init连接缓存
	get/set 方法
	
	*/
private:
	string ip;
	int port;
	redisContext* rdc;
public:
	CacheConnection(string ip_,int port_):ip(ip_),port(port_),rdc(NULL){}
	~CacheConnection();



	bool connect(){
		const string ip_=this->ip;
		rdc= redisConnect(this->ip.data(),this->port);
		return rdc==NULL?false:true;

	}//连接
	bool reconnect(){
		if(this->isConnected()){
			return true;

		}else
		{
			const string ip_=this->ip;
			rdc= redisConnect(this->ip.data(),this->port);
			return rdc==NULL?false:true;
		}


	}//重新连接
	bool disconnect(){
		freeContext();
		rdc=NULL;

	}//断开
	bool isConnected(){
		
		return rdc==NULL?false:true;


	}//是否连接
	bool init(){
		return this->connect();

	}// 连接
	void setIpAndPort(string ip_,int port_){
		this->ip=ip_;
		this->port=port_;
	}//重新设置

	void freeContext(){
		redisFree(this->rdc);
	}//清除context
	bool isContainKey(char*  key){
		void* intP;
		int arg_nums=2;
		char* args[]={"exists",key};
		this->setCommndWithArgs(arg_nums,args,"keyExist",intP);
		return (*(int*)intP == 0)?false:true;


	}//是否包含Key
	bool deleteKey(char*  key){
		void* intP;
		int arg_nums=2;
		char* args[]={"del",key};
		this->setCommndWithArgs(arg_nums,args,"keyDelete",intP);
		return (*(int*)intP == 0)?false:true;

	}//删除键
	bool setCommndWithArgs(int agr_nums,char** args,string des,void* &res){
		if(this->isConnected() == false) return false;
		 redisCommandArgv(this->rdc,agr_nums,args,NULL);
		 redisReply *reply;
		 redisGetReply(rdc,&reply);
		 switch (reply->type)
		 {
		 case REDIS_REPLY_ARRAY:{
			 //是数组，那么就需要读数组
			 vector<string>* vec=new vector<string>();

			 for(int i=0;i<reply->elements;i++){
				 vec->push_back((*(reply->element))[i].str);
			 }
			 res=(void*)vec;
			 return true;
		
		  }
		 case REDIS_REPLY_INTEGER:{
			 LONGLONG* a= new LONGLONG();
			  (*a)=reply->integer;
			   res=(void*)a;
			   return true;
								}
		 case REDIS_REPLY_STRING:{
			 string* a= new string(reply->str);
			 res=(void*)a;
			 return true;

								}
		 case REDIS_REPLY_NIL:{
			 cout<<"error:get null from redis"<<endl;
			  return false;
								}
		 case REDIS_REPLY_ERROR:{
			  cout<<"error:executed error"<<endl;
			  return false;
								}
		 case REDIS_REPLY_STATUS:{
			 //某个操作的，比如DEL SET 放东西的操作,删除和查键就不算在里面了
			 if(des == GET_LIST){
				 //获取列表,不会触发

			 }else if(des == PUT_LIST ){
				 //放列表的操作
				 return  reply->integer<=0?false:true;

			 }else if(des == ADD_VALUE_TO_SET){
				 //创建或添加set
				  return  reply->integer<=0?false:true;

			 }else if(des == DELETE_VALUE_TO_SET){
				 //删除某个set的成员
				 return  reply->integer<=0?false:true;
			 }else if(des == GET_STR){
				 //获取字符串(序列化对象),不会到这里


			 }else if(des == SET_STR){
				 //设置字符串（序列化对象）
				 return strcmp(reply->str,"OK")==0;
			 }else {
				 //奇怪的操作
				 return false;
			 }


			  cout<<reply->str<<endl;
			  return true;
								}
		 default :
			 cout<<"unexcepted error"<<endl;
			 return false;
			 break;
		 }


	}//发送命令


};

#endif // !