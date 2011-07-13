/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/
#ifndef __shortcuts_hxx__
#define __shortcuts_hxx__

#include <QtCore/QSettings>
#include <QtCore/QMap>

struct ShortCut
{
    QString SPK;
    QString shortcut;
    QString description;
    int keycode;
};

class Shortcuts : public QObject
{
    Q_OBJECT
public:
    Shortcuts();
    ~Shortcuts();

    static Shortcuts* getInstance();

    void load();

    void add(const QString &shortcut, const QString &description, const QString &SPK, const int &keycode);

    QList<ShortCut> getActions();

    /* returns QString::null if it's not reserved or the action name
      the shortcut belongs if it's already taken. */
    QString isReserved(const QString &shortcut, const QString &SPK);

    void setShortcut(const QString &shortcut, const QString &description, const QString &SPK, const int &keycode);

    void clearShortcut(const QString &SPK);
    void removeShortcut(const QString &SPK);

    QString cleanName(const ShortCut &shortcut); // without & chars
    QString cleanName(const QString &s);  // same with a string

private:
    QSettings settings;

    QMap<QString, ShortCut> actions;

    static Shortcuts* instance;

    QString settingsKey(ShortCut &shortcut);
    QString settingsKey(const QString &shortcut);

};

#endif // __shortcuts_hxx__
