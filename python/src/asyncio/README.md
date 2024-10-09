### Asyncio简介

`asyncio`是Python库，用于实现异步编程，通过协程和事件循环提高程序在处理IO操作时的并发性和响应性。

### 术语

| 名称 | 说明 |
| --- | --- |
| 协程 | 一种可以在长时间运行的任务中暂停并恢复的函数，使用`async`和`await`关键字定义 |
| 事件循环 | `asyncio`中用于调度和执行协程的机制，负责管理多个任务的异步执行 |

### 环境搭建

```
pip install aiohttp -i https://pypi.tuna.tsinghua.edu.cn/simple
```

### 参考文档

[asyncio 系列](https://www.cnblogs.com/traditional/tag/%E6%B7%B1%E5%BA%A6%E8%A7%A3%E5%AF%86%20asyncio/)