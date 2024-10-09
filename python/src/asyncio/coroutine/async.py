import asyncio

async def coroutine_add_one(number: int) -> int:
    return number + 1

def add_one(number: int) -> int:
    return number + 1

function_result = add_one(1)
coroutine_result = asyncio.run(coroutine_add_one(1))
print(function_result)
print(type(function_result))
print("----")
print(coroutine_result)
print(type(coroutine_result))
"""
2
<class 'int'>
----
2
<class 'int'>
"""

async def delay(seconds: int):
    print(f"开始休眠 {seconds} 秒")
    await asyncio.sleep(seconds)
    print(f"休眠完成")
    return seconds

async def add_two(number: int):
    return number + 2

async def hello_world():
    await delay(1)
    return "Hello, World!"

async def main():
    # 暂停 main()，直到 add_two(1) 返回
    one_plus_two = await add_two(1)
    # 暂停 main()，直到 hello_world() 返回
    message = await hello_world()
    print(one_plus_two)
    print(message)

asyncio.run(main())
"""
开始休眠 1 秒
休眠完成
3
Hello, World!
"""