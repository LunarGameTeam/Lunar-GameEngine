#pragma once
#include "core/core_module.h"
class GlobelTimeCount
{
	double   count_freq;      //系统时钟频率
	double   delta_time;      //两帧之间时间差
	double   all_timeneed;    //总运行时间
	double   all_pause_time;  //暂停的总时间
	__int64  start_time;      //起始时间
	__int64  stop_time;       //停止时间
	__int64  now_time;        //当前时间
	__int64  last_time;       //上一帧的时间
	bool     if_stop;         //是否暂停
private:
	GlobelTimeCount();          //构造函数
public:
	static GlobelTimeCount* GetInstance()
	{
		static GlobelTimeCount* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new GlobelTimeCount();
		}
		return this_instance;
	}
	void Reset();          //时间重置
	void Start();          //开始计时
	void Stop();           //暂停计时
	void Refresh();        //刷新计时器
	float GetPause();      //获取总的暂停时间
	float GetDelta();      //获取帧间时间差
	float GetAllTime();    //获取总时间
	__int64 GetSystemTime();//获取系统时间
	double GetSystemFreq()
	{
		return count_freq;
	};//获取系统每微秒时间间隔

};