/*
 * Copyright (C) 2015 by Emmanuel Pescosta <emmanuelpescosta099@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License (LGPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KIO_ADB_H
#define KIO_ADB_H

#include <KIO/Global>
#include <KIO/SlaveBase>

class QUrl;
class QString;
class QDateTime;
class QByteArray;

class AdbProtocol : public KIO::SlaveBase
{
public:
    AdbProtocol(const QByteArray &pool, const QByteArray &app);
    virtual ~AdbProtocol();

    void stat(const QUrl &url) override;
    void get(const QUrl &url) override;
    void put(const QUrl &url, int mode, KIO::JobFlags flags) override;
    void copy(const QUrl &src, const QUrl &dest, int mode, KIO::JobFlags flags) override;

    void rename(const QUrl &src, const QUrl &dest, KIO::JobFlags flags) override;
    void symlink(const QString &target, const QUrl &dest, KIO::JobFlags flags) override;
    void chmod(const QUrl &url, int permissions) override;
    void chown(const QUrl &url, const QString &owner, const QString &group) override;
    void setModificationTime(const QUrl &url, const QDateTime &mtime) override;
    void del(const QUrl &url, bool isfile) override;

    void listDir(const QUrl &url) override;
    void mkdir(const QUrl &url, int permissions) override;

protected:
    void virtual_hook(int id, void *data) override;

private:
    void fileSystemFreeSpace(const QUrl &url);
    void listDevices();
};

#endif // KIO_ADB_H
