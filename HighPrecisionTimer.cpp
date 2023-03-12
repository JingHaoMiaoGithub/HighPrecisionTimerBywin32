/* 
������ �� �����
����ʱ�� �� 2023/3/10
����˵�� �� һ�ָ߾��ȵĶ�ʱ�������� QueryPerformanceCounter ��APIʵ��΢�루��s������Ķ�ʱ������˶��CPU������²�׼ȷ������
ͷ�ļ� �� HighPrecisionTimer.h
*/

/*
* *******************************************************************************************************
* ����ͷ�ļ�
*/
#include "HighPrecisionTimer.h"


/*
* *******************************************************************************************************
* ����HighPrecisionTimerClass��Ĺ��з���ʵ��
*/
/// <summary>
/// �˺�����HighPrecisionTimerClass���һ�����캯��
/// </summary>
/// <param name="_interval">�˲���Ϊ��ʱ���ļ��ʱ������λΪ΢�루��s�����뾡��д�����ֵ</param>
/// <param name="_end_time">�˲���Ϊ��ʱ���Ĺر�ʱ�䣬��λΪ΢�루��s��������벻�رգ���д��-1�����ฺ������д��</param>
/// <param name="Timer_proc">�˲���Ϊ��ʱ�������������ָ�룬ע����ǣ��˺������β��뷵��ֵ</param>
/// <param name="Timer_time_out_proc">�˲���Ϊ��ʱ����ʱ�˳���������ָ�룬ע����ǣ��˺������β��뷵��ֵ������ΪNULL</param>
/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
HighPrecisionTimerClass::HighPrecisionTimerClass(unsigned long long _interval, long long _end_time, void (*Timer_proc)(), void (*Timer_time_out_proc)(), bool _begin_first)
{
	/*д���������Ϊ���������Ա�����Բ�д�����Ϸ�������*/
	this->_interval = _interval;//���ö�ʱ�����ʱ��
	this->_end_time = _end_time;//���ö�ʱ���ر�ʱ��
	this->Timer_proc = Timer_proc;//���ö�ʱ�������������ָ��
	this->Timer_time_out_proc = Timer_time_out_proc;//���ö�ʱ����ʱ�˳���������ָ��
	if (_begin_first) this->_next = 0; else this->_next = _interval;//�������ִ�м��������������һ��ʱ���Ϊ���ʱ��
}

/// <summary>
/// �˺�����HighPrecisionTimerClass���һ�����캯����Ĭ�ϲ��رն�ʱ��
/// </summary>
/// <param name="_interval">�˲���Ϊ��ʱ���ļ��ʱ������λΪ΢�루��s�����뾡��д�����ֵ</param>
/// <param name="Timer_proc">�˲���Ϊ��ʱ�������������ָ�룬ע����ǣ��˺������β��뷵��ֵ</param>
/// <param name="Timer_time_out_proc">�˲���Ϊ��ʱ����ʱ�˳���������ָ�룬ע����ǣ��˺������β��뷵��ֵ������ΪNULL</param>
/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
HighPrecisionTimerClass::HighPrecisionTimerClass(unsigned long long _interval, void (*Timer_proc)(),void(*Timer_time_out_proc)(),bool _begin_first)
{
	this->HighPrecisionTimerClass::HighPrecisionTimerClass(_interval,-1, Timer_proc, Timer_time_out_proc, _begin_first);
}

/// <summary>
/// �˺�����HighPrecisionTimerClass���һ�����캯��������Ϊ�չ��캯��
/// </summary>
HighPrecisionTimerClass::HighPrecisionTimerClass(){}

/// <summary>
/// �˺���Ϊ������ʱ��������ʹ�ô˺���ǰ������ȷ�ϲ����������
/// </summary>
/// <returns>����ֵΪbool�ͣ�true�������ɹ���false����ʧ��</returns>
/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
bool HighPrecisionTimerClass::Start_high_precision_timer(bool _begin_first)
{
	if (_begin_first) this->_next = 0; else this->_next = this->_interval;//�������ִ�м��������������һ��ʱ���Ϊ���ʱ��

	/*��ȡ��ʼ�����ܼ�����Ƶ��*/
	LARGE_INTEGER _bim;
	if (!QueryPerformanceFrequency(&_bim)) return false;//�������0ֵ����ʾ�������֧�ָ�Ӳ��
	this->_init = _bim.QuadPart / 1000000;

	/*��ʱ���߳�����*/
	this->__is_start_timer = true;//״̬��־λ��Ϊtrue
	this->_high_precision_timer_thread = CreateThread(NULL, 0,HighPrecisionTimerClass::High_precision_timer_thread_proc, this, 0, NULL);//������������ʱ���̣߳���������ָ�봫��
	if (this->_high_precision_timer_thread == NULL) return false;//�жϽ��̴����Ƿ�ɹ�

	/*��ȡ��ʱ���߳�����*/
	Get__high_precision_timer_mask();

	/*�󶨶�ʱ���̵߳���һ������*/
	return Bind_threads_to_core();
}

