### 学习目标

- 路由关键字使用

### 测试方法

```
### 先运行消费者
# ./consumer
### 再运行生产者（下面的指令执行快点，消费者有超时时间设定，超时后队列会消失，再发送消息就丢失了）
# ./producer
```