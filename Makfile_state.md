[原文链接](https://blog.csdn.net/haoel/article/details/2890)
# 一、概述
什么是makefile？或许很多Winodws的程序员都不知道这个东西，因为那些Windows的IDE都为你做了这个工作，但我觉得要作一个优秀的和professional的程序员，makefile还是要懂。这就好像现在有这么多的HTML的编辑器，但如果你想成为一个专业人士，你还是要了解HTML的标识的含义。特别在Unix下的软件编译，你就不能不自己写makefile了，会不会写makefile，从一个侧面说明了一个人是否具备完成大型工程的能力。

因为makefile关系到了整个工程的编译规则。一个工程中的源文件不计数，其按类型、功能、模块分别放在若干个目录中，makefile定义了一系列的规则来指定，哪些文件需要先编译，哪些文件需要后编译，哪些文件需要重新编译，甚至于进行更复杂的功能操作，因为makefile就像一个Shell脚本一样，其中也可以执行操作系统的命令。

makefile带来的好处就是——“自动化编译”，一旦写好，只需要一个make命令，整个工程完全自动编译，极大的提高了软件开发的效率。make是一个命令工具，是一个解释makefile中指令的命令工具，一般来说，大多数的IDE都有这个命令，比如：Delphi的make，Visual C++的nmake，Linux下GNU的make。可见，makefile都成为了一种在工程方面的编译方法。

在这篇文档中，将以C/C++的源码作为我们基础，所以必然涉及一些关于C/C++的编译的知识，相关于这方面的内容，还请各位查看相关的编译器的文档。这里所默认的编译器是UNIX下的GCC和CC。

# 二、关于程序的编译和链接
在此，我想多说关于程序编译的一些规范和方法，一般来说，无论是C、C++、还是pas，首先要把源文件编译成**中间代码文件**,在Windows下也就是 .obj 文件，UNIX下是 .o 文件，即 Object File，这个动作叫做**编译（compile）**。然后再把大量的Object File合成执行文件，这个动作叫作**链接（link）**。

## 1.编译
编译时，编译器需要的是**语法的正确**，**函数与变量的声明的正确**。对于后者，通常是你需要告诉编译器头文件的所在位置（头文件中应该只是声明，而定义应该放在C/C++文件中），只要所有的语法正确，编译器就可以编译出中间目标文件。一般来说，每个源文件都应该对应于一个中间目标文件（O文件或是OBJ文件）。

## 2.链接
链接时，主要是**链接函数和全局变量**，所以，我们可以使用这些中间目标文件（O文件或是OBJ文件）来链接我们的应用程序。链接器并不管函数所在的源文件，只管函数的中间目标文件（Object File），在大多数时候，由于源文件太多，编译生成的中间目标文件太多，而在链接时需要明显地指出中间目标文件名，这对于编译很不方便，所以，我们要给中间目标文件打个包，在Windows下这种包叫“库文件”（Library File)，也就是 .lib 文件，在UNIX下，是Archive File，也就是 .a 文件。

**总结一下，源文件首先会生成中间目标文件，再由中间目标文件生成执行文件**。在**编译**时，编译器只检测程序语法，和函数、变量是否被声明。如果函数未被声明，编译器会给出一个**警告**,但可以生成Object File。而在**链接程序**时，链接器会在所有的Object File中找寻函数的实现，如果找不到，那到就会**报链接错误码（Linker Error）**。

# 三、Makefile介绍
make命令执行时，需要一个 Makefile 文件，以告诉make命令需要怎么样的去编译和链接程序。

首先，我们用一个示例来说明Makefile的书写规则。以便给大家一个感兴认识。这个示例来源于GNU的make使用手册，在这个示例中，我们的工程有8个C文件，和3个头文件，我们要写一个Makefile来告诉make命令如何编译和链接这几个文件。我们的规则是：<br>

* 1）如果这个工程没有编译过，那么我们的所有C文件都要编译并被链接。

* 2）如果这个工程的某几个C文件被修改，那么我们只编译被修改的C文件，并链接目标程序。

* 3）如果这个工程的头文件被改变了，那么我们需要编译引用了头文件的C文件，并链接目标程序。

