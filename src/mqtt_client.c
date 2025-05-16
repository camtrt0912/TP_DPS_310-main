#include "../include/mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <signal.h>
#include <stdbool.h>

#define LOG_INFO(...) printf("[INFO] " __VA_ARGS__)
#define LOG_ERROR(...) fprintf(stderr, "[ERROR] " __VA_ARGS__)

static bool running = true;

// Fonction pour simuler l'allumage/extinction du capteur
void simulate_dps310_power(bool power_on) {
    if (power_on) {
        printf("DPS310 powered ON. Initializing...\n");
        // Simuler une initialisation
        sleep(1);
        printf("DPS310 initialized.\n");
    } else {
        printf("DPS310 powered OFF. Shutting down...\n");
        // Simuler un arrêt propre
        sleep(1);
        printf("DPS310 shut down.\n");
    }
}

// Gestion du signal pour arrêter proprement le programme
void handle_sigint(int sig) {
    printf("\nCTRL+C detected. Exiting program...\n");
    running = false;
}

// Fonction pour envoyer la température via MQTT
void send_temperature_mqtt(const char *topic, float temperature) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    // Création du client MQTT
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Connexion au broker MQTT
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect to MQTT broker, return code %d\n", rc);
        return;
    }

    // Préparation du message à envoyer
    char payload[100];
    snprintf(payload, sizeof(payload), "Sensor: %s, Temperature: %.2f °C", CLIENTID, temperature);

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    // Publication du message
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    printf("Publishing message to topic '%s': %s\n", topic, payload);

    // Attente de la confirmation de livraison
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message delivered\n");

    // Déconnexion et destruction du client MQTT
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

// Fonction pour envoyer la pression via MQTT
void send_pressure_mqtt(const char *topic, float pressure) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect to MQTT broker, return code %d\n", rc);
        return;
    }

    char payload[100];
    snprintf(payload, sizeof(payload), "Sensor: %s, Pressure: %.2f mbar", CLIENTID, pressure);

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    printf("Publishing message to topic '%s': %s\n", topic, payload);

    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message delivered\n");

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

// Wrapper pour adapter la fonction à pthread_create
void *subscribe_to_commands_thread(void *args) {
    subscribe_args_t *sub_args = (subscribe_args_t *)args;
    subscribe_to_commands(sub_args->topic, sub_args->command_handler);
    free(sub_args); // Libérer la mémoire allouée
    return NULL;
}

// Fonction pour écouter les commandes MQTT
void subscribe_to_commands(const char *topic, void (*command_handler)(const char *)) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    // Création du client MQTT
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Connexion au broker MQTT
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "Failed to connect to MQTT broker, return code %d\n", rc);
        return;
    }

    // Souscription au topic
    MQTTClient_subscribe(client, topic, QOS);

    while (1) {
        char *received_topic = NULL;
        int topic_len;
        MQTTClient_message *message = NULL;

        // Attente des messages
        rc = MQTTClient_receive(client, &received_topic, &topic_len, &message, 1000);
        if (rc == MQTTCLIENT_SUCCESS && message != NULL) {
            if (strncmp(received_topic, topic, topic_len) == 0) {
                char *payload = (char *)message->payload;
                command_handler(payload); // Appelle le gestionnaire de commandes
            }
            MQTTClient_freeMessage(&message);
            MQTTClient_free(received_topic);
        }
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

// Fonction pour sauvegarder les données dans un fichier
void save_data_to_file(const char *filename, float temperature, float pressure) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "Temperature: %.1f °C, Pressure: %.2f mbar\n", temperature, pressure);
    fclose(file);
}

void print_help() {
    printf("Usage: ./program [OPTIONS]\n");
    printf("Options:\n");
    printf("  -h, --help           Show this help message\n");
    printf("  -t, --topic-name     Specify the MQTT topic name\n");
    printf("  -i, --interval       Set the interval for sensor readings (in seconds)\n");
    printf("\nExample:\n");
    printf("  ./program -t my_topic -i 10\n");
}

