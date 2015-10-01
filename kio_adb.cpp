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
#include <QProcess>
#include <QRegularExpression>

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

    const QString device = url.authority();
    const QString path = url.path();

    if (device.isEmpty()) {
        listDevices();
    } else if (path.isEmpty()) {
        QUrl targetUrl = url;
        targetUrl.setPath("/");
        redirection(targetUrl);

        finished();
    } else {
        QProcess process;
        process.setProgram(QLatin1String("adb"));
        process.setArguments({"-s", device, "shell", "ls", "-l", "-a", path});
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start();

        process.waitForFinished();
        const QString data = process.readAll();
        const QStringList lines = data.split(QLatin1Char('\n'), QString::SkipEmptyParts);

        static const QRegularExpression re("^(?<type>[\\-dlcbps])"
                                           "(?<permission>[\\-rwxsStT]{9})\\s+"
                                           "(?<owner>\\w+)\\s+"
                                           "(?<group>\\w+)\\s+"
                                           "((?<size>\\d+)\\s+)?"
                                           "(?<datetime>\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2})\\s+"
                                           "(?<name>.+)\\s$",
                                           QRegularExpression::OptimizeOnFirstUsageOption);

        foreach (const QString &line, lines) {
            const auto match = re.match(line); // TODO try to use globalMatch instead
            if (!match.hasMatch()) {
                continue;
            }

            const auto parsePermission = [](const QString &permissionStr) -> int {
                if (permissionStr.size() != 9) {
                    return 0;
                }

                const auto parsePermissionChar = [](char c, int r, int w, int x, int s) -> int {
                    switch (c) {
                        case 'r':
                            return r;
                        case 'w':
                            return w;
                        case 'x':
                            return x;
                        case 't':
                            return x | S_ISVTX;
                        case 'T':
                            return S_ISVTX;
                        case 's':
                            return x | s;
                        case 'S':
                            return s;
                        case '-':
                        default:
                            return 0;
                    }
                };

                int permissions = 0;

                permissions |= parsePermissionChar(permissionStr[0].toLatin1(), S_IRUSR, S_IWUSR, S_IXUSR, S_ISUID);
                permissions |= parsePermissionChar(permissionStr[1].toLatin1(), S_IRUSR, S_IWUSR, S_IXUSR, S_ISUID);
                permissions |= parsePermissionChar(permissionStr[2].toLatin1(), S_IRUSR, S_IWUSR, S_IXUSR, S_ISUID);

                permissions |= parsePermissionChar(permissionStr[3].toLatin1(), S_IRGRP, S_IWGRP, S_IXGRP, S_ISGID);
                permissions |= parsePermissionChar(permissionStr[4].toLatin1(), S_IRGRP, S_IWGRP, S_IXGRP, S_ISGID);
                permissions |= parsePermissionChar(permissionStr[5].toLatin1(), S_IRGRP, S_IWGRP, S_IXGRP, S_ISGID);

                permissions |= parsePermissionChar(permissionStr[6].toLatin1(), S_IROTH, S_IWOTH, S_IXOTH, 0);
                permissions |= parsePermissionChar(permissionStr[7].toLatin1(), S_IROTH, S_IWOTH, S_IXOTH, 0);
                permissions |= parsePermissionChar(permissionStr[8].toLatin1(), S_IROTH, S_IWOTH, S_IXOTH, 0);

                return permissions;
            };

            UDSEntry entry;

            const QDateTime modificationTime = QDateTime::fromString(match.captured("datetime"), "yyyy-MM-dd HH:mm");
            entry.insert(UDSEntry::UDS_MODIFICATION_TIME, modificationTime.toTime_t());

            entry.insert(UDSEntry::UDS_USER, match.captured("owner"));
            entry.insert(UDSEntry::UDS_GROUP, match.captured("group"));
            entry.insert(UDSEntry::UDS_SIZE, match.captured("size").toULongLong());
            entry.insert(UDSEntry::UDS_ACCESS, parsePermission(match.captured("permission")));
            entry.insert(UDSEntry::UDS_NAME, match.captured("name"));

            const QChar type = match.captured("type")[0];
            if (type == QLatin1Char('-')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFREG);
            } else if (type == QLatin1Char('d')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFDIR);
                entry.insert(UDSEntry::UDS_MIME_TYPE, QLatin1String("inode/directory"));
            } else if (type == QLatin1Char('l')) {
                const auto nameLink = match.captured("name");
                const auto name = nameLink.section(" -> ", 0, -2);
                const auto linkDest = nameLink.section(" -> ", -1);

                QUrl targetUrl = url;
                targetUrl.setPath(linkDest);

                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFDIR); // S_IFLNK
                entry.insert(UDSEntry::UDS_URL, targetUrl.url());
                entry.insert(UDSEntry::UDS_LINK_DEST, linkDest);
                entry.insert(UDSEntry::UDS_NAME, name);
            } else if (type == QLatin1Char('c')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFCHR);
            } else if (type == QLatin1Char('b')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFBLK);
            } else if (type == QLatin1Char('p')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFIFO);
            } else if (type == QLatin1Char('s')) {
                entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFSOCK);
            }

            listEntry(entry);
        }

        finished();
    }
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

void AdbProtocol::listDevices()
{
    qCDebug(KIO_ADB) << "listDevices";

    QProcess process;
    process.setProgram(QLatin1String("adb"));
    process.setArguments({"devices", "-l"});
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start();

    process.waitForFinished();
    const QString data = process.readAll();
    const QStringList lines = data.split(QLatin1Char('\n'), QString::SkipEmptyParts);

    static const QRegularExpression re("^(?<id>\\S+)\\s+"
                                       "device\\s+"
                                       "(usb:(?<usb>\\S+)\\s+)?"
                                       "product:(?<product>\\w+)\\s+"
                                       "model:(?<model>\\w+)\\s+"
                                       "device:(?<device>\\w+)$",
                                       QRegularExpression::OptimizeOnFirstUsageOption);

    foreach (const QString &line, lines) {
        const auto match = re.match(line); // TODO try to use globalMatch instead
        if (!match.hasMatch()) {
            continue;
        }

        QUrl url;
        url.setScheme(QLatin1String("adb"));
        url.setAuthority(match.captured("id"));
        url.setPath("/");

        UDSEntry entry;
        entry.insert(UDSEntry::UDS_URL, url.url());
        entry.insert(UDSEntry::UDS_NAME, match.captured("model"));
        entry.insert(UDSEntry::UDS_ICON_NAME, QLatin1String("smartphone"));
        entry.insert(UDSEntry::UDS_FILE_TYPE, S_IFDIR);
        entry.insert(UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
        entry.insert(UDSEntry::UDS_MIME_TYPE, QLatin1String("inode/directory"));

        listEntry(entry);
    }

    finished();
}
