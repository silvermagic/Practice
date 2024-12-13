import oslo_messaging
from oslo_config import cfg


class NotificationEndpoint(object):
    filter_rule = oslo_messaging.NotificationFilter(publisher_id="^compute.*")

    def warn(self, context, publisher_id, event_type, payload, metadata):
        print(context, publisher_id, event_type, payload, metadata)


class ErrorEndpoint(object):
    filter_rule = oslo_messaging.NotificationFilter(publisher_id=r"^instance\..*\.start$")

    def error(self, context, publisher_id, event_type, payload, metadata):
        print(context, publisher_id, event_type, payload, metadata)


transport = oslo_messaging.get_notification_transport(cfg.CONF, url="rabbit://test:test@127.0.0.1:5672")
targets = [oslo_messaging.Target(topic="notifications", fanout=True)]
endpoints = [NotificationEndpoint(), ErrorEndpoint()]
server = oslo_messaging.get_notification_listener(transport, targets, endpoints)

if __name__ == "__main__":
    print("Starting notification server...")
    server.start()
    server.wait()
