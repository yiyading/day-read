gcc编译过程：
> gcc -E *.c -o *.i 预处理 <br>
> <br>
> gcc -S *.i -o *.s 编译 <br>
> <br>
> gcc -c *.s -o *.o 汇编 <br>
> <br>
> gcc *.o -o * 链接 <br>
> <br>
> 合成一步 gcc *.c -o *


