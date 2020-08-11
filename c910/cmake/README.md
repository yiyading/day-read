# 一、CMakeLists.txt文件

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
# 明确指定包含哪些源文件
add_library(demo demo.c demo1.c demo2.c)

# 搜索所有的 .c 文件
aux_source_directory(. SRC_LIST)	# 将当前目录下所有 .c 文件放在SRC_LIST变量中
add_library(demo ${SRC_LIST})		# 编译，宏定义替换

# 还可以自定义搜索规则
```

7. 查找指定库文件
```cmake
find_library(VAR name path)	# 查找指定的预编译库，并将它的路径存储在答复变量中

# 添加依赖库
# 
find_package(<package> [][]...)	


# 类似的还有find_file()、find_path()、find_program()、find_package()。
```

8. 设置变量
```cmake
# set直接设置变量的值
set(SRC_LIST main.c main2.c)
add_executable(demo ${SRC_LIST})

# set追加设置变量值
# list追加或者删除变量值
set(SRC_LIST main.c)
set(SRC_LIST ${SRC_LIST} add.c)
list(APPEND SRC_LIST add1.c)
list(REMOVE_ITEM SRC_LIST main.c)
```

9. include类
```cmake
include_directories()	# 目的：编译时加到包含目录
			# 增加指定目录给指定编译器去搜索包含文件
```
# 二、常用变量
1. 预定义变量
```cmake
CMAKE_CURRENT_SOURCE_DIR	# 当前Cmake正在处理的源目录的完整路径
CMAKE_CURRENT_BINARY_DIR	# 当前Cmake正在处理的二进制目录的完整路径

```
