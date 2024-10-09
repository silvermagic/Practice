### 学习目标

- 协程的定义
- 协程的运行
- 协程的取消

### 学习心得

- 协程被包装成 Task 对象的时候就已经开始运行了，你可以让主程序继续往下执行，也可以使用 await 让主程序阻塞等它执行完毕
- 不要直接 await 一个协程，而是将协程包装成任务来让它运行(不要立即 await，这样会退化成 await 协程，从而导致主程序阻塞)，而是当你的代码逻辑依赖某个任务的执行结果时，再对该任务执行 await
- CancelledError 只能从 await 语句抛出，任务被取消，只要不 await 这个任务，而是执行其他 Python 代码，包括 await 其他协程/任务都不会抛出 CancelledError
- 如果任务在取消的时候已经运行完毕了，那么 await 的时候就不会抛 CancelledError，所以对一个已完成的任务调用 cancel 方法，没有任何影响
- future 必须在调用 set_result（设置结果）之后才能调用 result（获取结果），并且 set_result 只能调用一次，但 result 可以调用多次
- future 可以使用 await 来阻塞主程序直到 future 的值被设置，或者使用 add_done_callback 的方式，让 future 的值被设置时自动触发回调处理，前者就是现代风格的异步代码(像同步代码一样)，后者就是传统回调风格的异步代码

> 协程和任务的陷阱

虽然通过将协程包装成任务来并发执行，可以获得一些性能改进，但有些场景下却得不到提升：

- 第一个场景：代码是 CPU 密集
- 第二个场景：代码虽然是 IO 密集，但 IO 是阻塞 IO，而不是非阻塞 IO

解决办法就是使用线程池或进程池

> Coroutine、Future和Task的关系

```
                     +----------------+
                     |   Awaitable    |  <-- Interface/Protocol
                     +----------------+
                              ^
                              |
            +-----------------+-----------------+
            |                                   |
+------------------------+          +------------------------+
|       Coroutine        |          |         Future         |
| (Implements Awaitable) |          | (Implements Awaitable) |
+------------------------+          +------------------------+
                                                ^
                                                |
                                    +------------------------+
                                    |          Task          |
                                    |  (Subclass of Future)  |
                                    +------------------------+
```

- Awaitable: 协议，表示对象可以被 await，Coroutine 和 Future 都实现了这个接口
- Coroutine: 协程是 Awaitable 的实现，表示可以被 await 的异步函数
- Future: Future 是 Awaitable 的实现，表示一个将来会完成并返回结果的对象
- Task: 任务是 Future 的子类，可以被认为是一个协程和一个 future 的组合。创建一个任务时，相当于创建了一个 future和一个运行着的协程，然后当协程运行得到结果或出现异常时，我们将设置 future 的结果或异常