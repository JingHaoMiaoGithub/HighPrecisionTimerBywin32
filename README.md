# ����QueryPerformanceCounter�ĸ߾��ȶ�ʱ��

**����ʱ������ԭ��WIN32 API��C++����ʵ�֡�ƽ�������185΢��**

### ������ܣ�  
����Ŀͨ��WIN32 API�е�QueryPerformanceCounter����ʵ��һ�и߾��ȵ�ѭ��
��ʱ����ʹ���߿���ͨ��������ʱ����ִ�д��룬�����ý���ѭ����ʱ��ʱ
�䣬��ʱ����ʱ��ִ�г�ʱ�������������������á� 
 
Ҳ�����ֶ�ֹͣ���ֶ�ֹͣ����ִ�г�ʱ����������  

����ʵ�飬ÿ������ƽ�����Ϊ185΢�룬�Ҳ����ۼ���  

������ʹ��**vs2019**��д


### ����ͷ�ļ���  
`#include <iostream>`  
`#include <windows.h>`

### �����ļ���  
`HighPrecisionTimer.h`

### ʵ���ļ���  
`HighPrecisionTimer.cpp`

### ���Դ���:  
��HighPrecisionTimer.cpp�ļ���
```c++
/*���Գ��򣬴����ʱ��ע�͵�*/
/*�����õ�ȫ�ֱ���*/
LARGE_INTEGER __tmp;
LARGE_INTEGER __start;
LARGE_INTEGER __end;
int num = 0;
int num2 = 0;
long long accurate = 0;//׼ȷ
long long error = 0;//���
/*�����õĵ�һ��ʱ��ִ�к���*/
void proc()
{
	num++;
	QueryPerformanceCounter(&__end);
	std::cout << "��һ��ʱ�������ǵ�" << num << "��ִ�У���ʱ��" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart/1000000) << "us\n";
}
/*�����õĵڶ���ʱ��ִ�к���*/
void proc2()
{
	QueryPerformanceCounter(&__end);
	long long z = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) - accurate;
	error += z>0?z: accurate- (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000);
	accurate = (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) + 500000;
	num2++;
	std::cout << "�ڶ���ʱ�������ǵ�" << num2 << "��ִ�У���ʱ��" << (__end.QuadPart - __start.QuadPart) / (__tmp.QuadPart / 1000000) << "us\n";
}
/*���Գ������*/
int main()
{
	//��Ϊ���Գ�����ͬһ���ڴ�ӡ���֣������ʱ��ͬʱ����ʱ����ʹ��ӡ�������໥���ţ�˳����ң����ܿ���������ʱ������������
	std::cout << "��ʼ\n";
	QueryPerformanceFrequency(&__tmp);
	QueryPerformanceCounter(&__start);
	HighPrecisionTimerClass _high_precision_timer(166666,30000000,proc,NULL,true);//166.666ms���,30s����,��ʼִ��
	HighPrecisionTimerClass _high_precision_timer2(500000, 30000000, proc2, NULL, true);//500.000ms���,30s����,��ʼִ��

	_high_precision_timer.Start_high_precision_timer(true);//��ʼ��ʱ��1
	_high_precision_timer2.Start_high_precision_timer(true);//��ʼ��ʱ��2

	Sleep(10000);
	_high_precision_timer.Stop_high_precision_timer();//�ֶ��رռ�ʱ��1

	Sleep(30000);

	std::cout << "ƽ����" << error/60 << "us\n";
	system("pause");

	return 0;
}
```

### ���Խ��:  
���Խ��-1��  
![���Խ��-1](image/��ȷ�Ȳ���1.png "��ȷ�Ȳ���-1")
���Խ��-2��  
![���Խ��-2](image/��ȷ�Ȳ���2.png "��ȷ�Ȳ���-2")
���Խ��-3��  
![���Խ��-3](image/��ȷ�Ȳ���3.png "��ȷ�Ȳ���-3")

### ����:  
����ˮƽ���ޣ����в��㣬�����½�
