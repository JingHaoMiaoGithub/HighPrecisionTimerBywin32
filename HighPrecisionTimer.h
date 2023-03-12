/*
创作者 ： 景浩淼
创作时间 ： 2023/3/10
解释说明 ： 一种高精度的定时器，采用 QueryPerformanceCounter 的API实现微秒（μs）级别（经过测试，误差稳定在几百微秒）的定时，解决了多核CPU的情况下不准确的问题
实现文件 ： HighPrecisionTimer.cpp
*/

/*
* ******************************************************************************************************
* 这里是所需要的头文件
*/
#pragma once
#include <iostream>
#include <windows.h>//win32 API头文件

/*
* ******************************************************************************************************
* 这里是HighPrecisionTimerClass类的定义
*/
class HighPrecisionTimerClass {
public:											//公有属性与方法
	/*公有属性*/
	bool __is_start_timer = false;				//定时器状态标志位
	long long _init;							//此变量用于保存当前性能计数器频率（以微秒计数为单位）
	LARGE_INTEGER _start;						//此变量用于保存开始定时的时间
	long long _next;							//此变量用于保存下一次间隔的时间
	unsigned long long _interval;				//此变量用于保存定时间隔的时间，单位微秒（μs），但是请尽量大，因为间隔过小时，处理函数的调用越多，CPU占有变大，定时也不准
	long long _end_time = -1;					//此变量用于保存结束定时的时间，单位微秒（μs），当需要一直开启定时时，请置为-1
	void (*Timer_proc)() = NULL;				//此变量是一定时间隔处理函数的指针
	void (*Timer_time_out_proc)() = NULL;		//此变量是定时器超时退出处理函数的指针

	/*公有方法*/
	/// <summary>
	/// 此函数是HighPrecisionTimerClass类的一个构造函数
	/// </summary>
	/// <param name="_interval">此参数为定时器的间隔时长，单位为微秒（μs），请尽量写大的数值</param>
	/// <param name="_end_time">此参数为定时器的关闭时间，单位为微秒（μs），如果想不关闭，请写入-1，其余负数不可写入</param>
	/// <param name="Timer_proc">此参数为定时器间隔处理函数的指针，注意的是，此函数无形参与返回值</param>
	/// <param name="Timer_time_out_proc">此参数为定时器超时退出处理函数的指针，注意的是，此函数无形参与返回值，可以为NULL</param>
	/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
	HighPrecisionTimerClass(unsigned long long _interval, long long _end_time, void (*Timer_proc)(), void(*Timer_time_out_proc)(), bool _begin_first);
	/// <summary>
	/// 此函数是HighPrecisionTimerClass类的一个构造函数，默认不关闭定时器
	/// </summary>
	/// <param name="_interval">此参数为定时器的间隔时长，单位为微秒（μs），请尽量写大的数值</param>
	/// <param name="Timer_proc">此参数为定时器间隔处理函数的指针，注意的是，此函数无形参与返回值</param>
	/// <param name="Timer_time_out_proc">此参数为定时器超时退出处理函数的指针，注意的是，此函数无形参与返回值，可以为NULL</param>
	/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
	HighPrecisionTimerClass(unsigned long long _interval, void (*Timer_proc)(), void(*Timer_time_out_proc)(), bool _begin_first);
	/// <summary>
	/// 此函数是HighPrecisionTimerClass类的一个构造函数，并且为空构造函数
	/// </summary>
	HighPrecisionTimerClass();
	/// <summary>
	/// 此函数为开启定时器函数，使用此函数前，请先确认参数设置完成
	/// </summary>
	/// <returns>返回值为bool型，true代表开启成功，false代表失败</returns>
	/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
	bool Start_high_precision_timer(bool _begin_first);
	/// <summary>
	/// 此函数为关闭定时器函数，使用此函数前，请先定时器已开启
	/// </summary>
	/// <returns>返回值为bool型，true代表关闭成功，false代表失败</returns>
	bool Stop_high_precision_timer();

private:										//私有属性与方法
	/*私有属性*/
	HANDLE _high_precision_timer_thread;		//此变量为定时器线程句柄
	ULONG_PTR _high_precision_timer_mask;		//此变量为定时器运行的处理器的掩码
	ULONG_PTR _high_precision_timer_old_mask;	//此变量为初始时定时器运行的处理器的掩码

	/*私有方法*/
	/// <summary>
	/// 此函数用于获取定时器的线程掩码
	/// </summary>
	void Get__high_precision_timer_mask();
	/// <summary>
	/// 此函数是将定时器线程与单一处理器绑定，注意的是，使用此函数之前，请先获取定时器的线程掩码
	/// </summary>
	/// <returns>返回值为bool型，true代表绑定成功，false代表失败</returns>
	bool Bind_threads_to_core();
	/// <summary>
	/// 此函数是解除定时器线程与单一处理器绑定，注意的是，使用此函数之前，请先确定定时器线程已经与单一处理器绑定
	/// </summary>
	/// <returns>返回值为bool型，true代表解除绑定成功，false代表失败</returns>
	bool Untie_threads_to_core();

	/*私有静态方法*/
	/// <summary>
	/// 此函数是定时器线程处理函数
	/// </summary>
	/// <param name="lpParamater">传入的HighPrecisionTimerClass类指针</param>
	/// <returns></returns>
	static DWORD WINAPI High_precision_timer_thread_proc(LPVOID lpParamater);
};