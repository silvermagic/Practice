import asyncio

async def delay(seconds: int):
    print(f"开始休眠 {seconds} 秒")
    await asyncio.sleep(seconds)
    print(f"休眠完成")
    return seconds

async def hello_from_second():
    for i in range(10):
        await asyncio.sleep(1)
        print("你好，我每秒钟负责打印一次")

async def main():
    # 错误写法，会导致主程序阻塞
    # await hello_from_second()

    # 错误写法，会导致主程序阻塞
    # sleep_for_three = await asyncio.create_task(delay(3))
    # sleep_again = await asyncio.create_task(delay(3))
    sleep_for_three = asyncio.create_task(delay(3))
    sleep_again = asyncio.create_task(delay(3))

    await hello_from_second()

asyncio.run(main())
"""
开始休眠 3 秒
开始休眠 3 秒
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
休眠完成
休眠完成
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
你好，我每秒钟负责打印一次
"""