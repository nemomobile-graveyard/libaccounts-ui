#include "plugin-service.h"
#include "provider-plugin-process.h"
#include "provider-plugin-proxy.h"

#include <MBanner>
#include <MApplication>
#include <MApplicationWindow>

class PluginServicePrivate
{
public:
    PluginServicePrivate()
        : providerName(QString())
    {}
    QString providerName;
};

PluginService::PluginService(QObject *parent):
        MApplicationService(QLatin1String(""), parent),
        d_ptr(new PluginServicePrivate())
{}

PluginService::~PluginService()
{
    delete d_ptr;
}

void PluginService::launch()
{
    AccountsUI::ProviderPluginProcess *providerPluginProcess = AccountsUI::ProviderPluginProcess::instance();
    if (!providerPluginProcess->isSetupComplete()) {
        MApplicationWindow *window = MApplication::instance()->activeApplicationWindow();
        MApplicationService::launch();
        MBanner *banner = new MBanner();
        banner->setStyleName("InformationBanner");
        QString translatedProviderName( AccountsUI::ProviderPluginProcess::instance()->translatedProviderName());
        QString bannerTitle = qtTrId("qtn_acc_view_update_failed_infobanner").arg(translatedProviderName);
        banner->setTitle(bannerTitle);
        banner->appear(window, MSceneWindow::DestroyWhenDone);
    } else {
        QVariant data(AccountsUI::ProviderPluginProxy::RETURN_TO_ACCOUNTS_PAGE);
        providerPluginProcess->setExitData(data);
        providerPluginProcess->quit();
    }
}

void PluginService::setProviderName(const QString &name)
{
    Q_D(PluginService);
    setServiceName(QString("com.nokia.AccountPlugin.%1").arg(name));
    d->providerName = name;
}

