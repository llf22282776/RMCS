#ifndef INITMANAGER_H
#define INITMANAGER_H
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



	InitManager();
	~InitManager();

private:

};
class ConfigReader{




};



#endif