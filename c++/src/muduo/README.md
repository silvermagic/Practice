### 环境搭建

```
sudo apt install libboost-dev

git clone https://github.com/chenshuo/muduo.git
cd muduo
mkdir build
cd build
cmake ..
make -j$nproc
sudo make install
```

### 学习目标

学习muduo库的使用

### 教程简介

[大并发服务器架构（陈硕muduo库源码解析）](https://www.bilibili.com/video/BV1Zt411K7Gg/?spm_id_from=333.999.0.0&vd_source=32215fefb3700a6a9e99efc25ce4d4d5)
[muduo_server_learn](https://github.com/zhengchunxian-ai/muduo_server_learn)