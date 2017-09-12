#ifndef MQTTCLIENTWRAPPER_H
#define MQTTCLIENTWRAPPER_H

#include <QObject>
#include <QThread>
#include <mosquitto.h>
#include <QJsonObject>
#include <QJsonDocument>



class mqttclientwrapper : public QThread
{
    Q_OBJECT
public:
    mqttclientwrapper(char *host, int port, int keepalive, int qos,  bool clean_session, const char * username, const char * password);
    ~mqttclientwrapper();
    void received_callback(char *topic, char *message);
    void subscribe(const char *topic, const quint8 qos);

private:
    void run();

protected:
    void connected();
    void disconnected();
    void error(const int errorNumber);
    void subscribed(const QString& topic, const quint8 qos);
    void unsubscribed(const QString& topic);
    void published(const quint16 msgid, const quint8 qos);
    char *host;
    int port;
    int keepalive, qos;

public slots:
    void publish(char *topic, char * message);

signals:    
    void received(char *topic, char *message);
};

#endif // MQTTCLIENTWRAPPER_H