只要我们的Makefile写得够好，所有的这一切，我们只用一个make命令就可以完成，make命令会自动智能地根据当前的文件修改的情况来确定哪些文件需要重编译，从而自己编译所需要的文件和链接目标程序。

## 1.Makefile的规则
Makefile的规则：<br>
```
target ...: dependence ...<br>
	commend<br>
	...<br>
	...<br>
```
target也就是一个目标文件，可以是Object File，也可以是执行文件。还可以是一个标签（Label）。<br>

dependence是生成target所依赖的文件。<br>

command也就是make需要执行的命令。（任意的Shell命令）

这是一个文件的依赖关系，也就是说，target这一个或多个的目标文件依赖dependence中的文件，其生成规则定义在command中。也就是说，dependence中如果有一个以上的文件比target文件要新的话，command所定义的命令就会被执行。这就是Makefile的规则。也是**Makefile中的核心内容**。<br>

## 2.举个栗子
正如前面所说的，如果一个工程有3个头文件，和8个C文件，我们为了完成前面所述的那三个规则，我们的Makefile应该是下面的这个样子的。
```
edit : main.o kbd.o command.o display.o insert.o search.o files.o utils.o
	cc -o edit main.o kbd.o command.o display.o insert.o search.o files.o utils.o

main.o : main.c defs.h
	cc -c main.c

kbd.o : kbd.c defs.h command.h
	cc -c kbd.c

command.o : command.c defs.h command.h
	cc -c command.c

display.o : display.c defs.h buffer.h
	cc -c display.c

insert.o : insert.c defs.h buffer.h
	cc -c insert.c

search.o : search.c defs.h buffer.h
	cc -c search.c

files.o : files.c defs.h buffer.h command.h
	cc -c files.c

utils.o : utils.c defs.h
	cc -c utils.c

clean :
	rm edit main.o kbd.o command.o display.o insert.o search.o files.o utils.o
```
我们可以把这个内容保存在文件为“Makefile”或“makefile”的文件中，然后在该目录下直接输入命令“make”就可以生成执行文件edit。执行“make clean”就可以执行Makefile文件中"clean ："所对应的删除命令。

在定义好依赖关系后，后续的那一行定义了如何生成目标文件的操作系统命令，一定要以一个Tab键作为开头。记住，make并不管命令是怎么工作的，他只管执行所定义的命令。make会比较targets文件和dependence文件的修改日期，如果dependence文件的日期要比targets文件的日期要新，或者target不存在的话，那么，make就会执行后续定义的命令。

这里要说明一点的是，clean不是一个文件，它只不过是一个动作名字，有点像C语言中的lable一样，其冒号后什么也没有，那么，make就不会自动去找文件的依赖性，也就不会自动执行其后所定义的命令。**要执行其后的命令，就要在make命令后明显得指出这个lable(clean)的名字**。这样的方法非常有用，我们可以在一个makefile中定义不用的编译或是和编译无关的命令，比如程序的打包，程序的备份，等等。

## 3.make是如何工作的
shell中输入make命令：

1、make会在当前目录下找名字叫“Makefile”或“makefile”的文件。

2、如果找到，它会找文件中的第一个目标文件（target），在上面的例子中，他会找到“edit”这个文件，并把这个文件作为最终的目标文件。

3、如果edit文件不存在，或是edit所依赖的后面的 .o 文件的文件修改时间要比edit这个文件新，那么，他就会执行后面所定义的命令来生成edit这个文件。

4、如果edit所依赖的.o文件不存在，那么make会在当前文件中找目标为.o文件的依赖性，如果找到则再根据那一个规则生成.o文件。（这有点像一个堆栈的过程）

5、当然，你的C文件和H文件是存在的啦，于是make会生成 .o 文件，然后再用 .o 文件生命make的终极任务，也就是执行文件edit了。

这就是整个make的依赖性，make会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件。在找寻的过程中，如果出现错误，比如最后被依赖的文件找不到，那么make就会直接退出，并报错。而对于所定义的命令错误，或是编译不成功，make根本不理。make只管文件的依赖性，即，如果在我找了依赖关系之后，冒号后面的文件不存在，make则停止工作。

