import logging

import oslo_messaging
from oslo_config import cfg

# 设置日志格式
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")
logger = logging.getLogger(__name__)


# 创建 RPC 客户端
def create_client(target_topic, namespace=None, version="1.0"):
    transport = oslo_messaging.get_rpc_transport(cfg.CONF, url="rabbit://test:test@127.0.0.1:5672")
    target = oslo_messaging.Target(topic=target_topic, namespace=namespace, version=version)
    return oslo_messaging.get_rpc_client(transport, target)


# 测试 MathOperationsEndpoint
def test_math_operations():
    logger.info("Testing MathOperationsEndpoint")
    client = create_client(target_topic="demo", namespace="math")

    # 正常调用
    try:
        logger.info("Testing call add operation...")
        result = client.call({}, "add", a=5, b=3)
        logger.info(f"Result of call add: {result}")

        logger.info("Testing cast add operation...")
        result = client.cast({}, "add", a=5, b=5)
        logger.info(f"Result of cast add: {result}")

        logger.info("Testing call divide operation...")
        result = client.call({}, "divide", a=10, b=2)
        logger.info(f"Result of call divide: {result}")
    except Exception as e:
        logger.error(f"Error during normal operations: {e}")


# 测试 ServerControlEndpoint
def test_server_control():
    logger.info("Testing ServerControlEndpoint")
    client = create_client(target_topic="demo", namespace="control", version="2.0")

    # 正常调用
    try:
        logger.info("Testing status operation...")
        client.prepare(fanout=True).cast({"user": "admin"}, "status")
    except Exception as e:
        logger.error(f"Error during normal operations: {e}")


def test_math_operations_with_errors():
    client = create_client(target_topic="demo", namespace="math")

    # 参数错误
    try:
        logger.info("Testing add operation with missing parameters...")
        client.call({}, "add", a=5)
    except Exception as e:
        logger.error(f"Expected error for invalid parameters: {e}")

    try:
        logger.info("Testing divide by zero...")
        client.call({}, "divide", a=10, b=0)
    except Exception as e:
        logger.error(f"Expected error for division by zero: {e}")

    # 方法不对
    try:
        logger.info("Testing with incorrect method...")
        client.call({}, "mul", a=5, b=3)
    except Exception as e:
        logger.error(f"Expected error for incorrect method: {e}")

    # 主题不对
    try:
        logger.info("Testing with incorrect topic...")
        cctx = client.prepare(timeout=5, topic="wrong_topic")
        cctx.call({}, "add", a=5, b=3)
    except Exception as e:
        logger.error(f"Expected error for incorrect topic: {e}")

    # 版本不对
    try:
        logger.info("Testing with incorrect version...")
        cctx = client.prepare(timeout=5, version="3.0")
        cctx.call({}, "add", a=5, b=3)
    except Exception as e:
        logger.error(f"Expected error for incorrect version: {e}")


if __name__ == "__main__":
    logger.info("Starting client tests...")

    test_server_control()
    test_math_operations()
    test_math_operations_with_errors()

    logger.info("Client tests completed.")
