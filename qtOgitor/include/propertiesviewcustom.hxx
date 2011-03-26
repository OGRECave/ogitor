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

#ifndef PROPERTIESVIEWCUSTOM_HXX
#define PROPERTIESVIEWCUSTOM_HXX

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <Ogre.h>
#include "qttreepropertybrowser.h"
#include "OgitorsPrerequisites.h"


class QtTreePropertyBrowser;
class QtGroupPropertyManager;
class QtBoolPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtStringPropertyManager;
class QtEnumPropertyManager;
class QtColorPropertyManager;
class QuaternionManager;
class Vector2Manager;
class Vector3Manager;
class Vector4Manager;
class QtProperty;

typedef std::map<QtProperty*,Ogitors::OgitorsPropertyBase*> QtToOgitorPropertyMap;
typedef std::map<Ogitors::OgitorsPropertyBase*, QtProperty*> OgitorToQtPropertyMap;

class CustomTreeBrowser: public QtTreePropertyBrowser
{
    Q_OBJECT;
public:
    CustomTreeBrowser(QWidget *parent = 0) : QtTreePropertyBrowser(parent) {};
    virtual ~CustomTreeBrowser() {};
};

class CustomPropertiesViewWidget : public QWidget, public Ogitors::OgitorsPropertySetListener
{
    Q_OBJECT;
public:

    explicit CustomPropertiesViewWidget(QWidget *parent = 0);
    virtual ~CustomPropertiesViewWidget();

    CustomTreeBrowser *getPropertiesWidget() {return propertiesWidget;}

    void PresentPropertiesView(Ogitors::CBaseEditor* object);

    Ogitors::OgitorsPropertyBase *getOgitorProperty(QtProperty *property);

public Q_SLOTS:
    void boolValueChanged(QtProperty *property, bool val);
    void stringValueChanged(QtProperty *property, const QString& val);
    void intValueChanged(QtProperty *property, int val);
    void colourValueChanged(QtProperty *property, const QColor& val);
    void doubleValueChanged(QtProperty *property, double val);
    void enumValueChanged(QtProperty *property, int val);
    void quaternionValueChanged(QtProperty *property, const QVariant& val);
    void vector2ValueChanged(QtProperty *property, const QVariant& val);
    void vector3ValueChanged(QtProperty *property, const QVariant& val);
    void vector4ValueChanged(QtProperty *property, const QVariant& val);
    void modifyClicked();
    void importClicked();
    void importInClicked();
    void exportClicked();

protected:
    CustomTreeBrowser       *propertiesWidget;
    QPushButton             *buttonModify;
    QPushButton             *buttonImport;
    QPushButton             *buttonImportIn;
    QPushButton             *buttonExport;

    QtGroupPropertyManager  *groupManager;
    QtBoolPropertyManager   *boolManager;
    QtColorPropertyManager  *colourManager;
    QtIntPropertyManager    *intManager;
    QtDoublePropertyManager *doubleManager;
    QtStringPropertyManager *stringManager;
    QtEnumPropertyManager   *enumManager;
    QuaternionManager       *quaternionManager;
    Vector2Manager          *vector2Manager;
    Vector3Manager          *vector3Manager;
    Vector4Manager          *vector4Manager;

    Ogitors::CBaseEditor           *mLastObject;  
    Ogitors::ScopedConnectionVector mPropertyConnections;
    QtToOgitorPropertyMap           mQtToOgitorPropertyMap;
    OgitorToQtPropertyMap           mOgitorToQtPropertyMap;

    inline QtProperty *getPropertyGroup(Ogre::String& name, QtProperty *defaultGroup);
    inline void createProperty(QtProperty *group, QString name, Ogitors::OgitorsPropertyBase *property); 
    void DisplayObjectProperties(Ogitors::CBaseEditor* object);

    void propertyChangeTracker(const Ogitors::OgitorsPropertyBase* property, Ogre::Any value); 
    void OnPropertyRemoved(Ogitors::OgitorsPropertySet* set, Ogitors::OgitorsPropertyBase* property);
    void OnPropertyAdded(Ogitors::OgitorsPropertySet* set, Ogitors::OgitorsPropertyBase* property);
    void OnPropertyChanged(Ogitors::OgitorsPropertySet* set, Ogitors::OgitorsPropertyBase* property) {};
    void OnPropertySetRebuilt(Ogitors::OgitorsPropertySet* set);

private:
    int BLOCKSETFUNCTIONS;
    QtProperty *lastGroup;
    Ogre::String lastGroupName;

};

#endif // PROPERTIESVIEWCUSTOM_HXX