/// <summary>
/// �˺���Ϊ�رն�ʱ��������ʹ�ô˺���ǰ�����ȶ�ʱ���ѿ���
/// </summary>
/// <returns>����ֵΪbool�ͣ�true����رճɹ���false����ʧ��</returns>
bool HighPrecisionTimerClass::Stop_high_precision_timer()
{
	/*����ʱ���뵥һ���������*/
	this->Untie_threads_to_core();

	/*�رն�ʱ���߳�*/
	this->__is_start_timer = false;//ͨ������־λ����Ϊfalse��ʹ���߳��ֶ�����
	return true;
}

/*
* *******************************************************************************************************
* ����HighPrecisionTimerClass���˽�з���ʵ��
*/
/// <summary>
/// �˺������ڻ�ȡ��ʱ�����߳�����
/// </summary>
void HighPrecisionTimerClass::Get__high_precision_timer_mask()
{
	/*�ֲ���������*/
	ULONG_PTR _pro_mask;					//��������
	ULONG_PTR _sys_mask;					//ϵͳ����

	/*��ȡ����������ϵͳ����*/
	GetProcessAffinityMask(GetCurrentProcess(), &_pro_mask, &_sys_mask);//ͨ���˺�����ȡ�������д˴���Ľ��̵�������ϵͳ����
	
	/*���ö�ʱ���߳�����*/
	if (!_pro_mask) _pro_mask = 1;//�����������Ϊ0������ֻ��һ���ں˿���
	/*Ѱ�����λ���ںˣ�����Ϊ��ʱ���߳�����*/
	this->_high_precision_timer_mask = 1;//��ʼ����ʱ���߳�����Ϊ1
	while (!(this->_high_precision_timer_mask & _pro_mask))//ѭ���ҵ����λ�Ľ�������
	{
		this->_high_precision_timer_mask <<= 1;
	}
}

/// <summary>
/// �˺����ǽ���ʱ���߳��뵥һ�������󶨣�ע����ǣ�ʹ�ô˺���֮ǰ�����Ȼ�ȡ��ʱ�����߳�����
/// </summary>
/// <returns>����ֵΪbool�ͣ�true����󶨳ɹ���false����ʧ��</returns>
bool HighPrecisionTimerClass::Bind_threads_to_core()
{
	/*����ʱ���߳��뵥һ��������*/
	this->_high_precision_timer_old_mask = SetThreadAffinityMask(this->_high_precision_timer_thread, this->_high_precision_timer_mask);
	if (this->_high_precision_timer_old_mask) return true; else return false;//�ж��Ƿ�󶨳ɹ�
}

/// <summary>
/// �˺����ǽ����ʱ���߳��뵥һ�������󶨣�ע����ǣ�ʹ�ô˺���֮ǰ������ȷ����ʱ���߳��Ѿ��뵥һ��������
/// </summary>
/// <returns>����ֵΪbool�ͣ�true�������󶨳ɹ���false����ʧ��</returns>
bool HighPrecisionTimerClass::Untie_threads_to_core()
{
	/*�����ʱ���߳��뵥һ��������*/
	this->_high_precision_timer_old_mask = SetThreadAffinityMask(this->_high_precision_timer_thread, this->_high_precision_timer_old_mask);
	if (this->_high_precision_timer_old_mask) return true; else return false;//�ж��Ƿ����󶨳ɹ�
}

