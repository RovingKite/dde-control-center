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

#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include <com_deepin_daemon_mime.h>
#include <QObject>
using com::deepin::daemon::Mime;

namespace dcc
{
namespace defapp
{
class DefAppModel;
class Category;
class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);
    void active();
    void deactive();

public slots:
    void onSetDefaultApp(const QString &category, const QJsonObject &item);
    void onGetDefaultApp();
    void onGetListApps();
    void onResetTriggered();
    void onDelUserApp(const QString &mine, const QJsonObject &item);
    void onCreateFile(const QString &mime, const QFileInfo &info);

private slots:
    void getListAppFinished(QDBusPendingCallWatcher *w);
    void getUserAppFinished(QDBusPendingCallWatcher *w);
    void getDefaultAppFinished(QDBusPendingCallWatcher *w);
    void saveListApp(const QString &mime, const QJsonArray &json);
    void saveUserApp(const QString &mime, const QJsonArray &json);
    void saveDefaultApp(const QString &mime, const QJsonObject &app);

private:
    DefAppModel *m_defAppModel;
    Mime     *m_dbusManager;

    enum DefaultAppsCategory {
        Browser, Mail, Text, Music, Video, Picture, Terminal
    };

    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QString m_userLocalPath;

private:
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);
    Category* getCategory(const QString &mime) const;
};
}
}

#endif // DEFAPPWORKER_H
