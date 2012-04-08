/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include "shortcuts.hxx"

#include <QtGui/QAction>
#include <QtGui/QMessageBox>

Shortcuts* Shortcuts::instance = 0;

Shortcuts::Shortcuts()
{

}

Shortcuts* Shortcuts::getInstance()
{
    if (instance == 0)
    {
       QMessageBox msgBox;
       msgBox.setText("Some function wanted the instance of the\nShortcuts class what is nonexistent at the moment.\nThis is probably a bug.");
       msgBox.setWindowTitle("Shortcut instance not found!");
#if OGRE_PLATFORM != OGRE_PLATFORM_LINUX
       msgBox.setIcon(QMessageBox::Critical);
#endif
       msgBox.exec();
    }

    return instance;
}

void Shortcuts::add(const QString &shortcut, const QString &description, const QString &SPK, const int &keycode)
{
    if (actions.contains(cleanName(SPK)))
        return;

    ShortCut tmpShortCut;
    tmpShortCut.shortcut = shortcut;
    tmpShortCut.description = description;
    tmpShortCut.SPK = SPK;
    tmpShortCut.keycode = keycode;
    actions[cleanName(SPK)] = tmpShortCut;
    QString key = settingsKey(tmpShortCut);
    if (!settings.contains(key))
    {
        settings.setValue(settingsKey(actions[cleanName(SPK)]), shortcut);
        settings.setValue(settingsKey(actions[cleanName(SPK)] ) + QString("_description"), description);
        settings.setValue(settingsKey(actions[cleanName(SPK)] ) + QString("_SPK"), SPK);
        settings.setValue(settingsKey(actions[cleanName(SPK)] ) + QString("_keycode"), keycode);

        settings.setValue(QString("orig_") + settingsKey(actions[cleanName(SPK)]), shortcut);
        settings.setValue(QString("orig_") + settingsKey(actions[cleanName(SPK)] ) + QString("_description"), description);
        settings.setValue(QString("orig_") + settingsKey(actions[cleanName(SPK)] ) + QString("_SPK"), SPK);
        settings.setValue(QString("orig_") + settingsKey(actions[cleanName(SPK)] ) + QString("_keycode"), keycode);
    }
}

QList<ShortCut> Shortcuts::getActions()
{
    return actions.values();
}

QString Shortcuts::settingsKey(ShortCut &shortcut)
{
    return QString("OgitorShortcuts/%1").arg(cleanName(shortcut));
}

QString Shortcuts::settingsKey(const QString &shortcut)
{
    return QString("OgitorShortcuts/%1").arg(cleanName(shortcut));
}

QString Shortcuts::cleanName(const ShortCut &shortcut)
{
    return cleanName(shortcut.SPK);
}

QString Shortcuts::cleanName(const QString &s)
{

    QString h = s;
    return h.remove("&");
}

QString Shortcuts::isReserved(const QString &shortcut, const QString &SPK)
{
    QString isTaken = QString::null;
    if (actions.contains(cleanName(SPK))) {
        QList<ShortCut> alist = actions.values();
        foreach(ShortCut act, alist) {
            if (act.shortcut == shortcut) {
                isTaken = act.shortcut;
                break;
            }
        }
    }
    return isTaken;
}

void Shortcuts::setShortcut(const QString &shortcut, const QString &description, const QString &SPK, const int &keycode)
{
    if (actions.contains(cleanName(SPK)))
    {
        actions[cleanName(SPK)].shortcut = shortcut;
        actions[cleanName(SPK)].description = description;
        actions[cleanName(SPK)].keycode = keycode;
        settings.setValue(settingsKey(actions[cleanName(SPK)]), shortcut);
        settings.setValue(settingsKey(actions[cleanName(SPK)] ) + QString("_description"), description);
        settings.setValue(settingsKey(actions[cleanName(SPK)] ) + QString("_keycode"), keycode);
    }
}

void Shortcuts::clearShortcut(const QString &SPK)
{
    if (actions.contains(cleanName(SPK)))
    {
        QString shortcut = settings.value(QString("orig_") + settingsKey(actions[cleanName(SPK)]), "").toString();
        int keycode = settings.value(QString("orig_") + settingsKey(actions[cleanName(SPK)]) + QString("_keycode"), 0).toInt();
        actions[cleanName(SPK)].shortcut = shortcut;
        actions[cleanName(SPK)].keycode = keycode;
    }
}

void Shortcuts::removeShortcut(const QString &SPK)
{
    settings.remove(settingsKey(actions[cleanName(SPK)]));
}

