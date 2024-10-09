import asyncio

async def delay(seconds):
    await asyncio.sleep(seconds)
    print(f"我睡了 {seconds} 秒")

async def main():
    tasks = [asyncio.create_task(delay(seconds)) for seconds in (3, 2, 4)]
    # 和 gather 一样，默认会等待所有任务都完成
    done, pending = await asyncio.wait(tasks)
    print(f"已完成的任务数: {len(done)}")
    print(f"未完成的任务数: {len(pending)}")

loop = asyncio.get_event_loop()
loop.run_until_complete(main())
"""
我睡了 2 秒
我睡了 3 秒
我睡了 4 秒
已完成的任务数: 3
未完成的任务数: 0
"""

async def delay2(seconds):
    await asyncio.sleep(seconds)
    if seconds == 3:
        raise ValueError("我出错了(second is 3)")
    print(f"我睡了 {seconds} 秒")

### 有一个任务出错就结束
async def main2():
    tasks = [asyncio.create_task(delay2(seconds), name=f"睡了 {seconds} 秒的任务")
             for seconds in range(1, 6)]
    done, pending = await asyncio.wait(tasks, return_when=asyncio.FIRST_EXCEPTION)
    print(f"已完成的任务数: {len(done)}")
    print(f"未完成的任务数: {len(pending)}")

    print("都有哪些任务完成了？")
    for t in done:
        print("    " + t.get_name())

    print("还有哪些任务没完成？")
    for t in pending:
        print("    " + t.get_name())
        t.cancel()
    
    # 阻塞 3 秒
    await asyncio.sleep(3)

loop = asyncio.get_event_loop()
loop.run_until_complete(main2())
"""
我睡了 1 秒
我睡了 2 秒
已完成的任务数: 3
未完成的任务数: 2
都有哪些任务完成了？
    睡了 3 秒的任务
    睡了 2 秒的任务
    睡了 1 秒的任务
还有哪些任务没完成？
    睡了 5 秒的任务
    睡了 4 秒的任务
"""

### 当任务完成时处理结果，有点类似as_completed
async def main3():
    tasks = [asyncio.create_task(delay(seconds))
             for seconds in range(1, 6)]
    done, pending = await asyncio.wait(tasks, return_when=asyncio.FIRST_COMPLETED)
    print(f"已完成的任务数: {len(done)}")
    print(f"未完成的任务数: {len(pending)}")

loop = asyncio.get_event_loop()
loop.run_until_complete(main3())
"""
已完成的任务数: 1
未完成的任务数: 4
"""