### 学习目标

- 创建事件循环
- 访问事件循环
- 获取当前运行的事件循环

### 学习心得

- call_soon 接收的函数不会立即运行，而是当事件循环下一次迭代的时候运行，即出现 IO 进行切换的时候运行
- asyncio 是单线程的，因此对于一个线程来说，只会有一个事件循环
- get_event_loop 方法只在主线程上实例化一个循环并分配给线程局部变量，如果你在主线程上，那么 get_event_loop 将实例化该循环并在策略中本地保存实例线程，如果你不在主线程上，它将引发一个 RuntimeError
- 事件循环的创建：对于主线程，在外部我们会调用 get_event_loop，在协程内部我们会调用 get_running_loop；如果是子线程，那么在外部则需要 new_event_loop + set_event_loop 来实现
- loop.stop() 之后仍然可以调用 loop.run_* 方法，但 loop.close() 不行，它会直接关闭事件循环