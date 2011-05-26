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
#ifndef USERDEFINEDDIALOG_HXX
#define USERDEFINEDDIALOG_HXX

#include <QtGui/QDialog>
#include <QtGui/QTableWidget>
#include "ui_userdefineddialog.h"
#include "qttreepropertybrowser.h"

namespace Ogitors
{
    class OgitorsCustomPropertySet;
    class OgitorsPropertyBase;
    class OgitorsPropertyDef;
};

class QtTreePropertyBrowser;
class QtGroupPropertyManager;
class QtStringPropertyManager;
class QtEnumPropertyManager;
class QtProperty;

class TreeBrowser: public QtTreePropertyBrowser
{
    Q_OBJECT;
public:
    TreeBrowser(QWidget *parent = 0) : QtTreePropertyBrowser(parent), mContextItem(0), mDefinition(0) { mContainerDialog = parent; };
    virtual ~TreeBrowser() {};

    void setDefinition(const Ogitors::OgitorsPropertyDef* definition) { mDefinition = const_cast<Ogitors::OgitorsPropertyDef*>(definition); }
public Q_SLOTS:
    void removeOption();
protected:
    QtBrowserItem               *mContextItem;
    Ogitors::OgitorsPropertyDef *mDefinition;
    QWidget                     *mContainerDialog;

    void contextMenuEvent(QContextMenuEvent *evt);
};


class UserDefinedDialog : public QDialog, public Ui::userdefineddialog {
    Q_OBJECT
public:
    UserDefinedDialog(QWidget *parent, Ogitors::OgitorsCustomPropertySet *set);
    virtual ~UserDefinedDialog();
    Ogitors::OgitorsCustomPropertySet *getSet() { return mSet; }

public Q_SLOTS:
    void stringValueChanged(QtProperty *property, const QString& val);
    void enumValueChanged(QtProperty *property, int val);
    void listSelectionChanged();
    void upClicked();
    void downClicked();
    void addClicked();
    void deleteClicked();
    void addOptionClicked();

protected:
    int           BLOCKSETFUNCTIONS;
    QStringList   mPropertyTypes;
    QTableWidget *mPropertyList;
    TreeBrowser  *mPropertiesWidget;
    QPushButton  *mAddOptionButton;
    Ogitors::OgitorsCustomPropertySet *mSet;
    Ogitors::OgitorsPropertyBase      *mCurrentDisplayedProperty;
    QtGroupPropertyManager  *groupManager;
    QtStringPropertyManager *stringManager;
    QtEnumPropertyManager   *enumManager;

    void DisplayProperties();
    void UpdateInterface();
};

#endif // USERDEFINEDDIALOG_HXX
