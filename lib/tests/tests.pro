include( ../../common-project-config.pri )
include( ../../common-vars.pri )

QMAKE_EXTRA_TARGETS += check
check.depends = libaccountplugin-test
check.commands = ./libaccountplugin-test

TARGET = libaccountplugin-test

CONFIG += \
    qtestlib \
    qt \
    meegotouch
SOURCES += \
    test.cpp
HEADERS += \
    test.h

QT += core xml

LIBS += -lAccountPlugin
PKGCONFIG += \
    accounts-qt \
    libsignon-qt
DEPENDPATH += $${INCLUDEPATH}

include( ../../common-installs-config.pri )

DATA_PATH = $${INSTALL_PREFIX}/share/libaccountplugin-tests/

DEFINES += \
    SERVICES_DIR=\\\"$$DATA_PATH\\\" \
    PROVIDERS_DIR=\\\"$$DATA_PATH\\\"

service.path = $$DATA_PATH
service.files += gmail.service
INSTALLS     += service

testsuite.path = $$DATA_PATH
testsuite.files = tests.xml
INSTALLS += testsuite
