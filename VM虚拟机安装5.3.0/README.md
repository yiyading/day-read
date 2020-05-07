# 使用工具
VMware Workstation：虚拟机，百度
ubuntu-18.04.4-desktop-amd64：Ubuntu的iso，文件夹下
5.3.0kernel：文件夹下

# 配置虚拟机
打开安装好的VMware，创建一个新的虚拟机，选择Ubuntu64位，64位和iso相匹配。

不要装载镜像文件，只创建一个空的机器。

打开创建好的虚拟机，装载ISO镜像文件，顺序安装即可。
> 注意：选择minimal安装，否则会下载文件特别慢，安装系统后，更换国内的yum源在安装软件

打开虚拟机，安装VMware Tools工具，目的是为了在win和Ubuntu之间复制文件，只需在虚拟机的菜单来安装即可，解压之后安装

通过共享将内核压缩包传入Ubuntu，解压后进入Linux，编译安装升级GRUB
