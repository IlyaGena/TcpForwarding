QT -= gui

TEMPLATE = subdirs
CONFIG += c++11 console
CONFIG -= app_bundle

SUBDIRS += \
    Client_SSH \
    Server_SSH

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
