#一、CMakeLists.txt文件

**cmake的文件名字必须为CMakeLists.txt**

**cmake的作用是为了生成Makefile文件**

一般我们采用 cmake 的 out-of-source 方式来构建（即**生成的中间产物和源代码分离**），这样做可以让生成的文件和源文件不会弄混，且目录结构看起来也会清晰明了。所以推荐使用这种方式，至于这个文件夹的命名并无限制，我们习惯命名为 build。

## 1. 常用命令
1. cmake_minimum\_required(VERSION 2.8.12)：该命令用于指定cmake最小版本，通常不用写，除非CMakeList.txt文件中使用一些高版本cmake特有的命令，加上一行提醒用户升级到该版本后在执行cmake。

2. project(trnn)：设置项目名，**非强制**
> 该命令会引入trnn\_BINARY\_DIR和trnn\_SOURCE\_DIR变量<br>
> 同时，cmake自动定义了两个等价的变量PROJECT\_BINARY\_DIR和PROJECT\_SOURCE\_DIR。

3. add_subdirectory(lib/trnn)：添加外部项目文件夹
> 一般情况下，我们的项目各个子项目都在一个总的项目根目录下，但有的时候，我们需要使用外部的文件夹，就需要使用该命令添加外部项目文件夹<br>

4. option()：添加一个USE_MYMATH选项，并且默认值为ON。根据USR_MYMATH变量的值来决定是否使用我们自己编写编写的MathFunctions库

5. 设置编译类型
```cmake
add_executable(hello hello.c)	# 生成可执行文件
add_library(demo STATIC util.c)	# 生成静态库
add_library(demo SHARED util.c)	# 生成动态库或者共享库

# add_library默认生成静态库，在linux中通过上述三个命令生成的文件名字：
# hello
# libdemo.a
# libdemo.so
```
6. 指定编译包含的源文件
```cmake

```