通过上述分析，像clean这种没有被第一个目标文件直接或间接关联，那么它后面所定义的命令将不会被自动执行。不过，我们可以显示要make执行。即命令“make clean”，以此来清除所有的目标文件，以便重编译。

于是在我们编程中，如果这个工程已被编译过了，当我们修改了其中一个源文件，比如file.c，那么根据我们的依赖性，我们的目标file.o会被重编译（也就是在这个依性关系后面所定义的命令），于是file.o的文件也是最新的啦，于是file.o的文件修改时间要比edit要新，所以edit也会被重新链接了（详见edit目标文件后定义的命令）。

而如果我们改变了“command.h”，那么，kdb.o、command.o和files.o都会被重编译，并且，edit会被重链接。

## 4.makefile中使用变量
在上面的例子中，先让我们看看edit的规则：
```
edit : main.o kbd.o command.o display.o insert.o search.o files.o utils.o
	cc -o edit main.o kbd.o command.o display.o insert.o search.o files.o utils.o
```
我们可以看到[.o]文件的字符串被重复了两次，如果我们的工程需要加入一个新的[.o]文件，那么我们需要在两个地方加（应该是三个地方，还有一个地方在clean中）。当然，我们的makefile并不复杂，所以在两个地方加也不累，但如果makefile变得复杂，那么我们就有可能会忘掉一个需要加入的地方，而导致编译失败。所以，为了makefile的易维护，在makefile中我们可以使用变量。makefile的变量也就是一个字符串，**理解成C语言中的宏可能会更好**。

我们**声明一个变量**，叫objects, OBJECTS, objs, OBJS, obj, 或是 OBJ，反正不管什么啦，只要能够**表示obj文件**就行了。我们在makefile一开始就这样定义：
```
obj = main.o kbd.o command.o display.o insert.o search.o files.o utils.o
```

于是，我们就可以很方便地在我们的makefile中以"**$(obj)**"的方式来使用这个变量了，于是我们的改良版makefile就变成下面这个样子：<br>
```
obj = main.o kbd.o command.o display.o insert.o search.o files.o utils.o

edit : $(obj)
	cc -o edit $(obj)

main.o : main.c defs.h
	cc -c main.c

kbd.o : kbd.c defs.h command.h
	cc -c kbd.c

command.o : command.c defs.h command.h
	cc -c command.c

display.o : display.c defs.h buffer.h
	cc -c display.c

insert.o : insert.c defs.h buffer.h
	cc -c insert.c

search.o : search.c defs.h buffer.h
	cc -c search.c

files.o : files.c defs.h buffer.h command.h
	cc -c files.c

utils.o : utils.c defs.h
	cc -c utils.c
 
clean :
	rm edit $(obj)
```
于是如果有新的 .o 文件加入，我们只需简单地修改一下 obj 变量就可以了。<br>

## 5.让make自动推导
GNU的make很强大，它可以**自动推导文件以及文件依赖关系后面的命令**，于是我们就没必要去在每一个[\*.o]文件后都写上类似的命令，因为，我们的make会自动识别，并自己推导命令。

只要make看到一个[\*.o]文件，它就会自动的把[\*.c]文件加在依赖关系中，如果make找到一个whatever.o，那么whatever.c，就会是whatever.o的依赖文件。并且 cc -c whatever.c 也会被推导出来。于是，我们的makefile被变得更加简单。

还是上面的栗子：
```
obj = main.o kbd.o command.o display.o insert.o search.o files.o utils.o

edit : $(obj)
	cc -o edit $(obj)

main.o : main.c defs.h

kbd.o : kbd.c defs.h command.h

command.o : command.c defs.h command.h

display.o : display.c defs.h buffer.h

insert.o : insert.c defs.h buffer.h

search.o : search.c defs.h buffer.h

files.o : files.c defs.h buffer.h command.h

utils.o : utils.c defs.h

.PHONY : clean
clean :
	rm edit $(obj)
```
这种方法，也就是make的“隐晦规则”。上面文件内容中，“.PHONY”表示，clean是个伪目标文件。


> .PHONY后边的target表示的是一个伪造的target，而不是真实存在的文件target，注意Makefile的target默认是文件。<br>
> <br>
> 如果Makefile中有.PHONY，在当前目录下存在其后的文件（这个例子就是目录下存在名为clean的文件），则make clean(clean就是.PHONY后边target)命令无法执行。<br>
> <br>
> 如果Makefile中没有.PHONY，则make clean命令不受影响。<br>

