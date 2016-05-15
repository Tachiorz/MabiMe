#include "packmanager.h"

#include <QSettings>
#include <QDebug>
#include <QDir>

PackManager::PackManager()
{
    if (!findMabinogiPath()) {
        qDebug() << "could not find path";
    } else {
        qDebug() << path;
    }
}

PackManager::PackManager(QString path) {
    // load from path
    if (QDir(path).exists()) {
        this->path = path;
    }
}

QString PackManager::getPath() {
    return path;
}

#if defined(Q_OS_WIN)
bool PackManager::findMabinogiPath() {
    QSettings m("HKEY_CURRENT_USER\\Software\\Nexon\\Mabinogi", QSettings::NativeFormat); // get the default path, not used in nexon launcher installs
    QString p = m.value("Default","").toString();
    if (p.length() == 0) { // failed first check, check directplay location for 64bit
        QSettings m("HdKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\DirectPlay8\\Applications\\", QSettings::NativeFormat);
        foreach (const QString &group, m.childGroups()) {
            m.beginGroup(group);
            if (m.value("ExecutableFilename", "").toString().compare("mabinogi.exe", Qt::CaseInsensitive) == 0) {
                p = m.value("ExecutablePath", "").toString();
                m.endGroup();
                break;
            }
            m.endGroup();
        }
        if (p.length() == 0) { // failed again, try 32 bit
            QSettings m("HKdEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DirectPlay8\\Applications\\", QSettings::NativeFormat);
            foreach (const QString &group, m.childGroups()) {
                m.beginGroup(group);
                if (m.value("ExecutableFilename", "").toString().compare("mabinogi.exe", Qt::CaseInsensitive) == 0) {
                    p = m.value("ExecutablePath", "").toString();
                    m.endGroup();
                    break;
                }
                m.endGroup();
            }

        }
    }
    if (p.length() == 0) { // failed directplay locations, look at compat flags
        QSettings m("HKEY_USERS", QSettings::NativeFormat);
        foreach (const QString &group, m.childGroups()) {
            QSettings m2("HKEY_USERS\\" + group + "\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store", QSettings::NativeFormat);
            foreach (const QString &key, m2.childKeys()) {
                if (key.endsWith("mabinogi.exe", Qt::CaseInsensitive)) {
                    p = key;
                    p.remove("mabinogi.exe", Qt::CaseInsensitive);
                    break;
                }
            }
            if (p.length() > 0) break;
        }
    }
    if (p.length() == 0) return false;
    p.replace("\\", "/");
    p = p.endsWith("/") ? p + "package/" : p + "/package/";
    if (QDir(p).exists()) {
        path = p;
        return true;
    } else {
        return false;
    }
}
#elif defined (Q_OS_LINUX)
bool PackManager::findMabinogiPath() {
    return false;
}
#endif

bool PackManager::loadPackages() {
    QDir f(path);
    // search for packs if the dir exists
    if (f.exists()) {
        QStringList packList = f.entryList(QStringList("*.pack"), QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
        QStringList sortedPacks;
        // sort the packs from lowest to highest order, important for later
        int n = 0;
        while (true) {
            foreach (const QString &packName, packList) {
                if (packName.startsWith(QString::number(n) + "_")) {
                    sortedPacks.insert(0, packName);
                }
            }
            if (sortedPacks.count() == packList.count() - 1) break;
            n++;
            if (n > 50000) {
                qDebug() << "WARNING: COULD NOT FIND ALL PACKS";
                break;
            }
        }
        int id = 0;
        foreach (const QString &packName, sortedPacks) {
            Pack *p = new Pack();
            p->id = id;
            p->name = packName;
            p->reader = new MabiPackReader();
            p->reader->openPackage(path + packName);
            id++;
            packs.append(p);
        }
        return true;
    } else {
        qDebug() << path << "does not exist!";
    }
    return false;
}

PackManager::~PackManager() {
    freePackages();
}

void PackManager::freePackages() {
    foreach (Pack *p, packs) {
        p->reader->closePackage();
        delete p->reader;
        delete p;
    }
    packs.clear();
}

QString PackManager::findTexture(QString name) {
    QString packName;
    foreach (const Pack *pack, packs) {
        packName = pack->reader->findTexture(name);
        if (packName.length() > 0) return packName;
    }

}

QByteArray PackManager::extractFile(QString path) {
    QByteArray f;
    foreach (const Pack *pack, packs) {
        f = pack->reader->extractFile(path);
        if (f.length() > 0) {
            qDebug() << pack << pack->name;
            return f;
        }
    }
    return f;
}
