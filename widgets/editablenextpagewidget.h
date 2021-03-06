/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef EDITABLENEXTPAGEWIDGET_H
#define EDITABLENEXTPAGEWIDGET_H

#include "settingsitem.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

class EditableNextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit EditableNextPageWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

signals:
    void acceptNextPage() const;
    void selected() const;
    void textChanged(const QString &oldText, const QString &newText) const;

private:
    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void toEditMode();
    void toEchoMode();

private:
    QLabel *m_name;
    QLabel *m_selectIcon;
    QLineEdit *m_edit;
    DImageButton *m_editButton;
    DImageButton *m_nextButton;
};

}

}

#endif // EDITABLENEXTPAGEWIDGET_H
