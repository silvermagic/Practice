import asyncio
import time
from typing import Callable, Any
from functools import wraps

def async_timed(func: Callable) -> Callable:
    @wraps(func)
    async def wrapper(*args, **kwargs) -> Any:
        print(f"协程 {func.__name__} 开始执行")
        start = time.perf_counter()
        try:
            return await func(*args, **kwargs)
        finally:
            end = time.perf_counter()
            total = end - start
            print(f"协程 {func.__name__} 用 {total} 秒执行完毕")
    return wrapper

async def delay(seconds):
    await asyncio.sleep(seconds)
    return f"我睡了 {seconds} 秒"

@async_timed
async def main():
    # asyncio 提供的用于等待一组 awaitable 对象 API 都很智能
    # 如果检测到你传递的是协程，那么会自动包装成任务
    # 不过还是建议手动包装一下
    tasks = [asyncio.create_task(delay(seconds))
             for seconds in (3, 5, 2, 4, 6, 1)]
    for finished_task in asyncio.as_completed(tasks):
        print(await finished_task)

loop = asyncio.get_event_loop()
loop.run_until_complete(main())
"""
协程 main 开始执行
我睡了 1 秒
我睡了 2 秒
我睡了 3 秒
我睡了 4 秒
我睡了 5 秒
我睡了 6 秒
协程 main 用 6.0100407 秒执行完毕
"""

async def delay2(seconds):
    await asyncio.sleep(seconds)
    if seconds == 3:
        raise ValueError("出现异常了(seconds is 3)")
    return f"我睡了 {seconds} 秒"

@async_timed
async def main2():
    tasks = [asyncio.create_task(delay2(seconds))
             for seconds in (3, 5, 2, 4, 6, 1)]
    for finished_task in asyncio.as_completed(tasks):
        try:
            print(await finished_task)
        except Exception as e:
            print(e)

loop = asyncio.get_event_loop()
loop.run_until_complete(main2())
"""
协程 main2 开始执行
我睡了 1 秒
我睡了 2 秒
出现异常了(seconds is 3)
我睡了 4 秒
我睡了 5 秒
我睡了 6 秒
协程 main2 用 6.002056319033727 秒执行完毕
"""