riscv64-unknown-linux-gnu-gcc该工具针对于riscv64架构的编译器，使用的C运行库为linux中的**标准glibc**。

## 1. RISC-V数据模型，-mabi=ilp32, ilp32f, ilp32d, lp64, lp64f, lp64d

**选项：-mabi**

RISC-V GCC通过-mabi选项指定数据模型和浮点参数传递规则。

有效的选项值包括ilp32、ilp32f、ilp32d、lp64、lp64f 和 lp64d。前半部分指定数据模型，后半部分指定浮点参数传递规则。
> i指int，l指long，p指pointer即指针，32/64指前面给出的类型是32/64位的；f指float，指float型浮点数参数通过浮点数寄存器传递；d指double，指float型和double型浮点数参数通过浮点数寄存器传递。

数据模型：

![c910-1](img/1.png)

浮点参数传递规则：

![c910-2](img/2.png)


RISC_V GCC 使用手册对-mabi的说明：
```
地址：https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html

-mabi=ABI-string
    指定整数和浮点调用约定。ABI字符串包含两部分：整数类型的大小和用于浮点类型的寄存器。
    例如'-march = rv64ifd -mabi = lp64d'意思是'long'，并且指针是64位的（隐式定义了'整型'为32位），并且最大64位宽的浮点值在F寄存器中传递。
```
## 2. 浮点扩展指令
浮点参数传递规则只跟-mabi选项有关，和-march选项没有直接关系，但是部分-mabi选项需要浮点寄存器，浮点寄存器是通过浮点扩展指令引入的，这就需要在-march选项中指定浮点扩展。
