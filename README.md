### 介绍

此项目为个人平时学习开源库时写的一些代码，记录下来方便以后复习

### 目录说明

| 目录名 | 作用 |
| --- | --- |
| index | 源代码目录的快速索引 |
| doc | 学习笔记 |
| src | 源代码 |

### 代码阅读

src目录下源代码按照开源库名进行划分，每个子目录下的README.md描述了如何搭建开源库的编译测试环境，以及代码功能的简单描述。

### 构建代码

默认情况下，所有代码都运行在64位CentOS 7环境下

```
### 源代码目录
# mkdir build
# cd build
### 构建debug版本
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# make
### 构建release版本
# cmake ..
# make
```


