## 1. >>：左移位， 
```c
X = 8;
X >>= 3;
// out结果是：0001
```

## 2. #include<time.h>
```c
time_t time(time_t *timer)	// 返回从TC1970-01-01 0:0:0到现在的秒数

// 用time()函数结合其他函数（localtime、gmtime、asctiome、ctime）可获得系统标准时间

// timespec有两个成员，一个是秒，一个是纳秒，最高精度是纳秒。
struct timespec{
	time_t tv_sec;	// seconds
	time_t tv_nsec;	// nanosecnds
};

// int clock_gettime(clokid_t, struct timespec *)获取特定时钟的时间，常用如下：
CLOCK_REALTIME		// 系统当前时间，从1970.1.1算起，可被设置
CLOCK_MONOTONIC		// 系统运行时间，从系统启动时开始计时，系统休眠时不再计时（NTP与硬件时钟有问题时会影响其频率），不可被设置
CLOCK_PROCESS_CPUTIME_ID	// 本进程启动到此刻使用CPU的时间，当使用sleep等函数时不再计时
CLOCK_THREAD_CPUTIME_ID		// 本线程启动到此可使用CPU的时间，但是用sleep等函数时不再计时


// struct timeval
struct timeval{
	time_t tv_sec;	// seconds
	long tv_usec;	// microseconds
};

struct timezone{
	int tz_minuteswest;	// miniutes west of Greewich
	int tz_dsttime;		// type of DST correction
};

// struct timeval有两个成员——秒/微秒，最高精度微秒
int gettimeofday(struct timecal *tv, struct timezone *tz)	// 获取系统时间
```

## 3. uint8\_t unit16\_t uint32\_t unit64\_t
1) 这些类型的来源：这些数据类型中都带有_t, _t 表示这些数据类型是通过typedef定义的，而不是新的数据类型。也就是说，它们其实是我们已知的类型的别名。

2) 使用这些类型的原因：方便代码的维护。
> 在涉及到跨平台时，不同的平台会有不同的字长，所以利用预编译和typedef可以方便的维护代码。

## 4. \_\_attribute\_\_的使用 
更详细的内容：https://blog.csdn.net/huasir_hit/article/details/77531942

GNU C的一大特色就是\_\_attribute\_\_ 机制。\_\_attribute\_\_可以设置函数属性（Function Attribute）、变量属性（Variable Attribute）和**类型属性（Type Attribute）。**

\_\_attribute\_\_书写特征是：\_\_attribute\_\_前后都有两个下划线，后面会紧跟一对原括弧，括弧里面是相应的\_\_attribute\_\_参数。

\_\_attribute\_\_语法格式为：\_\_attribute\_\_ ((attribute-list))
> 注意 \_\_attribute\_\_和后边的()之间有一个空格。

位置约束：放于声明的尾部";"之前。
> RISC中声明vector例子：typedef int v4si \_\_attribute\_\_ ((vector_size(16)));


