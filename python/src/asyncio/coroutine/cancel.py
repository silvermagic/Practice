import asyncio

async def delay(seconds):
    print(f"开始休眠 {seconds} 秒")
    await asyncio.sleep(seconds)
    print(f"休眠完成")
    return seconds

async def main():
    long_task = asyncio.create_task(delay(10))
    seconds_elapsed = 0

    while not long_task.done():
        print("检测到任务尚未完成，一秒钟之后继续检测")
        await asyncio.sleep(1)
        seconds_elapsed += 1
        # 时间超过 5 秒，取消任务
        if seconds_elapsed == 5:
            long_task.cancel()
    
    try:
        # 等待 long_task 完成，显然执行到这里的时候，任务已经被取消
        # 不管是 await 一个已经取消的任务，还是 await 的时候任务被取消
        # 都会引发 asyncio.CancelledError
        await long_task
    except asyncio.CancelledError:
        print("任务被取消")

asyncio.run(main())
"""
检测到任务尚未完成，一秒钟之后继续检测
开始休眠 10 秒
检测到任务尚未完成，一秒钟之后继续检测
检测到任务尚未完成，一秒钟之后继续检测
检测到任务尚未完成，一秒钟之后继续检测
检测到任务尚未完成，一秒钟之后继续检测
检测到任务尚未完成，一秒钟之后继续检测
任务被取消
"""