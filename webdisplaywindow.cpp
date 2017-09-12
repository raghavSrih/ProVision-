#include "webdisplaywindow.h"

class QJsonObject;

WebDisplayWindow::WebDisplayWindow()
{
    setAttribute(Qt::WA_DeleteOnClose, true);


    QFile file;
    file.setFileName(":/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
    file.close();

    QFile file2;
    file2.setFileName(":/touch_injection.js");
    file2.open(QIODevice::ReadOnly);
    jQueryMobile = file2.readAll();
    file2.close();

    QFile file3;
    file3.setFileName(":/jquery.mobile-1.4.5.min.js");
    file3.open(QIODevice::ReadOnly);
    touchInjection = file3.readAll();
    file3.close();

    QFileInfo jsFileInfo(QDir::currentPath() + "/qwebchannel.js");

    if (!jsFileInfo.exists())
        QFile::copy(":/qtwebchannel/qwebchannel.js",jsFileInfo.absoluteFilePath());
    view.setParent(this);
    view.setAttribute(Qt::WA_AcceptTouchEvents, true);
    view.page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    server = new QWebSocketServer(QStringLiteral("QWebChannel Standalone Example Server"),
                            QWebSocketServer::NonSecureMode);
    if (!server->listen(QHostAddress::LocalHost, 11227)) {
        qInfo("Failed to open web socket server.");
    }

    // wrap WebSocket clients in QWebChannelAbstractTransport objects
    clientWrapper = new WebSocketClientWrapper(server);

    // setup the channel    
    QObject::connect(clientWrapper, &WebSocketClientWrapper::clientConnected,
                     &channel, &QWebChannel::connectTo);
    channel.registerObject(QStringLiteral("chatserver"), this);


    QObject::connect(&view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

}

void WebDisplayWindow::show(){
    //QUrl url = (QUrl)"http://scripty2.com/demos/touch/touchspector/";
    QUrl url = (QUrl)"https://stark-wave-40519.herokuapp.com/";
    firstLoad = false;
    view.load(url);    
    view.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint);
    view.showFullScreen();
    setCentralWidget(&view);
}

void WebDisplayWindow::finishLoading(bool)
{
    view.page()->runJavaScript(jQuery);
    view.page()->runJavaScript(jQueryMobile);
    view.page()->runJavaScript(touchInjection);
    if(!firstLoad) {
        view.page()->runJavaScript("document.cookie=\"VISITOR_INFO1_LIVE=oKckVSqvaGw; path=/; domain=.youtube.com\";window.location.reload();");
        firstLoad = true;
    }
}

void WebDisplayWindow::setUrl(char * urlString) {
    QUrl url = QUrl(urlString);
    view.load(url);
    view.setFocus();
}

void WebDisplayWindow::setHTMLText(char * text) {
    view.setHtml(QString::fromUtf8(text));
    view.setFocus();
}

void WebDisplayWindow::sendWebPageData(QJsonObject data){
    emit sendJsonData_toHTML(data);
}
void WebDisplayWindow::receiveTouchEvents(QJsonObject data){

    sendTouchEventsToWebView(data);
}

void WebDisplayWindow::receiveArUcoEvents(QJsonObject data){

    sendArUcoEventsToWebView(data);
}

void WebDisplayWindow::receiveJsonData_FromHTML(const QJsonObject data)
{
    QJsonDocument doc(data);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug("Received Data from HTML : %s", strJson.toLatin1().constData());
    //sendWebPageData(data);
    QJsonObject touchEvent = {};
    QJsonObject data1 = {};
    data1.insert("event", "touchstart");
    data1.insert("x", 600);
    data1.insert("y", 600);
    touchEvent.insert("touch", data1);
    sendTouchEventsToWebView(touchEvent);
}

void WebDisplayWindow::sendTouchEventsToWebView(QJsonObject data) {
        QJsonObject touchDataObj = data["touch"].toObject();
        QJsonDocument doc(touchDataObj);
        QString touchDataObjStr(doc.toJson(QJsonDocument::Compact));
        QString code = QString("sendTouchEvent(%1);")
                .arg(touchDataObjStr);
        view.page()->runJavaScript(code);

//    else if((data["touch"].toObject())["event"] == "touchmove") {
//        int x = (data["touch"].toObject())["x"].toInt();
//        int y = (data["touch"].toObject())["y"].toInt();
//        int id = (data["touch"].toObject())["id"].toInt();
//        QString code = QString("sendTouchEvent(%1 , %2 , %3, 'touchmove');")
//                .arg(x)
//                .arg(y)
//                .arg(id);
//        view.page()->runJavaScript(code);
//    }
//    else if((data["touch"].toObject())["event"] == "touchend") {
//        int x = (data["touch"].toObject())["x"].toInt();
//        int y = (data["touch"].toObject())["y"].toInt();
//        int id = (data["touch"].toObject())["id"].toInt();
//        QString code = QString("sendTouchEvent(%1 , %2 , %3, 'touchend');")
//                .arg(x)
//                .arg(y)
//                .arg(id);
//        view.page()->runJavaScript(code);
//    }
//    else if((data["touch"].toObject())["event"] == "touchcancel") {
//        int x = (data["touch"].toObject())["x"].toInt();
//        int y = (data["touch"].toObject())["y"].toInt();
//        QString code = QString("sendTouchEvent(%1 , %2 , 'touchcancel');")
//                .arg(x)
//                .arg(y);
//        view.page()->runJavaScript(code);
//    }
}

void WebDisplayWindow::sendArUcoEventsToWebView(QJsonObject data) {
    if((data["aruco"].toObject())["event"] == "detected") {
        int id = (data["aruco"].toObject())["id"].toInt();
        QString strId = QString("aruco_%1").arg(id);
        int x = (data["aruco"].toObject())["x"].toInt();
        int y = (data["aruco"].toObject())["y"].toInt();
        QString code = QString("qt.jQuery(' #%1 ').css({ left:  %2 + 20, top:   %3});").arg(strId).arg(x).arg(y);
        view.page()->runJavaScript(code);
    }
}

WebDisplayWindow::~WebDisplayWindow(){

    //delete view;
    delete server;
    delete clientWrapper;
}

void WebDisplayWindow::LeftMouseClick(QPoint clickPos)
{

    QWidget* eventsReciverWidget = nullptr;
    foreach(QObject* obj, view.children())
    {
        QWidget* wgt = qobject_cast<QWidget*>(obj);
        if (wgt)
        {
            eventsReciverWidget = wgt;
            eventsReciverWidget->setAttribute(Qt::WA_AcceptTouchEvents, true);
            break;
        }
    }
    QTouchEvent::TouchPoint point; point.setPos(clickPos);
    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(point);
    QTouchDevice * surfaceTouch = new QTouchDevice();
    surfaceTouch->setType(QTouchDevice::TouchScreen);
    QTouchEvent *touch = new QTouchEvent(QEvent::TouchBegin, surfaceTouch , Qt::NoModifier, Qt::TouchPointStates(), touchPoints);
    QCoreApplication::postEvent(eventsReciverWidget, touch);
}
