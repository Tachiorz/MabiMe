/*
 MabiMe Character Simulator - by Yai (Sophie N)
 Email: sinoc300@gmail.com
 Copyright (C) 2016

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PACKXMLMANAGER_H
#define PACKXMLMANAGER_H

#include <QString>
#include <QMap>
#include <QDomDocument>

#include "PackReader/mabipackreader.h"
#include "Parsers/localemaphelper.h"

class XMLParser
{
public:
    XMLParser(QString name, QByteArray xml, LocaleMapHelper *localeMap = nullptr);
    bool isReady();
protected:
    QDomDocument doc;
    QString name;
    LocaleMapHelper *localeMap = nullptr;
    virtual void parseFile() = 0;
private:
    bool ready = false;
};

#endif // PACKXMLMANAGER_H