## 6.另类风格的makefile
即然我们的make可以自动推导命令，那么我看到那堆[.o]和[.h]的依赖就有点不爽，那么多的重复的[.h]，能不能把其收拢起来，好吧，没有问题，这个对于make来说很容易，谁叫它提供了自动推导命令和文件的功能呢？来看看最新风格的makefile吧。
```
obj = main.o kbd.o command.o display.o insert.o search.o files.o utils.o

edit : $(obj)
	cc -o edit $(obj)

$(obj) : defs.h

kbd.o command.o files.o : command.h

display.o insert.o search.o files.o : buffer.h

.PHONY : clean
clean :
	rm edit $(obj)
```

这种风格，让我们的makefile变得很简单，但我们的**文件依赖关系就显得有点凌乱**了。鱼和熊掌不可兼得。还看你的喜好了。我是不喜欢这种风格的，一是文件的依赖关系看不清楚，二是如果文件一多，要加入几个新的.o文件，那就理不清楚了。

## 7.清空目标文件的规则
每个Makefile中都应该写一个清空目标文件（.o和执行文件）的规则，这不仅便于重编译，也很利于保持文件的清洁。一般风格为：
```
clean :
	rm edit $(obj)
```
更稳健的做法是：
```
.PHONY : clean
clean :
	-rm edit $(obj)
```
前面说过，.PHONY意思表示clean是一个“伪目标”。而在rm命令前面加了一个小减号的意思就是，也许某些文件出现问题，但不要管，继续做后面的事。当然，clean的规则不要放在文件的开头，不然，这就会变成make的默认目标，相信谁也不愿意这样。不成文的规矩是:
> “lean从来都是放在文件的最后”。

# 上面就是一个makefile的概貌，也是makefile的基础，下面还有很多makefile的相关细节。

# 四、Makefile总述
## 1.Makefile里面有什么？

Makefile里主要包含了五个东西：
> 显式规则: 显式规则说明了，如何生成一个或多的的目标文件。这是由Makefile的书写者明显指出，要生成的文件，文件的依赖文件，生成的命令。<br>
> <br>
> 隐晦规则: 由于我们的make有自动推导的功能，所以隐晦的规则可以让我们比较粗糙地简略地书写Makefile，这是由make所支持的。<br>
> <br>
> 变量定义: 在Makefile中我们要定义一系列的变量，变量一般都是字符串，这有点像c语言中的宏，当Makefile被执行时，其中的变量都会被扩展到相应的引用位置上。<br>
> <br>
> 文件指示: <br>
>> 1. 在一个Makefile中引用另一个Makefile，就像C语言中的include一样<br>
>> 2. 据某些情况指定Makefile中的有效部分，就像C语言中的预编译#if一样<br>
>> 3. 定义一个多行的命令<br>
> <br>
> 注释: Makefile中只有行注释，和UNIX的Shell脚本一样，其注释是用“#”字符，这个就像C/C++中的“//”一样。如果你要在你的Makefile中使用“#”字符，可以用反斜框进行转义，如：“\#”。<br>

最后，还值得一提的是，在Makefile中的命令，必须要以[Tab]键开始。

## 2.Makefile的文件名

默认的情况下，make命令会在当前目录下按顺序找寻文件名为“GNUmakefile”、“makefile”、“Makefile”的文件，找到了就解释这个文件。在这三个文件名中，最好使用“Makefile”这个文件名，因为，这个文件名第一个字符为大写，这样有一种显目的感觉。最好不要用“GNUmakefile”，这个文件是GNU的make识别的。有另外一些make只对全小写的“makefile”文件名敏感，但是基本上来说，大多数的make都支持“makefile”和“Makefile”这两种默认文件名。

当然，你可以使用别的文件名来书写Makefile，比如：“Make.Linux”，“Make.Solaris”，“Make.AIX”等，如果要指定特定的Makefile，你可以使用make的“-f”和“--file”参数，如：make -f Make.Linux或make --file Make.AIX。

## 3.引用其它的Makefile

