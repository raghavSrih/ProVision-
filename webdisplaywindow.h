#ifndef WEBDISPLAYWINDOW_H
#define WEBDISPLAYWINDOW_H

#include <QtWidgets>
#include <QtWebEngineWidgets>
#include <QWebEngineView>
#include <QFileInfo>
#include <QDir>
#include <QtWebSockets/QWebSocketServer>
#include <websocketclientwrapper.h>
#include <websockettransport.h>
#include <QWebChannel>
#include <QDialog>
#include <QJsonObject>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QWebEngineView;
class QLineEdit;
QT_END_NAMESPACE

class WebDisplayWindow : public QMainWindow
{
    Q_OBJECT

public:
    WebDisplayWindow();
    ~WebDisplayWindow();
    void show();

signals:
    void sendJsonData_toHTML(QJsonObject data);

public slots:
    void setUrl(char * url);
    void setHTMLText(char * text);
    void receiveTouchEvents(QJsonObject data);
    void receiveArUcoEvents(QJsonObject data);
    void receiveJsonData_FromHTML(QJsonObject data);

protected slots:
    void finishLoading(bool);
private:
    void sendWebPageData(QJsonObject data);
    void sendTouchEventsToWebView(QJsonObject data);
    void sendArUcoEventsToWebView(QJsonObject data);
    void LeftMouseClick(QPoint clickPos);
private:
    QString jQuery;
    bool firstLoad;
    QString jQueryMobile;
    QString touchInjection;
    QWebEngineView view;
    QWebChannel channel;
    QWebSocketServer * server;
    WebSocketClientWrapper * clientWrapper;
};

#endif // WEBDISPLAYWINDOW_H
