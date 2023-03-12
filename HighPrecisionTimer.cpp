/* 
创作者 ： 景浩淼
创作时间 ： 2023/3/10
解释说明 ： 一种高精度的定时器，采用 QueryPerformanceCounter 的API实现微秒（μs）级别的定时，解决了多核CPU的情况下不准确的问题
头文件 ： HighPrecisionTimer.h
*/

/*
* *******************************************************************************************************
* 这是头文件
*/
#include "HighPrecisionTimer.h"


/*
* *******************************************************************************************************
* 这是HighPrecisionTimerClass类的公有方法实现
*/
/// <summary>
/// 此函数是HighPrecisionTimerClass类的一个构造函数
/// </summary>
/// <param name="_interval">此参数为定时器的间隔时长，单位为微秒（μs），请尽量写大的数值</param>
/// <param name="_end_time">此参数为定时器的关闭时间，单位为微秒（μs），如果想不关闭，请写入-1，其余负数不可写入</param>
/// <param name="Timer_proc">此参数为定时器间隔处理函数的指针，注意的是，此函数无形参与返回值</param>
/// <param name="Timer_time_out_proc">此参数为定时器超时退出处理函数的指针，注意的是，此函数无形参与返回值，可以为NULL</param>
/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
HighPrecisionTimerClass::HighPrecisionTimerClass(unsigned long long _interval, long long _end_time, void (*Timer_proc)(), void (*Timer_time_out_proc)(), bool _begin_first)
{
	/*写入参数，因为是面向程序员，所以不写参数合法检测程序*/
	this->_interval = _interval;//设置定时器间隔时长
	this->_end_time = _end_time;//设置定时器关闭时间
	this->Timer_proc = Timer_proc;//设置定时器间隔处理函数的指针
	this->Timer_time_out_proc = Timer_time_out_proc;//设置定时器超时退出处理函数的指针
	if (_begin_first) this->_next = 0; else this->_next = _interval;//如果不先执行间隔处理函数，将下一次时间改为间隔时间
}

/// <summary>
/// 此函数是HighPrecisionTimerClass类的一个构造函数，默认不关闭定时器
/// </summary>
/// <param name="_interval">此参数为定时器的间隔时长，单位为微秒（μs），请尽量写大的数值</param>
/// <param name="Timer_proc">此参数为定时器间隔处理函数的指针，注意的是，此函数无形参与返回值</param>
/// <param name="Timer_time_out_proc">此参数为定时器超时退出处理函数的指针，注意的是，此函数无形参与返回值，可以为NULL</param>
/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
HighPrecisionTimerClass::HighPrecisionTimerClass(unsigned long long _interval, void (*Timer_proc)(),void(*Timer_time_out_proc)(),bool _begin_first)
{
	this->HighPrecisionTimerClass::HighPrecisionTimerClass(_interval,-1, Timer_proc, Timer_time_out_proc, _begin_first);
}

/// <summary>
/// 此函数是HighPrecisionTimerClass类的一个构造函数，并且为空构造函数
/// </summary>
HighPrecisionTimerClass::HighPrecisionTimerClass(){}

/// <summary>
/// 此函数为开启定时器函数，使用此函数前，请先确认参数设置完成
/// </summary>
/// <returns>返回值为bool型，true代表开启成功，false代表失败</returns>
/// <param name="_begin_first">此参数是定义开始定时时是否先执行间隔处理函数</param>
bool HighPrecisionTimerClass::Start_high_precision_timer(bool _begin_first)
{
	if (_begin_first) this->_next = 0; else this->_next = this->_interval;//如果不先执行间隔处理函数，将下一次时间改为间隔时间

	/*获取初始化性能计数器频率*/
	LARGE_INTEGER _bim;
	if (!QueryPerformanceFrequency(&_bim)) return false;//如果返回0值，表示计算机不支持该硬件
	this->_init = _bim.QuadPart / 1000000;

	/*定时器线程启动*/
	this->__is_start_timer = true;//状态标志位设为true
	this->_high_precision_timer_thread = CreateThread(NULL, 0,HighPrecisionTimerClass::High_precision_timer_thread_proc, this, 0, NULL);//创建并启动定时器线程，并将对象指针传入
	if (this->_high_precision_timer_thread == NULL) return false;//判断进程创建是否成功

	/*获取定时器线程掩码*/
	Get__high_precision_timer_mask();

	/*绑定定时器线程到单一处理器*/
	return Bind_threads_to_core();
}

