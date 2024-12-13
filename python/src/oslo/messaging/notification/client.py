import oslo_messaging
from oslo_config import cfg

transport = oslo_messaging.get_notification_transport(cfg.CONF, url="rabbit://test:test@127.0.0.1:5672")
notifier = oslo_messaging.Notifier(transport, "compute.host", driver="messaging", topics=["notifications"])
notification_payload = {
    "event_type": "example.event",
    "timestamp": "2024-12-14T00:00:00Z",
    "details": {
        "message": "This is a warn test notification",
        "severity": "WARN",
    },
}
notifier.warn({}, "example.event", notification_payload)

notification_payload = {
    "event_type": "example.event",
    "timestamp": "2024-12-14T00:00:00Z",
    "details": {
        "message": "This is a error test notification",
        "severity": "ERROR",
    },
}
notifier.prepare(publisher_id="instance.123.start").error({}, "example.event", notification_payload)
