1. 将原来的源文件备份
```
sudo mv /etc/apt/source.list /etc/apt/source.list.backup
```

2. 创建新的源文件，并更换为阿里源
```
sudo vi /etc/apt/source.list

// 注意，如果是从win向VMware中paste，需要开启VMware Tools工具

deb http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse
```

3. 更新软件列表和软件
```
// 一路“是”即可，因为更换了源，所以速度很快
sudo apt-get update
sudo apt-get upgrade
```
