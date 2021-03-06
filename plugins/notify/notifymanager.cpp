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

#include "notifymanager.h"
#include <QDebug>
#include <QScrollArea>
#include <QScrollBar>

NotifyManager::NotifyManager(QWidget *parent) :
    QWidget(parent),
    m_dbus(new Notification("com.deepin.dde.Notification", "/com/deepin/dde/Notification", QDBusConnection::sessionBus(), this))
{
    QWidget *widget = new QWidget;

    m_connectLayout = new QVBoxLayout(widget);
    m_connectLayout->setMargin(0);
    m_connectLayout->setSpacing(1);
    m_connectLayout->addStretch();

    QScrollArea *scrollarea = new QScrollArea;
    scrollarea->setWidget(widget);
    scrollarea->setObjectName("scrollarea");
    scrollarea->setWidgetResizable(true);
    scrollarea->setFocusPolicy(Qt::NoFocus);
    scrollarea->setFrameStyle(QFrame::NoFrame);
    scrollarea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    scrollarea->setContentsMargins(0, 0, 0, 0);
    scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setStyleSheet("background-color:transparent;");

    QScrollBar *bar = scrollarea->verticalScrollBar();
    connect(bar, &QScrollBar::valueChanged, this, [=](int value){
            if (m_checkIndex && value == bar->maximum())
                onLoadAgain();
    });

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    m_clearButton = new DImageButton;
    m_clearButton->setText(tr("Clear all"));
    m_clearButton->setStyleSheet("padding: 4px 0;");

    mainLayout->addWidget(m_clearButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(scrollarea);

    setLayout(mainLayout);

    m_clearButton->setVisible(false);

    connect(m_clearButton, &DImageButton::clicked, this, &NotifyManager::onCloseAllItem);
    connect(m_dbus, &Notification::RecordAdded, this, &NotifyManager::onNotifyAdded);

    m_dbus->setSync(false);

    QDBusPendingReply<QString> notify = m_dbus->GetAllRecords();
    QDBusPendingCallWatcher *notifyWatcher = new QDBusPendingCallWatcher(notify, this);
    connect(notifyWatcher, &QDBusPendingCallWatcher::finished, this, &NotifyManager::onNotifyGetAllFinished);
}

NotifyManager::~NotifyManager() {

}

void NotifyManager::onNotifyAdded(const QString &value)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(value.toLocal8Bit().data());
    Viewer* viewer = onNotifyAdd(jsonDocument.object());

    m_connectLayout->insertWidget(0, viewer);
}

Viewer *NotifyManager::onNotifyAdd(const QJsonObject &value) {
    m_clearButton->setVisible(true);

    Viewer* viewer = new Viewer(value, this);

    viewer->setFixedHeight(80);
    viewer->setContentsMargins(0, 0, 0, 0);

    viewer->adjustSize();

    m_viewerList.insert(viewer, value);
    connect(viewer, &Viewer::requestClose, this, &NotifyManager::onNotifyRemove);
    update();

    return viewer;
}

void NotifyManager::onNotifyRemove(const QString &id)
{
    // main key is time;

    Viewer *viewer = qobject_cast<Viewer*>(sender());

    Q_ASSERT(viewer);

    m_connectLayout->removeWidget(viewer);
    m_viewerList.remove(viewer);
    m_dbus->RemoveRecord(id);

    viewer->deleteLater();

    update();
}

void NotifyManager::onLoadAgain()
{
    if (m_checkIndex) {
        for (int i = 0; i < 20; i++) {
            if (--m_checkIndex < 1)
                return;
            Viewer* viewer = onNotifyAdd(m_dataJsonArray.at(m_checkIndex).toObject());
            m_connectLayout->insertWidget(m_viewerList.size(), viewer);
        }
    }
}

void NotifyManager::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(255, 255, 255, 255 * 0.03));

    if (m_viewerList.isEmpty()) {
        QString s(tr("No system notifications"));
        painter.drawText(rect(), Qt::AlignCenter, s);
        m_clearButton->setVisible(false);
    }

    QWidget::paintEvent(e);
}

void NotifyManager::onNotifyGetAllFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply.value().toLocal8Bit().data());

    m_dataJsonArray = jsonDocument.array();
    qDebug() << "====================================" << m_dataJsonArray;

    m_checkIndex = m_dataJsonArray.size();

    if (m_checkIndex) {
        if (m_checkIndex < 20) {
            for (const QJsonValue &value : m_dataJsonArray) {
                Viewer* viewer = onNotifyAdd(value.toObject());
                m_connectLayout->insertWidget(0, viewer);
            }
            m_checkIndex = 0;
        } else {
            for (int i = 20; i > 0; --i) {
                if (m_checkIndex - i < 1)
                    return;
                Viewer* viewer = onNotifyAdd(m_dataJsonArray.at(m_checkIndex - i).toObject());
                m_connectLayout->insertWidget(0, viewer);
            }
            m_checkIndex = m_dataJsonArray.size() - 20;
        }
    }
    w->deleteLater();
}

void NotifyManager::onCloseAllItem()
{
    for (Viewer *viewer : m_viewerList.keys()) {
        viewer->onClose();
    }

    m_viewerList.clear();
    m_checkIndex = 0;
    m_dbus->ClearRecords();
}