/// <summary>
/// 此函数为关闭定时器函数，使用此函数前，请先定时器已开启
/// </summary>
/// <returns>返回值为bool型，true代表关闭成功，false代表失败</returns>
bool HighPrecisionTimerClass::Stop_high_precision_timer()
{
	/*将定时器与单一处理器解绑*/
	this->Untie_threads_to_core();

	/*关闭定时器线程*/
	this->__is_start_timer = false;//通过将标志位设置为false，使得线程手动结束
	return true;
}

/*
* *******************************************************************************************************
* 这是HighPrecisionTimerClass类的私有方法实现
*/
/// <summary>
/// 此函数用于获取定时器的线程掩码
/// </summary>
void HighPrecisionTimerClass::Get__high_precision_timer_mask()
{
	/*局部变量创建*/
	ULONG_PTR _pro_mask;					//进程掩码
	ULONG_PTR _sys_mask;					//系统掩码

	/*获取进程掩码与系统掩码*/
	GetProcessAffinityMask(GetCurrentProcess(), &_pro_mask, &_sys_mask);//通过此函数获取正在运行此代码的进程的掩码与系统掩码
	
	/*设置定时器线程掩码*/
	if (!_pro_mask) _pro_mask = 1;//如果进程掩码为0，则考虑只有一个内核可用
	/*寻找最低位的内核，设置为定时器线程掩码*/
	this->_high_precision_timer_mask = 1;//初始化定时器线程掩码为1
	while (!(this->_high_precision_timer_mask & _pro_mask))//循环找到最低位的进程掩码
	{
		this->_high_precision_timer_mask <<= 1;
	}
}

/// <summary>
/// 此函数是将定时器线程与单一处理器绑定，注意的是，使用此函数之前，请先获取定时器的线程掩码
/// </summary>
/// <returns>返回值为bool型，true代表绑定成功，false代表失败</returns>
bool HighPrecisionTimerClass::Bind_threads_to_core()
{
	/*将定时器线程与单一处理器绑定*/
	this->_high_precision_timer_old_mask = SetThreadAffinityMask(this->_high_precision_timer_thread, this->_high_precision_timer_mask);
	if (this->_high_precision_timer_old_mask) return true; else return false;//判断是否绑定成功
}

/// <summary>
/// 此函数是解除定时器线程与单一处理器绑定，注意的是，使用此函数之前，请先确定定时器线程已经与单一处理器绑定
/// </summary>
/// <returns>返回值为bool型，true代表解除绑定成功，false代表失败</returns>
bool HighPrecisionTimerClass::Untie_threads_to_core()
{
	/*解除定时器线程与单一处理器绑定*/
	this->_high_precision_timer_old_mask = SetThreadAffinityMask(this->_high_precision_timer_thread, this->_high_precision_timer_old_mask);
	if (this->_high_precision_timer_old_mask) return true; else return false;//判断是否解除绑定成功
}

