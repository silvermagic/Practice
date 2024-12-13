### 学习目标

- 服务端如何暴露Endpoints
- 客户端如何发送RPC请求
- 如何设置call的timeout时间
- 如何将cast请求改成广播模式

### 学习心得

- Transport就是与RabbitMQ的连接
- 客户端发送RPC请求时，Target(包含了交换机和主机等信息)必须和服务端对应Endpoints的Target一致，即主题、版本等都不能错
- 服务端通过往oslo_messaging.get_rpc_server方法中传入Endpoints对象来暴露RPC服务，每个Endpoints对象都可以指定自己的Target
- 服务端的方法即使有返回值，客户端如果不关心，也可以使用cast方法去调用
- 可以同时启动多个相同的服务端，即同时在多个窗口执行`python server.py`是允许的，但是每次客户端请求只会有一个服务端响应，即多次`add`调用并不是被同一个服务端处理的，而是以轮训的方式被多个服务端处理
- OpenStack RPC Call (不指定服务器)：
    - `nova-api` 调用 `nova` 交换机的 `scheduler` 主题的 `run_instance ` 方法，处理该请求的是某个 `nova-scheduler` 服务
    - `nova-compute` 调用 `nova` 交换机中 `conductor` 主题上的 `instance_update` 方法，处理该请求的是某个 `nova-conductor` 服务
- OpenStack RPC Call (指定服务器)：
    - `nova-scheduler` 选择 `foobar` 作为主机来运行一个新实例，因此它调用 `nova` 交换机中 `compute` 主题上的 `foobar` 服务器的 `run_instance` 方法，运行在 `foobar` 上的 `nova-compute` 服务负责启动该实例
    - `nova-api` 接收到终止一个运行在主机 `foobar` 上的实例的请求，因此它调用 `nova` 交换机中 `compute` 主题上的 `foobar` 服务器的 `terminate_instance` 方法，运行在 `foobar` 上的 `nova-compute` 服务处理该请求。
- OpenStack RPC 广播：
    - `nova-compute` 定期在 `nova` 交换机中 `scheduler` 主题上以广播模式调用 `update_service_capabilities` 方法，所有 `nova-scheduler` 服务都会处理该请求。
    - 在启动时，`nova-scheduler` 在 `nova` 交换机中 `compute` 主题上以广播模式调用 `publish_service_capabilities` 方法，所有 `nova-compute` 服务都会处理该请求。
