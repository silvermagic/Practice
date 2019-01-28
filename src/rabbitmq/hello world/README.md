### 学习目标

- 发送、接收消息
- 队列创建

### 测试方法

```
### 先运行消费者
# ./consumer
### 再运行生产者
# ./producer
```

### 学习总结

消息发送

> 创建连接 -> 创建子频道 -> 发送消息 -> 关闭子频道 -> 关闭连接 -> 销毁连接

消息接收

> 创建连接 -> 创建子频道 -> [创建队列] -> [绑定队列] -> 接收消息 -> 关闭子频道 -> 关闭连接 -> 销毁连接