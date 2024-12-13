## 环境搭建

### 1. 安装 RabbitMQ 服务

请参考官方文档进行安装：[RabbitMQ 安装指南](https://www.rabbitmq.com/docs/install-debian)

### 2. 安装 Oslo.Messaging

使用 pip 安装所需依赖：

```bash
pip install oslo.messaging oslo.config
```

### 3. 创建用户

```bash
sudo rabbitmqctl add_user test test
sudo rabbitmqctl set_permissions -p / test '.*' '.*' '.*'
```

## 学习目标

- **RPC 通讯：** 使用 `oslo.messaging` 实现远程过程调用。
- **通知机制：** 使用 `oslo.messaging` 实现消息通知。
- **服务端方法暴露：** 掌握服务端 `Endpoints` 的配置与使用。

## 参考资料

- [Oslo Messaging Wiki](https://wiki.openstack.org/wiki/Oslo/Messaging)
- [Red Hat 配置指南](https://docs.redhat.com/en/documentation/red_hat_openstack_platform/8/html/configuration_reference_guide/configuring-rpc#configuration-rabbitmq)