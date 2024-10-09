### 学习目标

- 并发运行任务
- 异常处理
- 等待所有任务完成
- 等待所有任务，哪个先完成，哪个先处理
- 等待所有任务，一个失败了，取消其他任务

### 学习心得

- asyncio.gather 接收一系列等待对象，允许我们在一行代码中同时运行它们。如果传入的 awaitable 对象是协程，gather 会自动将其包装成任务，以确保它们可以同时运行
- asyncio.gather 的一个特性是，不管 awaitable 对象何时完成，都保证结果按传递它们的顺序返回
- asyncio.gather 里面也可以继续接收 asyncio.gather 返回的对象，从而实现分组功能
- asyncio.gather 里面一个任务异常，不会导致其他任务被取消，并且异常会向上传播
- asyncio.as_completed 接收一系列等待对象，并返回一个 future 迭代器。然后可以迭代这个 future，等待它们中的每一个对象都完成，并且哪个先完成，哪个就被迭代，而不是像 asyncio.gather 一样等待所有任务完成才能处理
- asyncio.wait 和 asyncio.as_completed 接收的都是任务列表，而 asyncio.gather 则要求将列表打散，以多个位置参数的方式传递
- asyncio.wait 普通模式下和 asyncio.gather 一样，都是等到任务全部结束再解除等待，但是可以通过指定 return_when 选择等待策略
    - asyncio.ALL_COMPLETED：等待所有任务完成后返回
    - asyncio.FIRST_COMPLETED：有一个任务完成就返回
    - asyncio.FIRST_EXCEPTION：当有任务出现异常时返回