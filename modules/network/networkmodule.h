#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "moduleinterface.h"

namespace dcc {
namespace network {

class NetworkDevice;
class NetworkModuleWidget;
class NetworkWorker;
class NetworkModel;
class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~NetworkModule();

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private slots:
    void showVpnPage();
    void showPppPage();
    void showProxyPage();
    void showDetailPage();
    void showDeviceDetailPage(NetworkDevice *dev);
    void showWiredConnectionEditPage(const QString &session);

private:
    NetworkModel *m_networkModel;
    NetworkWorker *m_networkWorker;
    NetworkModuleWidget *m_networkWidget;
};

} // namespace network
} // namespace dcc

#endif // NETWORKMODULE_H
