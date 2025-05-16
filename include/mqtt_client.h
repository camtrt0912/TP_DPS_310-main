#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#define ADDRESS     "tcp://test.mosquitto.org:1883"
#define CLIENTID    "DPS310_Client"
#define QOS         1
#define TIMEOUT     10000L

void send_temperature_mqtt(const char *topic, float temperature);
void send_pressure_mqtt(const char *topic, float pressure);
void subscribe_to_commands(const char *topic, void (*command_handler)(const char *));
void *subscribe_to_commands_thread(void *args);

// Structure pour passer les arguments à la fonction du thread
typedef struct {
    char topic[256];
    void (*command_handler)(const char *);
} subscribe_args_t;

#endif // MQTT_CLIENT_H