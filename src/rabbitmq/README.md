### RabbitMQ简介

RabbitMQ是一个消息代理：它接受并转发消息。你可以把它想象成一个邮局：当你将邮件放进邮箱中时，您可以确定邮件员最终会将邮件送给你的收件人，RabbitMQ就好比是一个邮箱、邮局以及邮递员，只是它负责投递的"邮件"不是纸，而是二进制blob数据

### 术语

| 名称 | 说明 |
| --- | --- |
| 消息 | RabbitMQ接受、存储以及转发的"邮件" |
| 生产者 | 发送"邮件"的人 |
| 消费者 | 接收"邮件"的人 |
| 队列 | RabbitMQ的"邮箱" |

### 环境搭建

```
### 编译环境
# yum install -y librabbitmq-devel

### 测试环境
# yum install -y rabbitmq-server
# systemctl start rabbitmq-server
```

### 学习心得

rabbitmq-c实现基于单线程的事件驱动模式，所以不允许在多线程中共享AMQP连接（amqp_connection_state_t）或频道，每个线程需要创建自己的AMQP连接

### 教程简介

Hello World

> 使用RabbitMQ C库发送和接收消息

Work Queues

> 多消费者模式
