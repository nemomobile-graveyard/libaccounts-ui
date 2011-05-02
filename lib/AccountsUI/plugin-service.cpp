#include "plugin-service.h"

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
    Q_D(PluginService);
    MApplicationWindow *window = MApplication::instance()->activeApplicationWindow();
    MApplicationService::launch();
    MBanner *banner = new MBanner();
    banner->setStyleName("InformationBanner");
    if (!d->displayName.isEmpty())
        banner->setTitle(qtTrId("qtn_acc_view_update_failed_infobanner").arg(d->displayName));
    banner->appear(window, MSceneWindow::DestroyWhenDone);
}

void PluginService::setProviderName(const QString &name)
{
    Q_D(PluginService);
    setServiceName(QString("com.nokia.AccountPlugin.%1").arg(name));
    d->providerName = name;
}

