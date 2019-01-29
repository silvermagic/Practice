### 环境搭建

软件安装

```
# yum install -y automake autoconf
```

### 测试方法

```
# ./autogen.sh
# make
```

### 学习目标

- 如何使用automake来构建项目

### 学习心得

- 使用autoscan生成configure.ac
- 除了Makefile.am需要自己编写之外，其他文件都可以参考其他项目
- 使用make maintainer-clean命令可以将环境恢复到最初的构建状态
