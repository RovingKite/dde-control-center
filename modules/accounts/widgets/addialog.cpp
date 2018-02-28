/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "addialog.h"

#include <QImageReader>
#include <QVBoxLayout>
#include <DThemeManager>
#include <QDebug>

using namespace dcc;
using namespace dcc::accounts;

ADDialog::ADDialog(QWidget *parent)
    : DDialog(parent)
    , m_server(new QLineEdit)
    , m_username(new QLineEdit)
    , m_password(new DPasswordEdit)
{
    initUI();
}

void ADDialog::setUserModel(UserModel *model)
{
    m_model = model;

#ifdef DCC_ENABLE_ADDOMAIN
    addButton(model->isJoinADDomain() ? tr("Exit domain") : tr("Join domain"), true);
#endif
}

void ADDialog::initUI()
{
    QPixmap pixmap;
    QImageReader reader;
    reader.setFileName(":/accounts/themes/dark/icons/default_avatar.png");
    if (reader.canRead()) {
        reader.setScaledSize(QSize(64, 64));
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(devicePixelRatio());
    }
    setIcon(QIcon(pixmap));

    m_server->setPlaceholderText(tr("Domain Server"));
    m_username->setPlaceholderText(tr("Admin account"));
    m_password->setPlaceholderText(tr("Password"));

    m_server->setClearButtonEnabled(true);
    m_username->setClearButtonEnabled(true);

    addContent(m_server);
    addContent(m_username);
    addContent(m_password);

    addButton(tr("Cancel"));

    setSpacing(10);

    connect(this, &ADDialog::buttonClicked, this, [=] (int index, const QString &text) {
        Q_UNUSED(text);

        switch (index) {
        case Submit:
            emit requestInfos(m_server->text(), m_username->text(), m_password->text());
            break;
        default:
            break;
        }
    });
}