在Makefile使用include关键字可以把别的Makefile包含进来，这很像C语言的#include，被包含的文件会原模原样的放在当前文件的包含位置。include的语法是：
```
include <filename>
filename可以是当前操作系统Shell的文件模式（可以保含路径和通配符）
```

在include前面可以有一些空字符，但是绝不能是[Tab]键开始。include和<filename>可以用一个或多个空格隔开。举个例子，你有这样几个Makefile：a.mk、b.mk、c.mk，还有一个文件叫foo.make，以及一个变量$(bar)，其包含了e.mk和f.mk，那么，下面的语句：
```
include foo.make *.mk $(bar)

等价于

include foo.make a.mk b.mk c.mk e.mk f.mk
```
make命令开始时，会把找寻include所指出的其它Makefile，并把其内容安置在当前的位置。就好像C/C++的#include指令一样。如果文件都没有指定绝对路径或是相对路径的话，make会在当前目录下首先寻找，如果当前目录下没有找到，那么，make还会在下面的几个目录下找：
* 1、如果make执行时，有“-I”或“--include-dir”参数，那么make就会在这个参数所指定的目录下去寻找。<br>

* 2、如果目录<prefix>/include（一般是：/usr/local/bin或/usr/include）存在的话，make也会去找。<br>

如果有文件没有找到的话，make会生成一条警告信息，但不会马上出现致命错误。它会继续载入其它的文件，一旦完成makefile的读取，make会再重试这些没有找到，或是不能读取的文件，如果还是不行，make才会出现一条致命信息。如果你想让make不理那些无法读取的文件，而继续执行，你可以在include前加一个减号“-”。如：

> -include <filename>

无论include过程中出现什么错误，都不要报错继续执行。和其它版本make兼容的相关命令是sinclude，其作用和这一个是一样的。<br>

## 4. 环境变量MAKEFILES
如果你的当前环境中定义了环境变量MAKEFILES，那么，make会把这个变量中的值做一个类似于include的动作。这个变量中的值是其它的Makefile，用空格分隔。只是，它和include不同的是，从这个环境变中引入的Makefile的“目标”不会起作用，如果环境变量中定义的文件发现错误，make也会不理。

但是在这里我还是建议不要使用这个环境变量，因为只要这个变量一被定义，那么当你使用make时，所有的Makefile都会受到它的影响，这绝不是你想看到的。在这里提这个事，只是为了告诉大家，也许有时候你的Makefile出现了怪事，那么你可以看看当前环境中有没有定义这个变量。

## 5.make的工作方式
GNU的make工作时的执行步骤入下：（想来其它的make也是类似）
> 1.读入所有的Makefile。<br>
> <br>
> 2.读入被include的其它Makefile。<br>
> <br>
> 3.初始化文件中的变量。<br>
> <br>
> 4.推导隐晦规则，并分析所有规则。<br>
> <br>
> 5.为所有的目标文件创建依赖关系链。<br>
> <br>
> 6.根据依赖关系，决定哪些目标要重新生成。<br>
> <br>
> 7.执行生成命令。<br>

1-5步为第一个阶段，6-7为第二个阶段。第一个阶段中，如果定义的变量被使用了，那么，make会把其展开在使用的位置。但make并不会完全马上展开，make使用的是拖延战术，如果变量出现在依赖关系的规则中，那么仅当这条依赖被决定要使用了，变量才会在其内部展开。<br>

当然，这个工作方式你不一定要清楚，但是知道这个方式你也会对make更为熟悉。有了这个基础，后续部分也就容易看懂了。

# 五、书写规则
该规则包含两个部分，一个是依赖关系，一个是生成目标的方法。

在Makefile中，规则的顺序是很重要的，因为，**Makefile中只应该有一个最终目标**，在Makefile中，规则的顺序是很重要的，因为，Makefile中只应该有一个最终目标，其它的目标都是被这个目标所连带出来的，所以一定要让make知道你的最终目标是什么。一般来说，定义在Makefile中的目标可能会有很多，但是第一条规则中的目标将被确立为最终的目标。如果第一条规则中的目标有很多个，那么，第一个目标会成为最终的目标。make所完成的也就是这个目标。

好了，还是让我们来看一看如何书写规则。

