### 学习目标

- RPC实现

### 测试方法

```
### 先运行生产者
# ./producer
### 再运行消费者
# ./consumer
```

### 学习总结

- RPC模式下，生产者同时也是消费者，消费者同时也是发送者，唯一不同的是，生产者将消息发送到RPC队列，在独占队列上监听返回消息，
消费者在RPC队列上监听消息，将消息发送给独占队列
- 使用不同的子频道进行接收和发送有助于业务逻辑的区分