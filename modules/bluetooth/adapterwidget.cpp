/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "adapterwidget.h"
#include "devicesettingsitem.h"
#include "translucentframe.h"
#include "settingsheaderitem.h"
#include "loadingindicator.h"
#include "switchwidget.h"
#include "settingsgroup.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;

namespace dcc {
namespace bluetooth {

AdapterWidget::AdapterWidget(const Adapter *adapter) :
    m_adapter(adapter),
    m_titleEdit(new TitleEdit),
    m_switch(new SwitchWidget(m_titleEdit)),
    m_titleGroup(new SettingsGroup),
    m_myDevicesGroup(new SettingsGroup(tr("My devices"))),
    m_otherDevicesGroup(new SettingsGroup(tr("Other devices")))
{
    setTitle(adapter->name());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    m_titleGroup->appendItem(m_switch);
    m_myDevicesGroup->setHeaderVisible(true);
    m_otherDevicesGroup->setHeaderVisible(true);

    m_refreshIndicator = new LoadingIndicator;
    m_refreshIndicator->setTheme("dark");
    m_otherDevicesGroup->headerItem()->setRightWidget(m_refreshIndicator);
    m_refreshIndicator->play();

    m_tip = new QLabel(tr("Enable bluetooth to find nearby devices (loudspeaker, keyboard, mouse)"));
    m_tip->setVisible(!m_switch->checked());
    m_tip->setWordWrap(true);
    m_tip->setContentsMargins(16, 0, 10, 0);

    layout->addSpacing(10);
    layout->addWidget(m_titleGroup);
    layout->addWidget(m_tip, 0, Qt::AlignTop);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_otherDevicesGroup);
    layout->addStretch();

    connect(m_switch, &SwitchWidget::checkedChanged, this, &AdapterWidget::toggleSwitch);
    connect(m_switch, &SwitchWidget::checkedChanged, [=](bool state) {
        m_tip->setVisible(!state);
    });
    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, [=](const QString &alias) {
        emit requestSetAlias(adapter, alias);
    });

    connect(this, &AdapterWidget::disappear, m_refreshIndicator, &DPictureSequenceView::stop);
    connect(this, &AdapterWidget::appear, m_refreshIndicator, &DPictureSequenceView::play);

    connect(adapter, &Adapter::nameChanged, this, &AdapterWidget::setTitle);
    connect(adapter, &Adapter::destroyed, this, &AdapterWidget::back);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);
    setContent(w);

    setAdapter(adapter);
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_titleEdit, &TitleEdit::setTitle, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::poweredChanged, m_switch, &SwitchWidget::setChecked, Qt::QueuedConnection);

    m_switch->blockSignals(true);
    m_titleEdit->setTitle(adapter->name());
    m_switch->setChecked(adapter->powered());
    m_tip->setVisible(!m_switch->checked());
    m_switch->blockSignals(false);

    blockSignals(true);
    toggleSwitch(adapter->powered());
    blockSignals(false);

    for (const Device *device : adapter->devices()) {
        addDevice(device);
    }
}

void AdapterWidget::toggleSwitch(const bool &checked)
{
    m_myDevicesGroup->setVisible(checked);
    m_otherDevicesGroup->setVisible(checked);

    emit requestSetToggleAdapter(m_adapter, checked);
}

void AdapterWidget::addDevice(const Device *device)
{
    DeviceSettingsItem *w = new DeviceSettingsItem(device);

    auto CategoryDevice = [this, w] (const bool paired) {
        if (paired) {
            m_myDevicesGroup->appendItem(w);
        } else {
            m_otherDevicesGroup->appendItem(w);
        }
    };
    CategoryDevice(device->paired());

    connect(w, &DeviceSettingsItem::requestConnectDevice, this, &AdapterWidget::requestConnectDevice);
    connect(device, &Device::pairedChanged, CategoryDevice);
    connect(w, &DeviceSettingsItem::requestShowDetail, [this] (const Device *device) {
        emit requestShowDetail(m_adapter, device);
    });

    m_deviceLists << w;
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    for (int i = 0; i != m_deviceLists.count(); ++i) {
        DeviceSettingsItem *item = m_deviceLists[i];
        if (item->device()->id() != deviceId) continue;
        m_myDevicesGroup->removeItem(item);
        m_deviceLists.removeAt(i);
        item->deleteLater();
        break;
    }
}

const Adapter *AdapterWidget::adapter() const
{
    return m_adapter;
}

}
}
