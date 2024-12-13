import logging
import random
import time

import eventlet
import oslo_messaging
from oslo_config import cfg

# 使用 eventlet
eventlet.monkey_patch()

# 设置日志格式
server = ".".join(str(random.randint(0, 255)) for _ in range(4))
logging.basicConfig(level=logging.INFO, format=f"%(asctime)s - {server} [%(levelname)s] - %(message)s")
logger = logging.getLogger(__name__)


class ServerControlEndpoint(object):
    target = oslo_messaging.Target(namespace="control", version="2.0")

    def status(self, ctx):
        logger.info(f"Service status checked by {ctx.get('user')}...")
        return {"status": "running", "timestamp": time.time()}


class MathOperationsEndpoint(object):
    target = oslo_messaging.Target(namespace="math", version="1.0")

    def add(self, ctx, a, b):
        result = a + b
        logger.info(f"Adding {a} + {b} = {result}")
        return result

    def divide(self, ctx, a, b):
        if b == 0:
            logger.warning("Division by zero attempted")
            raise ValueError("Cannot divide by zero")
        result = a / b
        logger.info(f"Dividing {a} / {b} = {result}")
        return result


# 配置 transport
transport = oslo_messaging.get_rpc_transport(cfg.CONF, url="rabbit://test:test@127.0.0.1:5672")

# 配置目标 (topic 和 server 必须匹配客户端)
target = oslo_messaging.Target(topic="demo", server=server)

# 定义 endpoints 列表
endpoints = [ServerControlEndpoint(), MathOperationsEndpoint()]

# 创建 RPC 服务
server = oslo_messaging.get_rpc_server(transport, target, endpoints, executor="eventlet")

try:
    logger.info("RPC Server starting...")
    server.start()
    while True:
        time.sleep(1)  # 保持服务端运行
except KeyboardInterrupt:
    logger.info("Interrupt received, stopping server...")
finally:
    server.stop()
    server.wait()
    logger.info("Server stopped.")