## 1.规则举例
```
foo.o : foo.c defs.h	# foo模块
	cc -c -g foo.c
```

看到这个例子，各位应该不是很陌生了，前面也已说过，foo.o是我们的目标，foo.c和defs.h是目标所依赖的源文件，而只有一个命令“cc -c -g foo.c”（以Tab键开头）。这个规则告诉我们两件事：
> 1.文件的依赖关系，foo.o依赖于foo.c和defs.h的文件，如果foo.c和defs.h的文件日期要比foo.o文件日期要新，或是foo.o不存在，那么依赖关系发生。<br>
> <br>
> 2.如果生成（或更新）foo.o文件。也就是那个cc命令，其说明了，如何生成foo.o这个文件。（当然foo.c文件include了defs.h文件）

## 2.规则的语法
```
target : dependence
	command
	...

或是这样：

target : dependence ; command
	command
	...	
```
targets是文件名，以空格分开，可以使用通配符。一般来说，我们的目标基本上是一个文件，但也有可能是多个文件。

command是命令行，如果其不与“target:dependence”在一行，那么，必须以[Tab键]开头，如果和dependence在一行，那么可以用分号做为分隔。（见上）

dependence是目标所依赖的文件（或依赖目标）。如果其中的某个文件要比目标文件要新，那么，目标就被认为是“过时的”，被认为是需要重生成的。这个在前面已经讲过了。

如果命令太长，你可以使用反斜框（‘/’）作为换行符。make对一行上有多少个字符没有限制。规则告诉make两件事，文件的依赖关系和如何成成目标文件。例如：
```
target : dependence
	this is one command /
	this is one-one command
```

一般来说，make会以UNIX的标准Shell，也就是/bin/sh来执行命令。

## 3.在规则中使用通配符
如果我们想定义一系列比较类似的文件，我们很自然地就想起使用通配符。make支持三个通配符：\* ? [...] 这是和Unix的B-Shell是相同的。

波浪号 **~** 字符在文件名中也有比较特殊的用途。如果是 **~/tes** ，这就表示当前用户的$HOME目录下的test目录。而“~hchen/test”则表示用户hchen的宿主目录下的test目录。（这些都是Unix下的小知识了，make也支持）而在Windows或是MS-DOS下，用户没有宿主目录，那么波浪号所指的目录则根据环境变量“HOME”而定。

通配符代替了你一系列的文件，如“\*.c”表示所以后缀为c的文件。一个需要我们注意的是，如果我们的文件名中有通配符，如：“\*”，那么可以用转义字符“\”，如“\\\*”来表示真实的“\*”字符，而不是任意长度的字符串。

举个例子：
```
clean:
	rm -f *.o
```
上边这个例子是OS Shell所支持的通配符，这是在命令中的通配符

```
print : \*.c
	lpr -p %?
	touch print
```
上边这个例子说明通配符也可以在我们的规则中，目标print依赖于所有的[\*.c]文件。其中 &? 是一个自动化变量，后边会有描述

> obj = \*.o

上面这个例子，表示通配符也可以用在变量中。Makefile中的变量其实就是C/C++中的宏，如果需要通配符在变量中展开，也就是让obj的值是所有[\*.o]文件名的集合，那么可以这样做：
> obj := $(wildcard \*.o)
这种方法由关键词 **wildcard** 指出，关于Makefile关键词，后边会有讨论

## 4.文件搜索
在一些大的工程中，有大量的源文件，我们通常的做法是把这许多的源文件分类，并存放在不同的目录中。所以，当make需要去找寻文件的依赖关系时，你可以在文件前加上路径，但最好的方法是把一个路径告诉make，让make在自动去找。

Makefile文件中的特殊变量“VPATH”就是完成这个功能的，如果没有指明这个变量，make只会在当前的目录中去找寻依赖文件和目标文件。如果定义了这个变量，那么，make就会在当前目录无法找到文件时，到指定目录去找寻文件
> VPATH = src : ../headers
> <br>
> 上边定义了 **src** 和 **../headers** 两个目录，make回按照这个顺序机型搜索
> <br>
> 目录由冒号 **:** 分隔

