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
    mobility \
    link_pkgconfig \
    gallerycore \
    qtsparql \
    mngfclient

MOBILITY = systeminfo

QT += xml \
    network \
    core

# -----------------------------------------------------------------------------
# dependencies
# -----------------------------------------------------------------------------
PKGCONFIG += accounts-qt \
    AccountSetup \
    libsignon-qt \
    meegotouch \
    TelepathyQt4 \
    QtSparql \
    QtSparqlTrackerLive

# -----------------------------------------------------------------------------
# input
# -----------------------------------------------------------------------------
HEADERS += account-plugin-interface.h \
    service-plugin-interface.h \
    abstract-account-setup-context.h \
    abstract-service-setup-context.h \
    abstract-setup-context.h \
    account-settings-page.h \
    account-settings-page-priv.h \
    account-setup-finished-page.h \
    account-setup-finished-page-priv.h \
    credentialdialog.h \
    credentialwidget.h \
    credentialwidgetview.h \
    credentialimagewidget.h \
    debug.h \
    genericaccountsetupform.h \
    generic-account-setup-context.h \
    genericaccountsetupformview.h \
    generic-service-plugin.h \
    accountsmanagersingleton.h \
    service-helper.h \
    last-page-actions.h \
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
    service-settings-widget.h \
    common.h \
    network-session-manager.h \
    network-session-manager-priv.h \
    settings-page.h \
    sort-service-model.h \
    account-sync-handler.h \
    accountlistitem.h \
    validation-data.h \
    account-setup-finished-widget.h \
    plugin-service.h \
    basic-item-widget.h \
    avatar-selector.h \
    imagecontentprovider.h \
    basic-header-widget.h \
    basic-service-widget.h
SOURCES += abstract-account-setup-context.cpp \
    abstract-service-setup-context.cpp \
    abstract-setup-context.cpp \
    account-settings-page.cpp \
    credentialdialog.cpp \
    credentialwidget.cpp \
    credentialwidgetview.cpp \
    generic-account-setup-context.cpp \
    genericaccountsetupform.cpp \
    genericaccountsetupformview.cpp \
    generic-service-plugin.cpp \
    accountsmanagersingleton.cpp \
    service-helper.cpp \
    last-page-actions.cpp \
    provider-plugin-process.cpp \
    provider-plugin-proxy.cpp \
    add-account-page.cpp \
    service-selection-page.cpp \
    service-model.cpp \
    generic-service-setup-context.cpp \
    parser.cpp \
    settings-widgets.cpp \
    layouts.cpp \
    service-settings-widget.cpp \
    common.cpp \
    network-session-manager.cpp \
    network-session-manager-priv.cpp \
    settings-page.cpp \
    sort-service-model.cpp \
    account-sync-handler.cpp \
    account-setup-finished-page.cpp \
    accountlistitem.cpp \
    account-setup-finished-widget.cpp \
    plugin-service.cpp \
    basic-item-widget.cpp \
    avatar-selector.cpp \
    imagecontentprovider.cpp \
    basic-header-widget.cpp \
    basic-service-widget.cpp
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
    AbstractAccountSetupContext \
    abstract-account-setup-context.h \
    AbstractServiceSetupContext \
    abstract-service-setup-context.h \
    AbstractSetupContext \
    abstract-setup-context.h \
    AccountSettingsPage \
    account-settings-page.h \
    account-setup-finished-page.h \
    AccountSyncHandler \
    account-sync-handler.h \
    AccountsManagerSingleton \
    accountsmanagersingleton.h \
    AddAccountPage \
    add-account-page.h \
    BasicHeaderWidget \
    basic-header-widget.h\
    BasicItemWidget \
    basic-item-widget.h \
    Common \
    common.h \
    CredentialDialog \
    credentialdialog.h \
    CredentialWidget \
    credentialwidget.h \
    credentialwidgetmodel.h \
    Debug \
    debug.h \
    GenericAccountSetupContext \
    generic-account-setup-context.h \
    GenericAccountSetupForm \
    genericaccountsetupform.h \
    GenericAccountSetupFormModel \
    genericaccountsetupformmodel.h \
    genericaccountsetupformstyle.h \
    GenericServicePlugin \
    generic-service-plugin.h \
    GenericServiceSetupContext \
    generic-service-setup-context.h \
    LastPageActions \
    last-page-actions.h \
    NetworkSessionManager \
    network-session-manager.h \
    Parser \
    parser.h \
    ProviderPluginProcess \
    provider-plugin-process.h \
    ProviderPluginProxy \
    provider-plugin-proxy.h \
    ServiceHelper \
    service-helper.h \
    ServiceModel \
    service-model.h \
    ServicePluginInterface \
    service-plugin-interface.h \
    ServiceSelectionPage \
    service-selection-page.h \
    SortServiceModel \
    sort-service-model.h \
    service-settings-widget.h \
    ServiceSettingsWidget \
    UiBuilder \
    ui-builder.h \
    ValidationData \
    validation-data.h \
    basic-item-widget.h \
    basic-header-widget.h \
    basic-service-widget.h

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
