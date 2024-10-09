import asyncio
import aiohttp
import time
from pprint import pprint
from aiohttp import ClientSession
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

async def fetch_status(session: ClientSession, url: str):
    timeout = aiohttp.ClientTimeout(2)
    async with session.get(url, timeout=timeout) as response:
        return response.status

@async_timed
async def main():
    async with aiohttp.ClientSession() as session:
        requests = [fetch_status(session, "http://www.baidu.com") for _ in range(100)]
        # 并发运行 100 个任务，并等待这些任务全部完成
        # 相比写 for 循环再单独 await，这种方式就简便多了
        status_codes = await asyncio.gather(*requests)
        print(len(status_codes))

loop = asyncio.get_event_loop()
loop.run_until_complete(main())
"""
协程 main 开始执行
100
协程 main 用 0.4120007 秒执行完毕
"""

@async_timed
async def main2():
    gather1 = asyncio.gather(*[asyncio.sleep(second, result=f"我睡了 {second} 秒")
                               for second in (5, 3, 4)])
    gather2 = asyncio.gather(*[asyncio.sleep(second, result=f"我睡了 {second} 秒")
                               for second in (3, 3, 3)])
    results = await asyncio.gather(gather1, gather2, asyncio.sleep(6, "我睡了 6 秒"))
    pprint(results)

loop = asyncio.get_event_loop()
loop.run_until_complete(main2())
"""
协程 main 开始执行
[['我睡了 5 秒', '我睡了 3 秒', '我睡了 4 秒'],
 ['我睡了 3 秒', '我睡了 3 秒', '我睡了 3 秒'],
 '我睡了 6 秒']
协程 main 用 6.0184796 秒执行完毕
"""

async def normal_running():
    await asyncio.sleep(3)
    return "正常运行"

async def raise_error():
    raise ValueError("出错啦")

async def main3():
    # return_exceptions=True可以阻止任务向上抛出异常
    results = await asyncio.gather(normal_running(), raise_error(),
                                   return_exceptions=True)
    print(results)

loop = asyncio.get_event_loop()
loop.run_until_complete(main3())