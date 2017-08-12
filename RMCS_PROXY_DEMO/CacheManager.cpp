#include "CacheManager.h"
#include "include/json/json.h"
#include <mutex>
#include <condition_variable>

CacheManager::CacheManager(string ip_,int port_,int sleep_time_=LookUpManager::DEAULT_SLEEP_TIME):cacheConnect(ip_,port_),canUseRedis(true),sleep_time(sleep_time_){




}
CacheManager::~CacheManager(){}
void CacheManager::initCacheManager(){
	this->isConnect=this->cacheConnect.init();
	this->start();//跑线程


}//链接，跑线程等等操作

vector<GroupStruct> CacheManager::getGroupInCache(){
	//别的线程里面
		std::unique_lock<std::mutex> lk(mut);  
		return this->getGroupInCache_pri(GET_LIST);

}//获取cache中的list,需要锁

bool CacheManager::updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap){
	//已经在队列函数中处理了锁
	if(this->isConnect == false)return false;
	else {
		this->family_name_queue.push(familyMap);
		return true;
	}

}
//函数内部放入队列
//如果是连接上的，那么返回的是true
//这个函数通常在别的线程掉

bool CacheManager::updateCacheGroupStateList(vector<GroupStruct> groupStructVec){
	if(this->isConnect == false)return false;
	else {
		this->group_struct_queue.push(groupStructVec);
		return true;
	}

}
//放入队列
//锁
bool CacheManager::isConnected(){return this->isConnect;}//是否正在连接上的
void CacheManager::customfamilyMap(){
	shared_ptr<map<string,vector<string>>> mapPtr  = this->family_name_queue.try_pop();
	if(!mapPtr)return ;//没有取到
	map<string,vector<string>>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){
			
			//放进family的集合里面去
			//缓存里面有family的set 和group 的 set 
			this->pushNewFamily_list_ToCache_mtx(it->first,it->second);

	
	}


}//消耗队列里面的familyMap，锁
void CacheManager::customGroupStateMap(){
	//
	shared_ptr<vector<GroupStruct>> mapPtr  = this->group_struct_queue.try_pop();
	if(!mapPtr)return ;//没有取到需要消耗的groupVec
	vector<GroupStruct>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){

		//放之前要先判断下，这个group还在不在缓存里面了，因为有可能被删掉
		char* keyTemp=new char(strlen(it->getName().data())+1);
		if(this->cacheConnect.isContainKey(strcpy(keyTemp,it->getName().data()))){
			this->flushCacheGroupState(*it);
		}
		delete keyTemp;

	}



} // 消耗队列里面的groupStructVec，锁
void CacheManager::setIpAndPort(string ip_,string port_){}//重新设置
void CacheManager::run(){
	while (true)
	{
		customfamilyMap();//消耗下队列
		customGroupStateMap();
		if(this->sleep_time>=0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
	}
}//重置运行函数

bool CacheManager::reconnect(){
	if(this->cacheConnect.isConnected()==false){
		return this->isConnect=this->cacheConnect.reconnect();
	}else
	{
		return this->isConnect = true;
		
	}
}//重新连接




vector<GroupStruct> CacheManager::getGroupInCache_pri(string des){
		//先取 group的list,再取str
		void* groupP=NULL;
		int arg_nums=2;
		char* args[]={"smembers","group",};
		this->cacheConnect.setCommndWithArgs(arg_nums,args,des,groupP);//返回的是向量
		vector<string>& g_strs = *(vector<string>*)groupP;
		vector<GroupStruct> groupV;
		for(int i=0;i<g_strs.size();i++){
			GroupStruct g;
			//取str
			void* str=NULL;
			char* group_key_ = new char(g_strs.size()+1);
			int arg_nums_1=2;
			char* args_1[]={"get",strcpy(group_key_,g_strs.at(i).data())};
			this->cacheConnect.setCommndWithArgs(arg_nums_1,args_1,GET_STR,str);//str已经有了一个str了
			g.DeSerialize((char*)str);
			delete str;
			delete group_key_;
			delete args_1;
			groupV.push_back(g);
			delete args;
			delete groupP;
			return groupV; //返回

	}



}//取数据
//调用缓存类，放进去
bool CacheManager::pushNewFamily_list_ToCache_mtx(const string& key ,vector<string>& list){
	//两个set都要add 
	void* groupP;
	int arg_nums=list.size()+2;
	char* key_=new char(strlen( key.data())+1);

	char** args=new char*[arg_nums];
	args[0]="sadd";
	args[1]=strcpy(key_,key.data());
	for(int i=0;i<list.size();i++){
		args[2+i]=new char(strlen(list.at(i).data())+1);
		strcpy(args[2+i],list.at(i).data());
	}
	bool opt= this->cacheConnect.setCommndWithArgs(arg_nums,args,ADD_VALUE_TO_SET,groupP);

	char* key__=new char(strlen( key.data())+1);
	char* args_[]={"sadd","family",strcpy(key__,key.data())};//放到family set里面去
	int arg_nums_=2;
	void* intx;
	bool opt1= this->cacheConnect.setCommndWithArgs(arg_nums_,args_,ADD_VALUE_TO_SET,intx);
	delete intx;
	delete key_;
	delete key__;
	delete args_;
	delete groupP;
	for(int i=0;i<arg_nums;i++){
		delete args[i];
	}
	delete args;

	return opt==opt1==true;

}
bool CacheManager::flushCacheGroupState(GroupStruct gst){
	//判断有key 再set str
	char* key_=new char(strlen( gst.getName().data())+1);
	string s(gst.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	char* args_[]={"set",strcpy(key_,gst.getName().data()),strcpy(objStr,s.data())};//放到family set里面去
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	delete res;
	delete key_;
	delete objStr;
	delete args_;
	return opt;


}
bool CacheManager::flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd){
	//判断有key 再set str
	string gfd_name_key=gfd.groupName+"_gfd";
	char* key_=new char(strlen(gfd_name_key.data())+1);
	string s(gfd.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	char* args_[]={"rpush",strcpy(key_,gfd_name_key.data()),strcpy(objStr,s.data())};//放到groupfdlist set里面去
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	delete res;
	delete key_;
	delete objStr;
	delete args_;
	return opt;



}//刷新远端缓存里面的groupFeedback
bool CacheManager::updateGroupFeedBack(GroupfeedbackCustomStruct gfd){
	if(this->isConnect == false)return false;
	else {
		this->group_feedback_queue.push(gfd);
		return true;
	}



}//更新缓存feedbacklist,只增不减
void CacheManager::customGroupFeedBack(){
	shared_ptr<vector<GroupfeedbackCustomStruct>> mapPtr  = this->group_feedback_queue.try_pop();
	if(!mapPtr)return ;//没有取到需要消耗的groupVec
	vector<GroupfeedbackCustomStruct>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){
		//放之前要先判断下，这个group还在不在缓存里面了，因为有可能被删掉
		char* keyTemp=new char(strlen(it->groupName.data())+1);
		if(this->cacheConnect.isContainKey(strcpy(keyTemp,it->groupName.data()))){
			this->flushCacheGroupFeedBackList(*it);
		}


	}


} //消耗队列里面的groupFeedBack