/*
* *******************************************************************************************************
* ����HighPrecisionTimerClass��ľ�̬˽�з���ʵ��
*/
/// <summary>
/// �˺����Ƕ�ʱ���̴߳�����
/// </summary>
/// <param name="lpParamater">�����HighPrecisionTimerClass��ָ��</param>
/// <returns></returns>
DWORD WINAPI HighPrecisionTimerClass::High_precision_timer_thread_proc(LPVOID lpParamater)
{
	/*��������*/
	HighPrecisionTimerClass* _lp_high_precision_timer = (HighPrecisionTimerClass*)lpParamater;//��ȡ�����HighPrecisionTimerClass��ָ��
	LARGE_INTEGER _now;//�����洢��ǰ��ȡ��ʱ��

	/*��ʼ����ʼ��ʱʱ��*/
	QueryPerformanceCounter(&(_lp_high_precision_timer->_start));//��ȡ��ʼ��ʱʱ��

	/*�˴���ʼѭ����ȡ��ʱ��ʱ��*/
	while (1)
	{
		if (!(_lp_high_precision_timer->__is_start_timer))  return 0;//��ʱ���ֶ�ֹͣ
		QueryPerformanceCounter(&_now);//��ȡ��ǰ��ʱ��
		long long _past_time = (_now.QuadPart - _lp_high_precision_timer->_start.QuadPart)/ _lp_high_precision_timer->_init;//�������ȥ��ʱ�䣬��λ΢�루��s��
		if (_lp_high_precision_timer->_end_time != -1 && _past_time >= _lp_high_precision_timer->_end_time) //����ﵽ����ʱ�䣬�˳��߳�
		{ 
			if (_lp_high_precision_timer->Timer_time_out_proc != NULL) _lp_high_precision_timer->Timer_time_out_proc();//��ʱ����ʱ������ִ��
			_lp_high_precision_timer->Untie_threads_to_core(); 
			_lp_high_precision_timer->__is_start_timer = false;
			return 0; 
		}
		if (_past_time >= _lp_high_precision_timer->_next)//�ﵽ��һ�μ��ʱ��
		{
			_lp_high_precision_timer->Timer_proc();//ִ�м��������
			_lp_high_precision_timer->_next += _lp_high_precision_timer->_interval;//������һ�μ��ִ�е�ʱ��
		}
		/*����cpuռ�ã�����ʵ����������Խ����޸ģ�ȡ����Sleep���*/
		if (_lp_high_precision_timer->_next - _past_time > 30000)//���������һ���������30ms����Sleep����˯��20ms
			Sleep(20);
			//Sleep(min(1000, (_lp_high_precision_timer->_next - _past_time)/1000-10));//����CPUռ��
	}
}



///*���Գ��򣬴����ʱ��ע�͵�*/
///*�����õ�ȫ�ֱ���*/
//LARGE_INTEGER __tmp;
//LARGE_INTEGER __start;
//LARGE_INTEGER __end;
//int num = 0;
//int num2 = 0;
//long long accurate = 0;//׼ȷ
//long long error = 0;//���
///*�����õĵ�һ��ʱ��ִ�к���*/
//void proc()
//{
//	num++;
//	QueryPerformanceCounter(&__end);
//	std::cout << "��һ��ʱ�������ǵ�" << num << "��ִ�У���ʱ��" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart/1000000) << "us\n";
//}
///*�����õĵڶ���ʱ��ִ�к���*/
//void proc2()
//{
//	QueryPerformanceCounter(&__end);
//	long long z = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) - accurate;
//	error += z>0?z: accurate- (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000);
//	accurate = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) + 500000;
//	num2++;
//	std::cout << "�ڶ���ʱ�������ǵ�" << num2 << "��ִ�У���ʱ��" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) << "us\n";
//}
///*���Գ������*/
//int main()
//{
//	//��Ϊ���Գ�����ͬһ���ڴ�ӡ���֣������ʱ��ͬʱ����ʱ����ʹ��ӡ�������໥���ţ�˳����ң����ܿ���������ʱ������������
//	std::cout << "��ʼ\n";
//	QueryPerformanceFrequency(&__tmp);
//	QueryPerformanceCounter(&__start);
//	HighPrecisionTimerClass _high_precision_timer(166666,30000000,proc,NULL,true);//166.666ms���,30s����,��ʼִ��
//	HighPrecisionTimerClass _high_precision_timer2(500000, 30000000, proc2, NULL, true);//500.000ms���,30s����,��ʼִ��
//
//	_high_precision_timer.Start_high_precision_timer(true);//��ʼ��ʱ��1
//	_high_precision_timer2.Start_high_precision_timer(true);//��ʼ��ʱ��2
//
//	Sleep(10000);
//	_high_precision_timer.Stop_high_precision_timer();//�ֶ��رռ�ʱ��1
//
//	Sleep(30000);
//
//	std::cout << "ƽ����" << error/60 << "us\n";
//	system("pause");
//
//	return 0;
//}