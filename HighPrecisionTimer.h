/*
������ �� �����
����ʱ�� �� 2023/3/10
����˵�� �� һ�ָ߾��ȵĶ�ʱ�������� QueryPerformanceCounter ��APIʵ��΢�루��s�����𣨾������ԣ�����ȶ��ڼ���΢�룩�Ķ�ʱ������˶��CPU������²�׼ȷ������
ʵ���ļ� �� HighPrecisionTimer.cpp
*/

/*
* ******************************************************************************************************
* ����������Ҫ��ͷ�ļ�
*/
#pragma once
#include <iostream>
#include <windows.h>//win32 APIͷ�ļ�

/*
* ******************************************************************************************************
* ������HighPrecisionTimerClass��Ķ���
*/
class HighPrecisionTimerClass {
public:											//���������뷽��
	/*��������*/
	bool __is_start_timer = false;				//��ʱ��״̬��־λ
	long long _init;							//�˱������ڱ��浱ǰ���ܼ�����Ƶ�ʣ���΢�����Ϊ��λ��
	LARGE_INTEGER _start;						//�˱������ڱ��濪ʼ��ʱ��ʱ��
	long long _next;							//�˱������ڱ�����һ�μ����ʱ��
	unsigned long long _interval;				//�˱������ڱ��涨ʱ�����ʱ�䣬��λ΢�루��s���������뾡������Ϊ�����Сʱ���������ĵ���Խ�࣬CPUռ�б�󣬶�ʱҲ��׼
	long long _end_time = -1;					//�˱������ڱ��������ʱ��ʱ�䣬��λ΢�루��s��������Ҫһֱ������ʱʱ������Ϊ-1
	void (*Timer_proc)() = NULL;				//�˱�����һ��ʱ�����������ָ��
	void (*Timer_time_out_proc)() = NULL;		//�˱����Ƕ�ʱ����ʱ�˳���������ָ��

	/*���з���*/
	/// <summary>
	/// �˺�����HighPrecisionTimerClass���һ�����캯��
	/// </summary>
	/// <param name="_interval">�˲���Ϊ��ʱ���ļ��ʱ������λΪ΢�루��s�����뾡��д�����ֵ</param>
	/// <param name="_end_time">�˲���Ϊ��ʱ���Ĺر�ʱ�䣬��λΪ΢�루��s��������벻�رգ���д��-1�����ฺ������д��</param>
	/// <param name="Timer_proc">�˲���Ϊ��ʱ�������������ָ�룬ע����ǣ��˺������β��뷵��ֵ</param>
	/// <param name="Timer_time_out_proc">�˲���Ϊ��ʱ����ʱ�˳���������ָ�룬ע����ǣ��˺������β��뷵��ֵ������ΪNULL</param>
	/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
	HighPrecisionTimerClass(unsigned long long _interval, long long _end_time, void (*Timer_proc)(), void(*Timer_time_out_proc)(), bool _begin_first);
	/// <summary>
	/// �˺�����HighPrecisionTimerClass���һ�����캯����Ĭ�ϲ��رն�ʱ��
	/// </summary>
	/// <param name="_interval">�˲���Ϊ��ʱ���ļ��ʱ������λΪ΢�루��s�����뾡��д�����ֵ</param>
	/// <param name="Timer_proc">�˲���Ϊ��ʱ�������������ָ�룬ע����ǣ��˺������β��뷵��ֵ</param>
	/// <param name="Timer_time_out_proc">�˲���Ϊ��ʱ����ʱ�˳���������ָ�룬ע����ǣ��˺������β��뷵��ֵ������ΪNULL</param>
	/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
	HighPrecisionTimerClass(unsigned long long _interval, void (*Timer_proc)(), void(*Timer_time_out_proc)(), bool _begin_first);
	/// <summary>
	/// �˺�����HighPrecisionTimerClass���һ�����캯��������Ϊ�չ��캯��
	/// </summary>
	HighPrecisionTimerClass();
	/// <summary>
	/// �˺���Ϊ������ʱ��������ʹ�ô˺���ǰ������ȷ�ϲ����������
	/// </summary>
	/// <returns>����ֵΪbool�ͣ�true�������ɹ���false����ʧ��</returns>
	/// <param name="_begin_first">�˲����Ƕ��忪ʼ��ʱʱ�Ƿ���ִ�м��������</param>
	bool Start_high_precision_timer(bool _begin_first);
	/// <summary>
	/// �˺���Ϊ�رն�ʱ��������ʹ�ô˺���ǰ�����ȶ�ʱ���ѿ���
	/// </summary>
	/// <returns>����ֵΪbool�ͣ�true����رճɹ���false����ʧ��</returns>
	bool Stop_high_precision_timer();

private:										//˽�������뷽��
	/*˽������*/
	HANDLE _high_precision_timer_thread;		//�˱���Ϊ��ʱ���߳̾��
	ULONG_PTR _high_precision_timer_mask;		//�˱���Ϊ��ʱ�����еĴ�����������
	ULONG_PTR _high_precision_timer_old_mask;	//�˱���Ϊ��ʼʱ��ʱ�����еĴ�����������

	/*˽�з���*/
	/// <summary>
	/// �˺������ڻ�ȡ��ʱ�����߳�����
	/// </summary>
	void Get__high_precision_timer_mask();
	/// <summary>
	/// �˺����ǽ���ʱ���߳��뵥һ�������󶨣�ע����ǣ�ʹ�ô˺���֮ǰ�����Ȼ�ȡ��ʱ�����߳�����
	/// </summary>
	/// <returns>����ֵΪbool�ͣ�true����󶨳ɹ���false����ʧ��</returns>
	bool Bind_threads_to_core();
	/// <summary>
	/// �˺����ǽ����ʱ���߳��뵥һ�������󶨣�ע����ǣ�ʹ�ô˺���֮ǰ������ȷ����ʱ���߳��Ѿ��뵥һ��������
	/// </summary>
	/// <returns>����ֵΪbool�ͣ�true�������󶨳ɹ���false����ʧ��</returns>
	bool Untie_threads_to_core();

	/*˽�о�̬����*/
	/// <summary>
	/// �˺����Ƕ�ʱ���̴߳�����
	/// </summary>
	/// <param name="lpParamater">�����HighPrecisionTimerClass��ָ��</param>
	/// <returns></returns>
	static DWORD WINAPI High_precision_timer_thread_proc(LPVOID lpParamater);
};