/*
* *******************************************************************************************************
* 这是HighPrecisionTimerClass类的静态私有方法实现
*/
/// <summary>
/// 此函数是定时器线程处理函数
/// </summary>
/// <param name="lpParamater">传入的HighPrecisionTimerClass类指针</param>
/// <returns></returns>
DWORD WINAPI HighPrecisionTimerClass::High_precision_timer_thread_proc(LPVOID lpParamater)
{
	/*参数创建*/
	HighPrecisionTimerClass* _lp_high_precision_timer = (HighPrecisionTimerClass*)lpParamater;//获取传入的HighPrecisionTimerClass类指针
	LARGE_INTEGER _now;//用来存储当前获取的时间

	/*初始化开始定时时间*/
	QueryPerformanceCounter(&(_lp_high_precision_timer->_start));//获取开始定时时间

	/*此处开始循环获取定时器时间*/
	while (1)
	{
		if (!(_lp_high_precision_timer->__is_start_timer))  return 0;//定时器手动停止
		QueryPerformanceCounter(&_now);//获取当前的时间
		long long _past_time = (_now.QuadPart - _lp_high_precision_timer->_start.QuadPart)/ _lp_high_precision_timer->_init;//计算出过去的时间，单位微秒（μs）
		if (_lp_high_precision_timer->_end_time != -1 && _past_time >= _lp_high_precision_timer->_end_time) //如果达到结束时间，退出线程
		{ 
			if (_lp_high_precision_timer->Timer_time_out_proc != NULL) _lp_high_precision_timer->Timer_time_out_proc();//定时器超时处理函数执行
			_lp_high_precision_timer->Untie_threads_to_core(); 
			_lp_high_precision_timer->__is_start_timer = false;
			return 0; 
		}
		if (_past_time >= _lp_high_precision_timer->_next)//达到下一次间隔时间
		{
			_lp_high_precision_timer->Timer_proc();//执行间隔处理函数
			_lp_high_precision_timer->_next += _lp_high_precision_timer->_interval;//计算下一次间隔执行的时间
		}
		/*降低cpu占用，根据实际情况，可以进行修改，取决于Sleep误差*/
		if (_lp_high_precision_timer->_next - _past_time > 30000)//如果距离下一个间隔超过30ms，用Sleep函数睡眠20ms
			Sleep(20);
			//Sleep(min(1000, (_lp_high_precision_timer->_next - _past_time)/1000-10));//降低CPU占用
	}
}



///*测试程序，打包库时请注释掉*/
///*测试用的全局变量*/
//LARGE_INTEGER __tmp;
//LARGE_INTEGER __start;
//LARGE_INTEGER __end;
//int num = 0;
//int num2 = 0;
//long long accurate = 0;//准确
//long long error = 0;//误差
///*测试用的第一定时器执行函数*/
//void proc()
//{
//	num++;
//	QueryPerformanceCounter(&__end);
//	std::cout << "第一定时器：这是第" << num << "次执行，用时：" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart/1000000) << "us\n";
//}
///*测试用的第二定时器执行函数*/
//void proc2()
//{
//	QueryPerformanceCounter(&__end);
//	long long z = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) - accurate;
//	error += z>0?z: accurate- (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000);
//	accurate = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) + 500000;
//	num2++;
//	std::cout << "第二定时器：这是第" << num2 << "次执行，用时：" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) << "us\n";
//}
///*测试程序入口*/
//int main()
//{
//	//因为测试程序在同一窗口打印文字，多个定时器同时开启时，会使打印的文字相互干扰，顺序混乱，但能看出两个定时器均正常运行
//	std::cout << "开始\n";
//	QueryPerformanceFrequency(&__tmp);
//	QueryPerformanceCounter(&__start);
//	HighPrecisionTimerClass _high_precision_timer(166666,30000000,proc,NULL,true);//166.666ms间隔,30s结束,开始执行
//	HighPrecisionTimerClass _high_precision_timer2(500000, 30000000, proc2, NULL, true);//500.000ms间隔,30s结束,开始执行
//
//	_high_precision_timer.Start_high_precision_timer(true);//开始定时器1
//	_high_precision_timer2.Start_high_precision_timer(true);//开始定时器2
//
//	Sleep(10000);
//	_high_precision_timer.Stop_high_precision_timer();//手动关闭计时器1
//
//	Sleep(30000);
//
//	std::cout << "平均误差：" << error/60 << "us\n";
//	system("pause");
//
//	return 0;
//}