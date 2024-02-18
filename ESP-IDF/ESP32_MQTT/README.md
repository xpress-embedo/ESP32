ESP32 MQTT Device Control
====================

Using the Free MQTT broker `mqtt.eclipseprojects.io`.

Pulish Command
```
mosquitto_pub -h "mqtt.eclipseprojects.io" -t "my_topic" -m "Hello World"
```

Subcsribe Command
```
mosquitto_sub -h "mqtt.eclipseprojects.io" -t "my_topic"
```


