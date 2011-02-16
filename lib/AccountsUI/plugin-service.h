#ifndef PLUGINSERVICE_H
#define PLUGINSERVICE_H

#include <MApplicationService>

class PluginServicePrivate;

class PluginService : public MApplicationService
{
public:
    PluginService(QObject *parent = 0);
    ~PluginService();
    void launch();
    void setProviderName(const QString &name);
private:
    PluginServicePrivate *d_ptr;
    Q_DISABLE_COPY(PluginService);
    Q_DECLARE_PRIVATE(PluginService);
};
#endif

