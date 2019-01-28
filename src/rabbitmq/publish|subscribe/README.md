### 学习目标

- 扇形消息路由模式使用
- 独占队列创建

### 测试方法

```
### 先运行消费者
# ./consumer
### 再运行生产者（下面的指令执行快点，消费者有超时时间设定，超时后队列会消失，再发送消息就丢失了）
# rabbitmqctl list_bindings
Listing bindings ...
	exchange	amq.gen-h1TJ98rUO65CL-DIiY6tIw	queue	amq.gen-h1TJ98rUO65CL-DIiY6tIw	[]
	exchange	amq.gen-wUpGCcjtlRy2sF1gWu2Vhg	queue	amq.gen-wUpGCcjtlRy2sF1gWu2Vhg	[]
amq.fanout	exchange	amq.gen-h1TJ98rUO65CL-DIiY6tIw	queue		[]
amq.fanout	exchange	amq.gen-wUpGCcjtlRy2sF1gWu2Vhg	queue		[]
...done.
# ./producer
# rabbitmqctl list_bindings
# rabbitmqctl list_queues
```

### 学习总结

消息路由器查询

```
### amq.*开头的是默认消息路由器
# rabbitmqctl list_exchanges
```

消息队列绑定查询

```
# rabbitmqctl list_bindings
```