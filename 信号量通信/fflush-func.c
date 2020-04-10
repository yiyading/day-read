// 该程序用来测试fflush函数䣌功能
// C 库函数 int fflush(FILE *stream) 刷新流 stream 的输出缓冲区。 
#include <stdio.h>
#include <string.h>
 
int main()
{
 
   char buff[1024];
 
   memset( buff, '\0', sizeof( buff ));
 
   fprintf(stdout, "启用全缓冲\n");
   setvbuf(stdout, buff, _IOFBF, 1024);
 
   fprintf(stdout, "这里是 runoob.com\n");
   fprintf(stdout, "该输出将保存到 buff\n");
// 在使用多个输出函数连续进行多次输出时，有可能发现输出错误。因为下一个数据再上一个数据还没输出完毕，还在输出缓冲区中时，下一个printf就把另一个数据加入输出缓冲区，结果冲掉了原来的数据，出现输出错误。 在 prinf（）；后加上fflush(stdout); 强制马上输出，避免错误。
   fflush( stdout );
 
   fprintf(stdout, "这将在编程时出现\n");
   fprintf(stdout, "最后休眠五秒钟\n");
 
   sleep(5);
 
   return(0);
}
