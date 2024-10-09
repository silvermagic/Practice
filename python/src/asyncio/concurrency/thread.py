import time
from concurrent.futures import ThreadPoolExecutor
import requests
import asyncio

def get_status_code(url: str) -> int:
    response = requests.get(url)
    return response.status_code

async def pool():
    loop = asyncio.get_running_loop()
    with ThreadPoolExecutor() as pool:
        tasks = [loop.run_in_executor(pool, get_status_code, "https://www.baidu.com")
                 for _ in range(100)]
        results = await asyncio.gather(*tasks)
        print(all(map(lambda x: x == 200, results)))

async def none():
    loop = asyncio.get_running_loop()
    tasks = [loop.run_in_executor(None, get_status_code, "https://www.baidu.com")
             for _ in range(100)]
    results = await asyncio.gather(*tasks)
    print(all(map(lambda x: x == 200, results)))

async def to_thread():
    tasks = [asyncio.to_thread(get_status_code, "https://www.baidu.com")
             for _ in range(100)]
    results = await asyncio.gather(*tasks)
    print(all(map(lambda x: x == 200, results)))

start = time.perf_counter()
asyncio.run(pool())
end = time.perf_counter()
print(f"自定义线程池：100 个请求在 {end - start} 秒内完成")

start = time.perf_counter()
asyncio.run(none())
end = time.perf_counter()
print(f"默认线程池：100 个请求在 {end - start} 秒内完成")

start = time.perf_counter()
asyncio.run(to_thread())
end = time.perf_counter()
print(f"100 个请求在 {end - start} 秒内完成")