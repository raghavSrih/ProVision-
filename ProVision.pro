#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T07:30:19
#
#-------------------------------------------------

QT       += core widgets gui webenginewidgets webchannel websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProVision
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /opt/softkinetic/DepthSenseSDK/include
INCLUDEPATH += /usr/local/include/boost
INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
LIBS += -L/opt/softkinetic/DepthSenseSDK/lib -lDepthSense -lDepthSensePlugins -lturbojpeg -lusb-1.0
LIBS += -L/usr/local/lib -lboost_system -lboost_thread
LIBS += -L/usr/local/lib -lopencv_aruco
LIBS += -L/usr/local/lib -lmosquitto

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    webdisplaywindow.cpp \
    provisionmain.cpp \
    websocketclientwrapper.cpp \
    websockettransport.cpp \
    procamcalibration.cpp \
    aruco_thread.cpp \
    depthsense_ds325.cpp \
    mqttclientwrapper.cpp \
    dscore.cpp \
    irdepthtouchtracker.cpp \
    backgroundupdaterthread.cpp \
    conversiontools.cpp \
    toucheventhandler.cpp \
    arucoeventshandler.cpp \
    touchresetapplication.cpp \
    testwidget.cpp

HEADERS += \
        mainwindow.h \
    webdisplaywindow.h \
    provisionmain.h \
    websocketclientwrapper.h \
    websockettransport.h \
    aruco_thread.h \
    procamcalibration.h \
    mqttclientwrapper.h \
    camera.h \
    camerafactory.h \
    cameratypes.h \
    dscore.h \
    applicationconstants.h \
    irdepthtouchtracker.h \
    touchtracker.h \
    backgroundupdaterthread.h \
    fpstracker.h \
    touch.h \
    fixedqueue.h \
    conversiontools.h \
    acquisitionparameters.h \
    toucheventhandler.h \
    arucoeventshandler.h \
    testwidget.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    webdisplay.qrc

DISTFILES += \
    jquery.min.js \
    default.html \
    qwebchannel.js \
    communicationchannel.js \
    sar_default.html \
    touch_test.html \
    jquery.mobile-1.4.5.min.js \
    jquery-1.11.3.min.js \
    touch.prototype.min.js \
    touch_injection.js
