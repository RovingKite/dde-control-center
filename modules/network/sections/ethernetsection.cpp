#include "ethernetsection.h"
#include "contentwidget.h"

#include <dspinbox.h>

#include <networkmanagerqt/manager.h>
#include <networkmanagerqt/wireddevice.h>

using namespace dcc::widgets;
using namespace NetworkManager;

EthernetSection::EthernetSection(NetworkManager::WiredSetting::Ptr wiredSetting, QFrame *parent)
    : AbstractSection("Ethernet", parent),
      m_deviceMac(new ComboBoxWidget(this)),
      m_clonedMac(new LineEditWidget(this)),
      m_customMtuSwitch(new SwitchWidget(this)),
      m_customMtu(new SpinBoxWidget(this)),
      m_wiredSetting(wiredSetting)
{
    for (auto device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Ethernet) {
            continue;
        }
        NetworkManager::WiredDevice::Ptr wDevice = device.staticCast<NetworkManager::WiredDevice>();
        /* Alt:  permanentHardwareAddress to get real hardware address which is connot be changed */
        const QString &macStr = wDevice->hardwareAddress() + " (" + wDevice->interfaceName() + ")";
        m_macStrMap.insert(macStr, wDevice->hardwareAddress().remove(":"));
    }

    // "^([0-9A-Fa-f]{2}[:-\\.]){5}([0-9A-Fa-f]{2})$"
    m_macAddrRegExp = QRegExp("^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$");

    initUI();
    initConnection();
}

EthernetSection::~EthernetSection()
{
}

bool EthernetSection::allInputValid()
{
    const QString &clonedMacStr = m_clonedMac->text();
    if (clonedMacStr.isEmpty()) {
        return true;
    }
    bool matched = m_macAddrRegExp.exactMatch(clonedMacStr);
    m_clonedMac->setIsErr(!matched);
    return matched;
}

void EthernetSection::saveSettings()
{
    QString hwAddr = m_macStrMap.value(m_deviceMac->value());
    m_wiredSetting->setMacAddress(QByteArray::fromHex(hwAddr.toUtf8()));

    QString clonedAddr = m_clonedMac->text().remove(":");
    m_wiredSetting->setClonedMacAddress(QByteArray::fromHex(clonedAddr.toUtf8()));

    m_wiredSetting->setMtu(m_customMtuSwitch->checked() ? m_customMtu->spinBox()->value() : 0);

    m_wiredSetting->setInitialized(true);
}

void EthernetSection::initUI()
{
    m_deviceMac->setTitle(tr("Device MAC Addr"));
    for (const QString &key : m_macStrMap.keys()) {
        m_deviceMac->appendOption(key, m_macStrMap.value(key));
    }

    const QString &macAddr = QString(m_wiredSetting->macAddress().toHex()).toUpper();
    if (m_macStrMap.values().contains(macAddr)) {
        m_deviceMac->setCurrent(macAddr);
    } else {
        m_deviceMac->setCurrent(m_macStrMap.first());
    }

    m_clonedMac->setTitle(tr("Cloned MAC Addr"));
    QString tmp = QString(m_wiredSetting->clonedMacAddress().toHex()).toUpper();
    QString clonedMacAddr;
    if (!tmp.isEmpty()) {
        for (int i = 0; i < tmp.size(); ++i) {
            if (i != 0 && i % 2 == 0) {
                clonedMacAddr.append(":");
            }
            clonedMacAddr.append(tmp.at(i));
        }
    }
    m_clonedMac->setText(clonedMacAddr);

    m_customMtuSwitch->setTitle(tr("Customize MTU"));
    m_customMtuSwitch->setChecked(!(m_wiredSetting->mtu() == 0));

    m_customMtu->setTitle(tr("MTU"));
    m_customMtu->spinBox()->setValue(m_wiredSetting->mtu());
    onCostomMtuChanged(m_customMtuSwitch->checked());

    appendItem(m_deviceMac);
    appendItem(m_clonedMac);
    appendItem(m_customMtuSwitch);
    appendItem(m_customMtu);
}

void EthernetSection::initConnection()
{
    connect(m_deviceMac, &ComboBoxWidget::requestPage, this, &EthernetSection::requestPage);
    connect(m_clonedMac->textEdit(), &QLineEdit::editingFinished, this, &EthernetSection::allInputValid);
    connect(m_customMtuSwitch, &SwitchWidget::checkedChanged, this, &EthernetSection::onCostomMtuChanged);
}

void EthernetSection::onCostomMtuChanged(const bool enable)
{
    m_customMtu->setVisible(enable);
}