void Shortcuts::load()
{
    QString shortcut = settings.value(settingsKey("SPK_FORWARD"), "W").toString();
    QString description = tr("Move Forwards");
    QString SPK = settings.value(settingsKey("SPK_FORWARD_SPK"), "SPK_FORWARD").toString();
    int keycode = settings.value(settingsKey("SPK_FORWARD_keycode"), Qt::Key_W).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_LEFT"), "A").toString();
    description = tr("Move Left");
    SPK = settings.value(settingsKey("SPK_LEFT_SPK"), "SPK_LEFT").toString();
    keycode = settings.value(settingsKey("SPK_LEFT_keycode"), Qt::Key_A).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_BACKWARD"), "S").toString();
    description = tr("Move Backwards");
    SPK = settings.value(settingsKey("SPK_BACKWARD_SPK"), "SPK_BACKWARD").toString();
    keycode = settings.value(settingsKey("SPK_BACKWARD_keycode"), Qt::Key_S).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_RIGHT"), "D").toString();
    description = tr("Move Right");
    SPK = settings.value(settingsKey("SPK_RIGHT_SPK"), "SPK_RIGHT").toString();
    keycode = settings.value(settingsKey("SPK_RIGHT_keycode"), Qt::Key_D).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_UP"), "E").toString();
    description = tr("Move Up");
    SPK = settings.value(settingsKey("SPK_UP_SPK"), "SPK_UP").toString();
    keycode = settings.value(settingsKey("SPK_UP_keycode"), Qt::Key_E).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_DOWN"), "Q").toString();
    description = tr("Move Down");
    SPK = settings.value(settingsKey("SPK_DOWN_SPK"), "SPK_DOWN").toString();
    keycode = settings.value(settingsKey("SPK_DOWN_keycode"), Qt::Key_Q).toInt();
    add(shortcut, description, SPK, keycode);
    
    shortcut = settings.value(settingsKey("SPK_OBJECT_CENTERED_MOVEMENT"), "Alt").toString();
    description = tr("Object Centered Movement Modifier Key");
    SPK = settings.value(settingsKey("SPK_OBJECT_CENTERED_MOVEMENT_SPK"), "SPK_OBJECT_CENTERED_MOVEMENT").toString();
    keycode = settings.value(settingsKey("SPK_OBJECT_CENTERED_MOVEMENT_keycode"), (Qt::Key_Alt & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_FOCUS_OBJECT"), "F").toString();
    description = tr("Focus Object");
    SPK = settings.value(settingsKey("SPK_FOCUS_OBJECT_SPK"), "SPK_FOCUS_OBJECT").toString();
    keycode = settings.value(settingsKey("SPK_FOCUS_OBJECT_keycode"), Qt::Key_F).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_CLONE"), "Shift").toString();
    description = tr("Clone Modifier");
    SPK = settings.value(settingsKey("SPK_CLONE_SPK"), "SPK_CLONE").toString();
    keycode = settings.value(settingsKey("SPK_CLONE_keycode"), (Qt::Key_Shift & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_DELETE"), "Delete").toString();
    description = tr("Delete Selected");
    SPK = settings.value(settingsKey("SPK_DELETE_SPK"), "SPK_DELETE").toString();
    keycode = settings.value(settingsKey("SPK_DELETE_keycode"), (Qt::Key_Delete & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_REVERSE_UPDATE"), "Shift").toString();
    description = tr("Reverse Update Modifier");
    SPK = settings.value(settingsKey("SPK_REVERSE_UPDATE_SPK"), "SPK_REVERSE_UPDATE").toString();
    keycode = settings.value(settingsKey("SPK_REVERSE_UPDATE_keycode"), (Qt::Key_Shift & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_SWITCH_AXIS"), "End").toString();
    description = tr("Switch Locked Axis");
    SPK = settings.value(settingsKey("SPK_SWITCH_AXIS_SPK"), "SPK_SWITCH_AXIS").toString();
    keycode = settings.value(settingsKey("SPK_SWITCH_AXIS_keycode"), (Qt::Key_End & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_SNAP"), "Ctrl").toString();
    description = tr("Snap to Grid Modifier");
    SPK = settings.value(settingsKey("SPK_SNAP_SPK"), "SPK_SNAP").toString();
    keycode = settings.value(settingsKey("SPK_SNAP_keycode"), (Qt::Key_Control & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_SNAP_GROUND"), "Space").toString();
    description = tr("Snap to Ground Modifier");
    SPK = settings.value(settingsKey("SPK_SNAP_GROUND_SPK"), "SPK_SNAP_GROUND").toString();
    keycode = settings.value(settingsKey("SPK_SNAP_GROUND_keycode"), Qt::Key_Space).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_ADD_TO_SELECTION"), "Shift").toString();
    description = tr("Add to Selection Modifier");
    SPK = settings.value(settingsKey("SPK_ADD_TO_SELECTION_SPK"), "SPK_ADD_TO_SELECTION").toString();
    keycode = settings.value(settingsKey("SPK_ADD_TO_SELECTION_keycode"), (Qt::Key_Shift & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_SUBTRACT_FROM_SELECTION"), "Ctrl").toString();
    description = tr("Subtract from Selection Modifier");
    SPK = settings.value(settingsKey("SPK_SUBTRACT_FROM_SELECTION_SPK"), "SPK_SUBTRACT_FROM_SELECTION").toString();
    keycode = settings.value(settingsKey("SPK_SUBTRACT_FROM_SELECTION_keycode"), (Qt::Key_Control & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);

    shortcut = settings.value(settingsKey("SPK_ALWAYS_SELECT"), "Alt").toString();
    description = tr("Always Select Modifier");
    SPK = settings.value(settingsKey("SPK_ALWAYS_SELECT_SPK"), "SPK_ALWAYS_SELECT").toString();
    keycode = settings.value(settingsKey("SPK_ALWAYS_SELECT_keycode"), (Qt::Key_Alt & 0xFFF) + 0xFF).toInt();
    add(shortcut, description, SPK, keycode);
}

Shortcuts::~Shortcuts()
{
    if (instance != 0)
        delete instance;
}
