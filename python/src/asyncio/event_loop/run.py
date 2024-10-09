import asyncio

async def main():
    await asyncio.sleep(1)
    print("执行完毕")

loop = asyncio.new_event_loop()
try:
    loop.run_until_complete(main())
finally:
    loop.close()
"""
执行完毕
"""

def some_func():
    print("我将稍后被调用")

async def main2():
    # 协程需要扔到事件循环里面运行，而当协程运行的时候，也可以获取所在的事件循环
    loop = asyncio.get_running_loop()
    loop.call_soon(some_func)
    print("在我之前执行吗？")
    await asyncio.sleep(1) # call_soon 在事件循环的下一次迭代的时候运行，即 some_func 将在这里被调用
    print("还是在我之后执行？")

loop = asyncio.new_event_loop()
try:
    loop.run_until_complete(main2())
finally:
    loop.close()
"""
在我之前执行吗？
我将稍后被调用
还是在我之后执行？
"""

# 不可以在外部调用
loop = asyncio.get_running_loop()
"""
    loop = asyncio.get_running_loop()
RuntimeError: no running event loop
"""