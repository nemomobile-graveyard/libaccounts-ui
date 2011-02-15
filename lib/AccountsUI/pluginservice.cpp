#include "pluginservice.h"

#include <MBanner>
#include <MApplication>
#include <MApplicationWindow>

class PluginServicePrivate
{
public:
    PluginServicePrivate()
        : providerName(QString())
    {}
    ~PluginServicePrivate() {}
    QString providerName;
};

PluginService::PluginService(QObject *parent):
        MApplicationService(QLatin1String(""), parent),
        d_ptr(new PluginServicePrivate())
{}

PluginService::~PluginService()
{}

void PluginService::launch()
{
    Q_D(PluginService);
    MApplicationWindow *window = MApplication::instance()->activeApplicationWindow();
    window->show();
    MApplicationService::launch();
    MBanner *banner = new MBanner();
    banner->setStyleName("InformationBanner");
    if (!d->providerName.isEmpty())
        banner->setTitle(qtTrId("qtn_acc_view_update_failed_infobanner").arg(d->providerName));
    banner->appear(MSceneWindow::DestroyWhenDone);
}

void PluginService::setProviderName(const QString &name)
{
    Q_D(PluginService);
    d->providerName = name;
}

