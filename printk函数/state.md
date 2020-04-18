printk()函数是在开发驱动过程中经常用到的一个函数，作用是在串口上输出内核信息。<br>

在printk()函数中使用日志级别，目的是允许编程人员在编程中自定义的进行信息的输出，更加容易掌握系统当前情况。在程序调试中有很重要的作用。<br>
* 日志级别和控制台日志级别是一个意思。<br>
```c
printk(日志级别 "消息文本");	// 这里的日志级别是对文本信息在输出范围上的指定
```

日志级别一共有8个级别（在linux/include/linux/kernel.h）中：
> #define KERN_EMERG   "<0>"   // 紧急事件消息，系统崩溃之前提示，表示系统不可用<br>
> #define KERN_ALERT   "<1>"   // 报告消息，表示必须立即采取措施<br>
> #define KERN_CRIT    "<2>"   // 临界条件，通常涉及严重的硬件或软件操作失败<br>
> #define KERN_ERR     "<3>"   // 错误条件，驱动程序常用KERN_ERR来报告硬件的错误<br>
> #define KERN_WARNING "<4>"   // 警告条件，对可能出现问题的情况进行警告<br>
> #define KERN_NOTICE  "<5>"   // 正常但又重要的条件，用于提醒。常用于与安全相关的消息<br>
> #define KERN_INFO    "<6>"   // 提示信息，如驱动程序启动时，打印硬件信息<br>
> #define KERN_DEBUG   "<7>"   // 调试级别的消息<br>

没有指定日志级别的printk语句默认采用的级别是DEFAULT_MESSAGE_LOGLEVEL（这个默认级别一般为<4>,即与KERN_WARNING在一个级别上），其定义在linux26/kernel/printk.c中可以找到。<br>

举个栗子：<br>
```c
printk(KERN_INFO "伊亚玎\n"); //这里可以使用数字代替KERN_INFO，即printk(<6> "伊亚玎\n"); 
```
在这个格式的定义中，日志级别和信息文本之间不能够使用逗号隔开，因为系统在进行编译的时候，将日志级别转换成字符串于后面的文本信息进行连接。<br>

在对系统输出进行控制时，主要讨论控制台和伪终端的输出情况，以及系统日志等。<br>
下面是控制台日志级别的简要介绍<br>
> #define MINIMUM_CONSOLE_LOGLEVEL　 1　 　/*可以使用的最小日志级别*/<br>
> #define DEFAULT_CONSOLE_LOGLEVEL 　7     /*比KERN_DEBUG 更重要的消息都被打印*/<br>
> int console_printk[4] = {<br>
>     DEFAULT_CONSOLE_LOGLEVEL,   /*控制台日志级别，优先级高于该值的消息将在控制台显示*/<br>
>     DEFAULT_MESSAGE_LOGLEVEL,   /*默认消息日志级别，printk没定义优先级时，打印这个优先级以上的消息*/<br>
>     MINIMUM_CONSOLE_LOGLEVEL,   /*最小控制台日志级别，控制台日志级别被设置的最小值（最高优先级）*/<br>
>     DEFAULT_CONSOLE_LOGLEVEL,   /* 默认的控制台日志级别*/<br>
> }<br>
> <br>
> 在进行查看的时候，可以使用命令 cat /proc/sys/kernel/printk来查看这四个值。<br>

可以通过修改文件/proc/sys/kernel/printk中的第一个值来更改当前的控制台日志级别。在下面的模块函数中控制台所使用的日志级别均为KERN_WARNING级别，当日志级别高于console_loglevel（控制台日志级别）时，消息才能在控制台显示出来。<br>

举个栗子：<br>
```c
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int book_init(void){
        printk(KERN_EMERG "EMERG/n");
        printk(KERN_ALERT "ALERT/n");
        printk(KERN_CRIT " CRIT/n");
        printk(KERN_ERR " ERR/n");
        printk(KERN_WARNING "WARNING/n");
        printk(KERN_NOTICE "NOTICE/n");
        printk(KERN_INFO "INFO/n");
        printk(KERN_DEBUG "DEBUG/n");
        return 0;
}

static void book_exit(void){
        printk(KERN_ALERT "Book module exit/n");
}

module_init(book_init);
module_exit(book_exit);
```

控制台（这里指的是虚拟终端 Ctrl+Alt+(F1~F6)）加载模块以后，控制台给出的信息为6~9行中要求输出的信息，我们在伪终端（如果对伪终端不是很清楚可以看相关的内容）上运行命令tail -n 10 /var/log/messages查看日志文件刚才得到的运行记录可以发现messages中的值为KERN_WARNING级别之后所要求输出到信息值。值。而如果我们在文件syslog和kern-log中查看系统日志文件，一般情况下可以得到所有的输出信息。<br>
