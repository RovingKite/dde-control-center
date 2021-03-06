#include "connectionwirelesseditpage.h"
#include "settings/wirelesssettings.h"

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelesssecuritysetting.h>

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionWirelessEditPage::ConnectionWirelessEditPage(const QString &devPath, const QString &connUuid, QWidget *parent)
    : ConnectionEditPageNew(ConnectionEditPageNew::ConnectionType::WirelessConnection, devPath, connUuid, parent)
{
}

ConnectionWirelessEditPage::~ConnectionWirelessEditPage()
{
}

void ConnectionWirelessEditPage::initSettingsWidgetFromAp(const QString &apPath)
{
    qDebug() << "create wireless connection from ap:" << apPath;

    NetworkManager::AccessPoint::Ptr nmAp = QSharedPointer<AccessPoint>(new NetworkManager::AccessPoint(apPath));

    if (!m_connectionSettings) {
        qDebug() << "ConnectionSettings of base class is invalid..." << m_connectionSettings;
        return;
    }

    if (!nmAp) {
        qDebug() << "AccessPoint path is invalid..." << apPath;
        return;
    }

    // init connection setting from data of given ap
    m_connectionSettings->setId(nmAp->ssid());

    initApSecretType(nmAp);

    m_connectionSettings->setting(NetworkManager::Setting::SettingType::Wireless)
        .staticCast<NetworkManager::WirelessSetting>()->setSsid(nmAp->rawSsid());

    m_settingsWidget = new WirelessSettings(m_connectionSettings, this);

    connect(m_settingsWidget, &WirelessSettings::requestNextPage, this, &ConnectionEditPageNew::requestNextPage);
    m_settingsLayout->addWidget(m_settingsWidget);
}

void ConnectionWirelessEditPage::initApSecretType(NetworkManager::AccessPoint::Ptr nmAp)
{
    NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySetting;
    wirelessSecuritySetting = m_connectionSettings->setting(NetworkManager::Setting::SettingType::WirelessSecurity)
        .staticCast<NetworkManager::WirelessSecuritySetting>();

    NetworkManager::AccessPoint::Capabilities capabilities = nmAp->capabilities();
    NetworkManager::AccessPoint::WpaFlags wpaFlags = nmAp->wpaFlags();
    NetworkManager::AccessPoint::WpaFlags rsnFlags = nmAp->rsnFlags();

    NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone;

    if (capabilities.testFlag(NetworkManager::AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep;
    }

    if (wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk;
    }

    if (wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap;
    }

    wirelessSecuritySetting->setKeyMgmt(keyMgmt);
}
