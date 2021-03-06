#ifndef VPNPROXYSECTION_H
#define VPNPROXYSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnProxySection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnProxySection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnProxySection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onProxyTypeChanged(const QString &type);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    ComboBoxWidget *m_proxyTypeChooser;
    LineEditWidget *m_server;
    SpinBoxWidget *m_port;
    SwitchWidget *m_retry;
    LineEditWidget *m_userName;
    PasswdEditWidget *m_password;

    QString m_currentProxyType;
};

} /* widgets */
} /* dcc */

#endif /* VPNPROXYSECTION_H */
