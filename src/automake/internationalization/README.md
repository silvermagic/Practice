### 环境搭建

软件安装

```
# yum install -y automake autoconf intltool
```

### 测试方法

```
# ./autogen.sh
# make
// 一定要安装，不然找不到国际化文件
# make install
# LANG=en_US src/app
# LANG=zh_CN src/app
```

### 学习目标

- 实现程序的语言国际化

### 学习心得

- intltool是一个用于从源文件中抽取可翻译字符串的国际化工具
