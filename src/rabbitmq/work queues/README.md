### 学习目标

- 创建持久化队列
- 多消费者模型下，轮询路由消息
- 手动确认删除消息

### 测试方法

```
### 先运行生产者
# ./producer
# rabbitmqctl list_queues
Listing queues ...
task_queue	4
...done.
### 再运行消费者
# ./consumer
```

### 学习总结

消息队列查询

```
# rabbitmqctl list_queues
```

未确认消息查询

```
# rabbitmqctl list_queues hello messages_ready messages_unacknowledged
```