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

#include "kio_adb.h"

#include "kio_adb_debug.h"

#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QByteArray>

using namespace KIO;

extern "C" Q_DECL_EXPORT int kdemain(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: kio_adb protocol domain-socket1 domain-socket2\n");
        exit(-1);
    }

    AdbProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();

    return 0;
}

AdbProtocol::AdbProtocol(const QByteArray &pool, const QByteArray &app) :
    KIO::SlaveBase(QByteArrayLiteral("adb"), pool, app)
{

}

AdbProtocol::~AdbProtocol()
{

}

void AdbProtocol::stat(const QUrl &url)
{
    qCDebug(KIO_ADB) << "stat:" << url;

    finished();
}

void AdbProtocol::get(const QUrl &url)
{
    qCDebug(KIO_ADB) << "get:" << url;

    finished();
}

void AdbProtocol::put(const QUrl &url, int mode, KIO::JobFlags flags)
{
    qCDebug(KIO_ADB) << "put:" << url << mode << flags;

    finished();
}

void AdbProtocol::copy(const QUrl &src, const QUrl &dest, int mode, KIO::JobFlags flags)
{
    qCDebug(KIO_ADB) << "copy:" << src << dest << mode << flags;

    finished();
}

void AdbProtocol::rename(const QUrl &src, const QUrl &dest, KIO::JobFlags flags)
{
    qCDebug(KIO_ADB) << "rename:" << src << dest << flags;

    finished();
}

void AdbProtocol::symlink(const QString &target, const QUrl &dest, KIO::JobFlags flags)
{
    qCDebug(KIO_ADB) << "symlink:" << target << dest << flags;

    finished();
}

void AdbProtocol::chmod(const QUrl &url, int permissions)
{
    qCDebug(KIO_ADB) << "chmod:" << url << permissions;

    finished();
}

void AdbProtocol::chown(const QUrl &url, const QString &owner, const QString &group)
{
    qCDebug(KIO_ADB) << "chown:" << url << owner << group;

    finished();
}

void AdbProtocol::setModificationTime(const QUrl &url, const QDateTime &mtime)
{
    qCDebug(KIO_ADB) << "setModificationTime:" << url << mtime;

    finished();
}

void AdbProtocol::del(const QUrl &url, bool isfile)
{
    qCDebug(KIO_ADB) << "del:" << url << isfile;

    finished();
}

void AdbProtocol::listDir(const QUrl &url)
{
    qCDebug(KIO_ADB) << "listDir:" << url;

    finished();
}

void AdbProtocol::mkdir(const QUrl &url, int permissions)
{
    qCDebug(KIO_ADB) << "mkdir:" << url << permissions;

    finished();
}

void AdbProtocol::virtual_hook(int id, void *data)
{
    switch(id) {
        case SlaveBase::GetFileSystemFreeSpace: {
            QUrl *url = static_cast<QUrl *>(data);
            fileSystemFreeSpace(*url);
        }   break;
        default:
            SlaveBase::virtual_hook(id, data);
    }
}

void AdbProtocol::fileSystemFreeSpace(const QUrl &url)
{
    qCDebug(KIO_ADB) << "fileSystemFreeSpace:" << url;

    finished();
}
