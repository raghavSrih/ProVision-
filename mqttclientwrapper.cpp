#include "mqttclientwrapper.h"
#include <mosquitto.h>

struct mosquitto *mosq = NULL;
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(message->payloadlen){
        //printf("%s %s\n", message->topic, (char*)message->payload);
    }else{
        //printf("%s (null)\n", message->topic);
    }
    //reinterpret_cast<mqttclientwrapper*>(userdata)->received(message->topic, (char*)message->payload);
    //fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
    }else{
        //fprintf(stderr, "Connect failed\n");
    }
}

void my_disconnect_callback(struct mosquitto *mosq, void *userdata, int reason){
    //printf("Client Disconnected\n");
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    //printf("%s\n", str);
}

mqttclientwrapper::mqttclientwrapper(char *host, int port, int keepalive, int qos,  bool clean_session, const char * username, const char * password)
{
    this->host = host;
    this->port = port;
    this->keepalive = keepalive;
    this->qos = qos;
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if(mosq){
        mosquitto_log_callback_set(mosq,  my_log_callback);
        mosquitto_connect_callback_set(mosq, my_connect_callback);
        mosquitto_disconnect_callback_set(mosq, my_disconnect_callback);
        mosquitto_message_callback_set(mosq, my_message_callback);
        mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
        mosquitto_username_pw_set(	mosq, username,	password);
    }
    else {
        fprintf(stderr, "Error: Out of memory.\n");
    }
}

void mqttclientwrapper::run() {
    if(mosquitto_connect(mosq, host, port, keepalive)){
            fprintf(stderr, "Unable to connect.\n");
    }
    else {
        mosquitto_loop_forever(mosq, -1, 1);
    }
}

mqttclientwrapper::~mqttclientwrapper(){
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

void mqttclientwrapper::subscribe(const char *topic, const quint8 qos){
    mosquitto_subscribe(mosq, NULL, topic, qos);
}

void mqttclientwrapper::publish(char *topic, char * message){

    mosquitto_publish(	mosq, NULL, topic, strlen(message), message, 0, false);
}

void mqttclientwrapper::received_callback(char *topic, char *message){
    emit received(topic, message);
}





