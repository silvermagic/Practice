import asyncio
import threading

loop = asyncio.get_event_loop()

try:
    loop = asyncio.get_running_loop()
except RuntimeError:
    print("没有事件循环在运行")

async def main():
    loop = asyncio.get_running_loop()

loop.run_until_complete(main())

def create_loop():
    asyncio.get_event_loop()

threading.Thread(target=create_loop).start()

"""
没有事件循环在运行
RuntimeError: There is no current event loop in thread 'Thread-1 (create_loop)'.
"""