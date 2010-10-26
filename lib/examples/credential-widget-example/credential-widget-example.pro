include( ../../common-project-config.pri )
include( ../../common-vars.pri )

TARGET = credentialwidgetexample
TEMPLATE = app

# for the style to work:
M_APPLICATION_NAME = $$TARGET
CONFIG += meegotouch
CONFIG += link_pkgconfig
CONFIG += qt

SOURCES += main.cpp \
           credentialwidgetexample.cpp
HEADERS += credentialwidgetexample.h

CONFIG += \
    console

# -----------------------------------------------------------------------------
# dependencies
# -----------------------------------------------------------------------------
PKGCONFIG += meegotouch
PKGCONFIG += signon-plugins
PKGCONFIG += libsignon-qt
PKGCONFIG += AccountsUI

LIBS += -lAccountPlugin

headers.files = $$HEADERS
include( ../../common-installs-config.pri )

# -----------------------------------------------------------------------------
# Installation target for application resources
# -----------------------------------------------------------------------------
view_configuration.path = $${INSTALL_PREFIX}/share/themes/base/meegotouch/credentialwidgetexample
view_configuration.files = $${_PRO_FILE_PWD_}/credentialwidgetexample.conf
style_css.path = $${INSTALL_PREFIX}/share/themes/base/meegotouch/credential-widget/style
style_css.files = $${_PRO_FILE_PWD_}/credentialwidgetexample.css
INSTALLS += view_configuration
INSTALLS += style_css