另一种设置文件搜索路径的方法是使用make的 **vpath** 关键字（注意，它是全小写的），这不是变量，这是一个make的**关键字**，这和上面提到的那个VPATH变量很类似，但是它更为灵活。它可以指定不同的文件在不同的搜索目录中。这是一个很灵活的功能。它的使用方法有三种：
1. vpath \<pattern\> \<directories\>：为符合模式<pattern>的文件指定搜索目录<directories>

2. vpath \<pattern\>：清除符合模式\<pattern\>的文件的搜索目录

3. vpath：清除所有已被设置好了的文件搜索目录

vapth使用方法中的\<pattern\>需要包含"%"字符。"%"的意思是匹配零或若干字符，例如，“%.h”表示所有以“.h”结尾的文件。\<pattern\>指定了要搜索的文件集，而\<directories\>则指定了\<pattern\>的文件集的搜索的目录。例如：
> vpath %.h ../headers
该语句表示，要求make在“../headers”目录下搜索所有以“.h”结尾的文件。（**如果某文件在当前目录没有找到的话**）

我们可以连续地使用vpath语句，以指定不同搜索策略。如果连续的vpath语句中出现了相同的\<pattern\>，或是被重复了的\<pattern\>，那么，make会按照vpath语句的先后顺序来执行搜索。如：
```
vpath %.c foo
vpath %   blish
vpath %.c bar

表示“.c”结尾的文件，现在“foo”目录，然后时“blish”目录，最后时“bar”目录

vpath %.c foo:bar
vpath %

表示“.c”结尾的文件，先在“foo”目录，然后时“bar”目录，最后才是“blish”目录
```

## 5.伪目标
前面一个例子我们提到了“clean”目标，这是一个“伪目标”
```
clean:
	rm *.o temp
```
正像我们前面例子中的“clean”一样，即然我们生成了许多文件编译文件，我们也应该提供一个清除它们的“目标”以备完整地重编译而用。 （以“make clean”来使用该目标）

因为，我们并不生成“clean”这个文件。**“伪目标”并不是一个文件，只是一个标签**，由于“伪目标”不是文件，所以make无法生成它的依赖关系和决定它是否要执行。我们只有通过显示地指明这个“目标”才能让其生效。当然，“伪目标”的取名不能和文件名重名，不然其就失去了“伪目标”的意义了。

当然，为了避免和文件重名的这种情况，我们可以使用一个特殊的标记“.PHONY”来显示地指明一个目标是“伪目标”，向make说明，不管是否有这个文件，这个目标就是“伪目标”。
```
.PHONY : clean
clean:
	rm *.o temp
```
使用上边这个声明，无论是否由“clean”这个文件，只要使用命令“make clean”都是运行**“clean”**这个伪目标

伪目标一般没有依赖的文件。但是，我们也可以为伪目标指定所依赖的文件。伪目标同样可以作为“默认目标”，只要将其放在第一个。一个示例就是，如果你的Makefile需要一口气生成若干个可执行文件，但你只想简单地敲一个make完事，并且，所有的目标文件都写在一个Makefile中，那么你可以使用“伪目标”这个特性：
```
all : prog1 prog2 prog3
.PHONY : all
prog1 : prog1.o utils.o
	cc -o prog1 prog1.o utils.o

prog2 : prog2.o
	cc -o prog2 prog2.o

prog3 : prog3.o sort.o utils.o
	cc -o prog3 prog3.o sort.o utils.o
```

我们知道，Makefile中的第一个目标会被作为其默认目标。我们声明了一个“all”的伪目标，其依赖于其它三个目标。由于伪目标的特性是，总是被执行的，所以其依赖的那三个目标就总是不如“all”这个目标新。所以，其它三个目标的规则总是会被执行，也就达到了我们一口气生成多个目标的目的。

“.PHONY : all”声明了“all”这个目标为“伪目标”

伪目标同样也可成为依赖。看下面的例子：
```
.PHONY: cleanall cleanobj cleandiff

cleanall : cleanobj cleandiff
	rm program

cleanobj :
	rm *.o

cleandiff :
	rm *.diff
```
“make clean”将清除所有要被清除的文件。“cleanobj”和“cleandiff”这两个伪目标有点像“子程序”的意思。我们可以输入“make cleanall”和“make cleanobj”和“make cleandiff”命令来达到清除不同种类文件的目的。
