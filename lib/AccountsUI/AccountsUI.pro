include( ../../common-project-config.pri )
include( ../../common-vars.pri )

# -----------------------------------------------------------------------------
# target setup
# -----------------------------------------------------------------------------
TEMPLATE = lib
TARGET = AccountPlugin
VERSION = 1.0.0
CONFIG += qt \
    meegotouch \
    link_pkgconfig
QT += xml network

# -----------------------------------------------------------------------------
# dependencies
# -----------------------------------------------------------------------------
PKGCONFIG += accounts-qt \
    libsignon-qt \
    meegotouch

# -----------------------------------------------------------------------------
# input
# -----------------------------------------------------------------------------
HEADERS += account-plugin-interface.h \
    service-plugin-interface.h \
    abstract-account-setup-context.h \
    abstract-service-setup-context.h \
    abstract-setup-context.h \
    credentialdialog.h \
    credentialwidget.h \
    credentialwidgetview.h \
    credential-image-widget.h \
    debug.h \
    genericaccountsetupform.h \
    generic-account-setup-context.h \
    genericaccountsetupformview.h \
    generic-service-plugin.h \
    accountsmanagersingleton.h \
    service-helper.h \
    provider-plugin-process.h \
    provider-plugin-process-priv.h \
    provider-plugin-proxy.h \
    provider-plugin-proxy-priv.h \
    add-account-page.h \
    service-selection-page.h \
    service-model.h \
    generic-service-setup-context.h \
    parser.h \
    settings-widgets.h \
    layouts.h \
    ui-builder.h \
    accountsettingspage.h \
    service-settings-widget.h \
    common.h \
    network-session-manager.h \
    network-session-manager-priv.h\
    settings-page.h \
    sort-service-model.h \
    account-sync-handler.h

SOURCES += abstract-account-setup-context.cpp \
    abstract-service-setup-context.cpp \
    abstract-setup-context.cpp \
    credentialdialog.cpp \
    credentialwidget.cpp \
    credentialwidgetview.cpp \
    generic-account-setup-context.cpp \
    genericaccountsetupform.cpp \
    genericaccountsetupformview.cpp \
    generic-service-plugin.cpp \
    accountsmanagersingleton.cpp \
    service-helper.cpp \
    provider-plugin-process.cpp \
    provider-plugin-proxy.cpp \
    add-account-page.cpp \
    service-selection-page.cpp \
    service-model.cpp \
    generic-service-setup-context.cpp \
    parser.cpp \
    settings-widgets.cpp \
    layouts.cpp \
    accountsettingspage.cpp \
    service-settings-widget.cpp \
    common.cpp \
    network-session-manager.cpp \
    network-session-manager-priv.cpp \
    settings-page.cpp \
    sort-service-model.cpp \
    account-sync-handler.cpp

MODEL_HEADERS += \
    credentialwidgetmodel.h \
    genericaccountsetupformmodel.h
STYLE_HEADERS += \
    credentialwidgetstyle.h \
    genericaccountsetupformstyle.h

# add model and style headers to the project
HEADERS += $$MODEL_HEADERS
HEADERS += $$STYLE_HEADERS

# headers are the files which will be installed with "make install"
headers.files += AccountPluginInterface \
    account-plugin-interface.h \
    ServicePluginInterface \
    service-plugin-interface.h \
    AbstractAccountSetupContext \
    abstract-account-setup-context.h \
    AbstractServiceSetupContext \
    abstract-service-setup-context.h \
    abstract-setup-context.h \
    CredentialDialog \
    credentialdialog.h \
    CredentialWidget \
    credentialwidget.h \
    Debug \
    debug.h \
    GenericAccountSetupContext \
    generic-account-setup-context.h \
    ServiceHelper \
    service-helper.h \
    ProviderPluginProcess \
    provider-plugin-process.h \
    ProviderPluginProxy \
    provider-plugin-proxy.h \
    AddAccountPage \
    add-account-page.h \
    AccountsManagerSingleton \
    accountsmanagersingleton.h \
    GenericServiceSetupContext \
    generic-service-setup-context.h \
    Parser \
    parser.h \
    UiBuilder \
    ui-builder.h \
    GenericServicePlugin \
    generic-service-plugin.h \
    AccountSettingsPage \
    accountsettingspage.h \
    Common \
    common.h \
    NetworkSessionManager \
    network-session-manager.h \
    account-sync-handler.h

# -----------------------------------------------------------------------------
# common installation setup
# NOTE: remember to set headers.files before this include to have the headers
# properly setup.
# -----------------------------------------------------------------------------
include( ../../common-installs-config.pri )

# -----------------------------------------------------------------------------
# Installation target for application resources
# -----------------------------------------------------------------------------
pkgconfig.files = AccountPlugin.pc
pkgconfig.path = $${INSTALL_PREFIX}/lib/pkgconfig
view_configuration.files = $${_PRO_FILE_PWD_}/libAccountPlugin.conf
view_configuration.path = $${INSTALL_PREFIX}/share/themes/base/meegotouch/libAccountPlugin
style_configuration.files = $${_PRO_FILE_PWD_}/libAccountPlugin.css
style_configuration.path = $${INSTALL_PREFIX}/share/themes/base/meegotouch/libAccountPlugin/style
INSTALLS += \
	pkgconfig \
        view_configuration \
        style_configuration
