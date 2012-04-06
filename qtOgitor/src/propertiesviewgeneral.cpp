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
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QSignalMapper>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QIcon>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "propertiesviewgeneral.hxx"
#include "complexproperties.hxx"
#include "tinyxml.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsUndoManager.h"
#include "BaseEditor.h"
#include "MultiSelEditor.h"

using namespace Ogitors;

QString              LastPropertyName = "";
OgitorsPropertyBase* LastProperty;

extern QString ConvertToQString(Ogre::UTFString& value);

void GeneralTreeBrowser::contextMenuEvent(QContextMenuEvent *evt)
{
    QtBrowserItem *citem = currentItem();
    if(citem == 0)
        return;

    CBaseEditor *e = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();

    LastProperty = static_cast<GeneralPropertiesViewWidget*>(parent())->getOgitorProperty(citem->property());

    if(!LastProperty)
        LastPropertyName = citem->property()->propertyName();
    else
        LastPropertyName = LastProperty->getName().c_str();

    if(e != 0)
    {
        UTFStringVector menuList;
        e->getPropertyContextMenu(LastPropertyName.toStdString(), menuList);
        
        if(menuList.size() > 0)
        {
            UTFStringVector vList;
            int counter = 0;
            QMenu* contextMenu = new QMenu(this);
            QSignalMapper *signalMapper = new QSignalMapper(this);

            for(unsigned int i = 0;i < menuList.size();i++)
            {
                OgitorsUtils::ParseUTFStringVector(menuList[i], vList);
                if(vList.size() > 0 && vList[0] != "")
                {                 
                    QAction *item = contextMenu->addAction( ConvertToQString(vList[0]), signalMapper, SLOT(map()), 0);
                    if(vList.size() > 1)
                        item->setIcon(QIcon(ConvertToQString(vList[1])));
                    signalMapper->setMapping(item, i);
                    counter++;
                }
            }
            if(counter)
            {
                connect(signalMapper, SIGNAL(mapped( int )), this, SLOT(contextMenu( int )));
                contextMenu->exec(QCursor::pos());
            }
            delete contextMenu;
            delete signalMapper;
        }
    }

    evt->accept();
}
//----------------------------------------------------------------------------------------
void GeneralTreeBrowser::contextMenu(int id)
{
    if(!OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
        OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle()->onPropertyContextMenu(LastPropertyName.toStdString(), id);
}
//----------------------------------------------------------------------------------------//----------------------------------------------------------------------------------------
GeneralPropertiesViewWidget::GeneralPropertiesViewWidget(QWidget *parent) :
QWidget(parent), mLastObject(0), BLOCKSETFUNCTIONS(0)
{
    groupManager = new QtGroupPropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    enumManager = new QtEnumPropertyManager(this);
    colourManager = new QtColorPropertyManager(this);
    quaternionManager = new QuaternionManager(this);
    vector2Manager = new Vector2Manager(this);
    vector3Manager = new Vector3Manager(this);
    vector4Manager = new Vector4Manager(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);
    QtColorEditorFactory *colourEditFactory = new QtColorEditorFactory(this);
    QtVariantEditorFactory *variantEditFactory = new QtVariantEditorFactory(this);
    QtVariantPropertyManager *varMan1 = quaternionManager;
    QtVariantPropertyManager *varMan2 = vector2Manager;
    QtVariantPropertyManager *varMan3 = vector3Manager;
    QtVariantPropertyManager *varMan4 = vector4Manager;
    propertiesWidget = new GeneralTreeBrowser();
    propertiesWidget->setFactoryForManager(boolManager, checkBoxFactory);
    propertiesWidget->setFactoryForManager(intManager, spinBoxFactory);
    propertiesWidget->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertiesWidget->setFactoryForManager(enumManager, comboBoxFactory);
    propertiesWidget->setFactoryForManager(stringManager, lineEditFactory);
    propertiesWidget->setFactoryForManager(colourManager, colourEditFactory);
    propertiesWidget->setAlternatingRowColors(true);
    propertiesWidget->setIndentation(10);
    propertiesWidget->setFactoryForManager(varMan1, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan2, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan3, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan4, variantEditFactory);
    propertiesWidget->setResizeMode(QtTreePropertyBrowser::Interactive);

    descLabel = new QLabel(this);
    descLabel->setFixedHeight(48);
    descLabel->setWordWrap(true);
    descLabel->setMargin(4);
    descLabel->setObjectName(QString::fromUtf8("descLabel"));
    QVBoxLayout *boxlayout = new QVBoxLayout();
    boxlayout->setMargin(0);
    boxlayout->addWidget(propertiesWidget);
    boxlayout->addWidget(descLabel);
    boxlayout->setStretch(0, 1);
    boxlayout->setStretch(1, 0);
    boxlayout->setSpacing(0);
    setLayout(boxlayout);

    mQtToOgitorPropertyMap.clear();
    mOgitorToQtPropertyMap.clear();
    mPropertyConnections.clear();

    connect(propertiesWidget, SIGNAL(currentItemChanged(QtBrowserItem *)), this, SLOT(itemChanged(QtBrowserItem *)));
}
//----------------------------------------------------------------------------------------
GeneralPropertiesViewWidget::~GeneralPropertiesViewWidget()
{
    mQtToOgitorPropertyMap.clear();
    mOgitorToQtPropertyMap.clear();

    for(unsigned int pidx = 0;pidx < mPropertyConnections.size();pidx++)
        delete mPropertyConnections[pidx];
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::itemChanged(QtBrowserItem *item)
{
    if(item)
    {
        QtProperty *prop = item->property();
        descLabel->setText(prop->whatsThis());
    }
    else
        descLabel->setText("");

}
//----------------------------------------------------------------------------------------
OgitorsPropertyBase *GeneralPropertiesViewWidget::getOgitorProperty(QtProperty *property)
{
    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
        return it->second;
    else
        return 0;
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::PresentPropertiesView(CBaseEditor* object)
{
    lastGroup = 0;
    lastGroupName = "";

    disconnect(quaternionManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(quaternionValueChanged(QtProperty *, const QVariant &)));
    disconnect(vector2Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector2ValueChanged(QtProperty *, const QVariant &)));
    disconnect(vector3Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector3ValueChanged(QtProperty *, const QVariant &)));
    disconnect(vector4Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector4ValueChanged(QtProperty *, const QVariant &)));
    disconnect(boolManager, SIGNAL(valueChanged(QtProperty *, bool)), this, SLOT(boolValueChanged(QtProperty *, bool)));
    disconnect(intManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(intValueChanged(QtProperty *, int)));
    disconnect(doubleManager, SIGNAL(valueChanged(QtProperty *, double)), this, SLOT(doubleValueChanged(QtProperty *, double)));
    disconnect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(stringValueChanged(QtProperty *, const QString &)));
    disconnect(enumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(enumValueChanged(QtProperty *, int)));
    disconnect(colourManager, SIGNAL(valueChanged(QtProperty *, const QColor &)), this, SLOT(colourValueChanged(QtProperty *, const QColor &)));

    propertiesWidget->clear();
    quaternionManager->clear();
    vector2Manager->clear();
    vector3Manager->clear();
    vector4Manager->clear();
    boolManager->clear();
    intManager->clear();
    doubleManager->clear();
    stringManager->clear();
    enumManager->clear();
    colourManager->clear();

    mQtToOgitorPropertyMap.clear();
    mOgitorToQtPropertyMap.clear();

    for(unsigned int pidx = 0;pidx < mPropertyConnections.size();pidx++)
        delete mPropertyConnections[pidx];

    mPropertyConnections.clear();
    
    if(mLastObject)
        mLastObject->getProperties()->removeListener(this);
    
    if(object != 0)
    {
        DisplayObjectProperties(object);

        object->getProperties()->addListener(this);
    }
    else
        descLabel->setText("");


    mLastObject = object;

    connect(quaternionManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(quaternionValueChanged(QtProperty *, const QVariant &)));
    connect(vector2Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector2ValueChanged(QtProperty *, const QVariant &)));
    connect(vector3Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector3ValueChanged(QtProperty *, const QVariant &)));
    connect(vector4Manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(vector4ValueChanged(QtProperty *, const QVariant &)));
    connect(boolManager, SIGNAL(valueChanged(QtProperty *, bool)), this, SLOT(boolValueChanged(QtProperty *, bool)));
    connect(intManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(intValueChanged(QtProperty *, int)));
    connect(doubleManager, SIGNAL(valueChanged(QtProperty *, double)), this, SLOT(doubleValueChanged(QtProperty *, double)));
    connect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(stringValueChanged(QtProperty *, const QString &)));
    connect(enumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(enumValueChanged(QtProperty *, int)));
    connect(colourManager, SIGNAL(valueChanged(QtProperty *, const QColor &)), this, SLOT(colourValueChanged(QtProperty *, const QColor &)));
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::createProperty(QtProperty *group, QString name, OgitorsPropertyBase *property)
{
    const OgitorsPropertyDef *propDef = property->getDefinition();
    QtProperty *newProp = 0;

    Ogre::Any           any;
    Ogre::Vector2       val2;
    Ogre::Vector3       val3;
    Ogre::Vector4       val4;
    Ogre::Quaternion    valQ;
    Ogre::ColourValue   valc;
    QPointF             vpf;
    QRectF              vrf;
    QColor              col;

    const PropertyOptionsVector *options = propDef->getOptions();

    if(options)
    {
        newProp = enumManager->addProperty(name);
        Ogre::String currentValue = property->getOptionName();
        int currentValuePos = -1;

        QStringList choices;

        for(unsigned int i = 0;i < (*options).size();i++)
        {
            if((*options)[i].mKey == currentValue)
                currentValuePos = i;

            choices.push_back(QString((*options)[i].mKey.c_str()));
        }

        enumManager->setEnumNames(newProp, choices);
        enumManager->setValue(newProp, currentValuePos);
    }
    else
    {
        QString fieldX, fieldY, fieldZ, fieldW;
        switch(propDef->getType())
        {
        case PROP_BOOL:
            newProp = boolManager->addProperty(name);
            boolManager->setValue(newProp, static_cast<OgitorsProperty<bool>*>(property)->get());
            break;   
        
        case PROP_SHORT:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<short>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<short>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<short>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<short>*>(property)->get());
            break;
        case PROP_UNSIGNED_SHORT:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<unsigned short>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<unsigned short>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<unsigned short>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<unsigned short>*>(property)->get());
            break;
        case PROP_INT:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<int>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<int>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<int>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<int>*>(property)->get());
            break;
        case PROP_UNSIGNED_INT:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<unsigned int>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<unsigned int>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<unsigned int>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<unsigned int>*>(property)->get());
            break;
        case PROP_LONG:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<long>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<long>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<long>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<long>*>(property)->get());
            break;
        case PROP_UNSIGNED_LONG:
            newProp = intManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty())
                intManager->setMinimum(newProp, Ogre::any_cast<unsigned long>(any));
            if(!(any = propDef->getMaxValue()).isEmpty())
                intManager->setMaximum(newProp, Ogre::any_cast<unsigned long>(any));
            if(!(any = propDef->getStepSize()).isEmpty())
                intManager->setSingleStep(newProp, Ogre::any_cast<unsigned long>(any));
            intManager->setValue(newProp, static_cast<OgitorsProperty<unsigned long>*>(property)->get());
            break;

        case PROP_REAL:
            newProp = doubleManager->addProperty(name);
            if(!(any = propDef->getMinValue()).isEmpty()) 
                doubleManager->setMinimum(newProp, Ogre::any_cast<Ogre::Real>(any));
            if((!(any = propDef->getMaxValue()).isEmpty()))
                doubleManager->setMaximum(newProp, Ogre::any_cast<Ogre::Real>(any));
            if((!(any = propDef->getStepSize()).isEmpty()))
                doubleManager->setSingleStep(newProp, Ogre::any_cast<Ogre::Real>(any));
            doubleManager->setValue(newProp, static_cast<OgitorsProperty<Ogre::Real>*>(property)->get());
            break;

        case PROP_STRING:
            newProp = stringManager->addProperty(name);
            stringManager->setValue(newProp, static_cast<OgitorsProperty<Ogre::String>*>(property)->get().c_str());
            break;

        case PROP_VECTOR2:
            newProp = vector2Manager->addProperty(QVariant::PointF, name);
            fieldX = propDef->getFieldName(0).c_str();
            fieldY = propDef->getFieldName(1).c_str();
            vector2Manager->setPropertyNames(newProp, fieldX, fieldY);
            if(!(any = propDef->getMinValue()).isEmpty())
            {
                val2 = Ogre::any_cast<Ogre::Vector2>(any);
                vpf = QPointF(val2.x, val2.y);
                vector2Manager->setMinimum(newProp, vpf);
            }       
            if(!(any = propDef->getMaxValue()).isEmpty())
            {
                val2 = Ogre::any_cast<Ogre::Vector2>(any);
                vpf = QPointF(val2.x, val2.y);
                vector2Manager->setMaximum(newProp, vpf);
            } 
            if(!(any = propDef->getStepSize()).isEmpty())
            {
                val2 = Ogre::any_cast<Ogre::Vector2>(any);
                vpf = QPointF(val2.x, val2.y);
                vector2Manager->setStepSize(newProp, vpf);
            } 
            val2 = static_cast<OgitorsProperty<Ogre::Vector2>*>(property)->get();
            vpf = QPointF(val2.x, val2.y);
            vector2Manager->setValue(newProp, vpf);
            break;
        case PROP_VECTOR3:
            newProp = vector3Manager->addProperty(QVariant::RectF, name);
            fieldX = propDef->getFieldName(0).c_str();
            fieldY = propDef->getFieldName(1).c_str();
            fieldZ = propDef->getFieldName(2).c_str();
            vector3Manager->setPropertyNames(newProp, fieldX, fieldY, fieldZ);
            if(!(any = propDef->getMinValue()).isEmpty())
            {
                val3 = Ogre::any_cast<Ogre::Vector3>(any);
                vrf = QRectF(val3.x, val3.y, val3.z, 0);
                vector3Manager->setMinimum(newProp, vrf);
            }       
            if(!(any = propDef->getMaxValue()).isEmpty())
            {
                val3 = Ogre::any_cast<Ogre::Vector3>(any);
                vrf = QRectF(val3.x, val3.y, val3.z, 0);
                vector3Manager->setMaximum(newProp, vrf);
            } 
            if(!(any = propDef->getStepSize()).isEmpty())
            {
                val3 = Ogre::any_cast<Ogre::Vector3>(any);
                vrf = QRectF(val3.x, val3.y, val3.z, 0);
                vector3Manager->setStepSize(newProp, vrf);
            } 
            val3 = static_cast<OgitorsProperty<Ogre::Vector3>*>(property)->get();
            vrf = QRectF(val3.x, val3.y, val3.z, 0);
            vector3Manager->setValue(newProp, vrf);
            break;
        case PROP_VECTOR4:
            newProp = vector4Manager->addProperty(QVariant::RectF, name);
            fieldX = propDef->getFieldName(0).c_str();
            fieldY = propDef->getFieldName(1).c_str();
            fieldZ = propDef->getFieldName(2).c_str();
            fieldW = propDef->getFieldName(3).c_str();
            vector4Manager->setPropertyNames(newProp, fieldX, fieldY, fieldZ, fieldW);
            if(!(any = propDef->getMinValue()).isEmpty())
            {
                val4 = Ogre::any_cast<Ogre::Vector4>(any);
                vrf = QRectF(val4.x, val4.y, val4.z, val4.w);
                vector4Manager->setMinimum(newProp, vrf);
            }       
            if(!(any = propDef->getMaxValue()).isEmpty())
            {
                val4 = Ogre::any_cast<Ogre::Vector4>(any);
                vrf = QRectF(val4.x, val4.y, val4.z, val4.w);
                vector4Manager->setMaximum(newProp, vrf);
            } 
            if(!(any = propDef->getStepSize()).isEmpty())
            {
                val4 = Ogre::any_cast<Ogre::Vector4>(any);
                vrf = QRectF(val4.x, val4.y, val4.z, val4.w);
                vector4Manager->setStepSize(newProp, vrf);
            } 
            val4 = static_cast<OgitorsProperty<Ogre::Vector4>*>(property)->get();
            vrf = QRectF(val4.x, val4.y, val4.z, val4.w);
            vector4Manager->setValue(newProp, vrf);
            break;
        case PROP_QUATERNION:
            newProp = quaternionManager->addProperty(QVariant::RectF, name);
            if(!(any = propDef->getMinValue()).isEmpty())
            {
                valQ = Ogre::any_cast<Ogre::Quaternion>(any);
                vrf = QRectF(valQ.x, valQ.y, valQ.z, valQ.w);
                quaternionManager->setMinimum(newProp, vrf);
            }       
            if(!(any = propDef->getMaxValue()).isEmpty())
            {
                valQ = Ogre::any_cast<Ogre::Quaternion>(any);
                vrf = QRectF(valQ.x, valQ.y, valQ.z, valQ.w);
                quaternionManager->setMaximum(newProp, vrf);
            } 
            if(!(any = propDef->getStepSize()).isEmpty())
            {
                valQ = Ogre::any_cast<Ogre::Quaternion>(any);
                vrf = QRectF(valQ.x, valQ.y, valQ.z, valQ.w);
                quaternionManager->setStepSize(newProp, vrf);
            }
            valQ = static_cast<OgitorsProperty<Ogre::Quaternion>*>(property)->get();
            vrf = QRectF(valQ.x, valQ.y, valQ.z, valQ.w);
            quaternionManager->setValue(newProp, vrf);
            break;

        case PROP_COLOUR:
            newProp = colourManager->addProperty(name);
            valc = static_cast<OgitorsProperty<Ogre::ColourValue>*>(property)->get();
            col = QColor(valc.r * 255.0f, valc.g * 255.0f, valc.b * 255.0f, valc.a * 255.0f);
            colourManager->setValue(newProp, col);
            break;     
        };
    }

    if(newProp)
    {
        mQtToOgitorPropertyMap.insert(QtToOgitorPropertyMap::value_type(newProp, property));
        mOgitorToQtPropertyMap.insert(OgitorToQtPropertyMap::value_type(property, newProp));
        
        OgitorsScopedConnection *connection = new OgitorsScopedConnection();
        property->connect(OgitorsSignalFunction::from_method<GeneralPropertiesViewWidget, &GeneralPropertiesViewWidget::propertyChangeTracker>(this), *connection);
        mPropertyConnections.push_back(connection);
        newProp->setWhatsThis(propDef->getDescription().c_str());
        newProp->setEnabled(propDef->canWrite());
        
        if(group)
            group->addSubProperty(newProp);
        else
        {
            propertiesWidget->addProperty(newProp);
        }
    }
}
//----------------------------------------------------------------------------------------
QtProperty *GeneralPropertiesViewWidget::getPropertyGroup(Ogre::String& name, QtProperty *defaultGroup)
{
    int position = name.find_last_of("::");
    if(position == -1)
        return defaultGroup;
 
    Ogre::String groupName = name.substr(0, position + 1);
    Ogre::String groupNameCopy = name.substr(0, position - 1);
    name.erase(0, position + 1);

    if(lastGroup && (lastGroupName == groupName))
        return lastGroup;

    lastGroupName = "";
    lastGroup = defaultGroup;

    while((position = groupName.find("::")) != -1)
    {
        QString tmpName = groupName.substr(0, position).c_str();
        groupName.erase(0, position + 2);
        lastGroupName += tmpName.toStdString() + "::";
        
        QtProperty* newGroup = 0;
        
        QList<QtProperty*> subProperties;

        if(lastGroup)
            subProperties = lastGroup->subProperties();
        else
            subProperties = propertiesWidget->properties();

        for(int i = 0;i < subProperties.size();i++)
        {
            if(subProperties[i]->propertyName() == tmpName)
            {
                 newGroup = subProperties[i];
                 break;
            }
        }

        if(!newGroup)
        {
            newGroup = groupManager->addProperty(tmpName);
            newGroup->setBold(true);
            newGroup->setToolTip(groupNameCopy.c_str());

            if(lastGroup)
                lastGroup->addSubProperty(newGroup);
            else
            {
                propertiesWidget->addProperty(newGroup);
            }
        }
        lastGroup = newGroup;
    }
    
    return lastGroup;
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::deleteGroups(Ogre::String& name)
{
    if(name == "")
        return;
    
    Ogre::String namecopy = name;
    QtProperty *group = getPropertyGroup(namecopy, 0);

    if(group && (group->subProperties().size() == 0))
    {
        delete group;

        int pos;
        if((pos = name.find_last_of("::")) != -1)
            name = name.substr(0, pos - 1);

        deleteGroups(name);
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property)
{
    OgitorToQtPropertyMap::iterator it = mOgitorToQtPropertyMap.find(property);
    if(it != mOgitorToQtPropertyMap.end())
    {
        QtToOgitorPropertyMap::iterator it2 = mQtToOgitorPropertyMap.find(it->second);
        if(it2 != mQtToOgitorPropertyMap.end())
            mQtToOgitorPropertyMap.erase(it2);

        propertiesWidget->removeProperty(it->second);
        delete it->second;
        mOgitorToQtPropertyMap.erase(it);
    }
    Ogre::String name = property->getDefinition()->getDisplayName();

    deleteGroups(name);
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property)
{
    const OgitorsPropertyDef *propDef = property->getDefinition();
    if(!propDef->canRead())
        return;

    Ogre::String strDisplayName = propDef->getDisplayName();
    QtProperty* propertyGroup = getPropertyGroup(strDisplayName, 0);
    createProperty(propertyGroup, strDisplayName.c_str(), property);
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::DisplayObjectProperties(CBaseEditor* object)
{
    lastGroup = 0;
    lastGroupName = "";
    
    OgitorsPropertyVector props = object->getProperties()->getPropertyVector();
    
    PropertySetType propsetType = object->getProperties()->getType();

    Ogre::String strDisplayName;

    const OgitorsPropertyDef *propDef;

    for(unsigned int i = 0;i < props.size();i++)
    {
        propDef = props[i]->getDefinition();
        if(!propDef->canRead())
            continue;

        strDisplayName = propDef->getDisplayName();
        QtProperty* propertyGroup = getPropertyGroup(strDisplayName, 0);
        createProperty(propertyGroup, strDisplayName.c_str(), props[i]);
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::enumValueChanged(QtProperty *property, int val)     
{
    if(BLOCKSETFUNCTIONS)
        return;

    if(val == -1)
        return;

    QStringList list = enumManager->enumNames(property);

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsPropertyBase *ogProperty = it->second;
        QString tmpVal = list[val];
        Ogre::String ogreStr = tmpVal.toStdString();

        Ogre::String prevVal;
        prevVal = ogProperty->getOptionName();
        
        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->setByOptionValue(ogreStr);
        undoMgr->EndCollection(true);

        if(prevVal == ogProperty->getOptionName())
        {
            BLOCKSETFUNCTIONS++;

            QString option = prevVal.c_str();
            bool found = false;
            for(int i = 0;i < list.size();i++)
                if(list[i] == option)
                {
                    enumManager->setValue(property, i);
                    found = true;
                    break;
                }
            if(!found)
                enumManager->setValue(property, -1);

            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::boolValueChanged(QtProperty *property, bool val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<bool> *ogProperty = static_cast<OgitorsProperty<bool>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(val);
        undoMgr->EndCollection(true);

        if(val != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            boolManager->setValue(property, ogProperty->get());
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::stringValueChanged(QtProperty *property, const QString& val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::String> *ogProperty = static_cast<OgitorsProperty<Ogre::String>*>(it->second);
        Ogre::String ogreStr = val.toStdString();

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(ogreStr);
        undoMgr->EndCollection(true);

        if(ogreStr != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QString resetVal = ogProperty->get().c_str();
            stringManager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::colourValueChanged(QtProperty *property, const QColor& val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::ColourValue> *ogProperty = static_cast<OgitorsProperty<Ogre::ColourValue>*>(it->second);
        Ogre::ColourValue ogreCol(val.redF(), val.greenF(), val.blueF(), val.alphaF());

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(ogreCol);
        undoMgr->EndCollection(true);

        if(ogreCol != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QColor resetVal(ogProperty->get().r * 255.0f, ogProperty->get().g * 255.0f, ogProperty->get().b * 255.0f, ogProperty->get().a * 255.0f);
            colourManager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::doubleValueChanged(QtProperty *property, double val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Real> *ogProperty = static_cast<OgitorsProperty<Ogre::Real>*>(it->second);
        Ogre::Real ogreVal = (Ogre::Real)val;

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(ogreVal);
        undoMgr->EndCollection(true);

        if(ogreVal != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            double resetVal = ogProperty->get();
            doubleManager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::quaternionValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = qVariantValue<QRectF>(val);
    Ogre::Quaternion qval(rval.height(), rval.x(), rval.y(), rval.width());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Quaternion> *ogProperty = static_cast<OgitorsProperty<Ogre::Quaternion>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(qval);
        undoMgr->EndCollection(true);

        if(qval != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QRectF resetVal(ogProperty->get().x, ogProperty->get().y, ogProperty->get().z, ogProperty->get().w);
            quaternionManager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::vector4ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = qVariantValue<QRectF>(val);
    Ogre::Vector4 qval(rval.x(), rval.y(), rval.width(),rval.height());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector4> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector4>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(qval);
        undoMgr->EndCollection(true);

        if(qval != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QRectF resetVal(ogProperty->get().x, ogProperty->get().y, ogProperty->get().z, ogProperty->get().w);
            vector4Manager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::vector3ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = qVariantValue<QRectF>(val);
    Ogre::Vector3 qval(rval.x(), rval.y(), rval.width());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector3> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector3>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(qval);
        undoMgr->EndCollection(true);

        if(qval != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QRectF resetVal(ogProperty->get().x, ogProperty->get().y, ogProperty->get().z, 0);
            vector3Manager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::vector2ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QPointF rval = qVariantValue<QPointF>(val);
    Ogre::Vector2 qval(rval.x(), rval.y());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector2> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector2>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + ogProperty->getDefinition()->getDisplayName() + " change");
        ogProperty->set(qval);
        undoMgr->EndCollection(true);

        if(qval != ogProperty->get())
        {
            BLOCKSETFUNCTIONS++;
            QPointF resetVal(ogProperty->get().x, ogProperty->get().y);
            vector4Manager->setValue(property, resetVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::intValueChanged(QtProperty *property, int val) 
{
    if(BLOCKSETFUNCTIONS)
        return;

    short valShort = val;
    unsigned short valUShort = val;
    int valInt = val;
    unsigned int valUInt = val;
    long valLong = val;
    unsigned long valULong = val;

    long tmpVal;

    OgitorsProperty<short> *ogPropertyShort;
    OgitorsProperty<unsigned short> *ogPropertyUShort;
    OgitorsProperty<int> *ogPropertyInt;
    OgitorsProperty<unsigned int> *ogPropertyUInt;
    OgitorsProperty<long> *ogPropertyLong;
    OgitorsProperty<unsigned long> *ogPropertyULong;


    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s " + it->second->getDefinition()->getDisplayName() + " change");

        switch(it->second->getType())
        {
        case PROP_SHORT:
            ogPropertyShort = static_cast<OgitorsProperty<short>*>(it->second);
            ogPropertyShort->set(valShort);
            if(valShort != ogPropertyShort->get())
            {
                tmpVal = ogPropertyShort->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        case PROP_UNSIGNED_SHORT:
            ogPropertyUShort = static_cast<OgitorsProperty<unsigned short>*>(it->second);
            ogPropertyUShort->set(valUShort);
            if(valUShort != ogPropertyUShort->get())
            {
                tmpVal = ogPropertyUShort->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        case PROP_INT:
            ogPropertyInt = static_cast<OgitorsProperty<int>*>(it->second);
            ogPropertyInt->set(valInt);
            if(valInt != ogPropertyInt->get())
            {
                tmpVal = ogPropertyInt->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        case PROP_UNSIGNED_INT:
            ogPropertyUInt = static_cast<OgitorsProperty<unsigned int>*>(it->second);
            ogPropertyUInt->set(valUInt);
            if(valUInt != ogPropertyUInt->get())
            {
                tmpVal = ogPropertyUInt->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        case PROP_LONG:
            ogPropertyLong = static_cast<OgitorsProperty<long>*>(it->second);
            ogPropertyLong->set(valLong);
            if(valLong != ogPropertyLong->get())
            {
                tmpVal = ogPropertyLong->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        case PROP_UNSIGNED_LONG:
            ogPropertyULong = static_cast<OgitorsProperty<unsigned long>*>(it->second);
            ogPropertyULong->set(valULong);
            if(valULong != ogPropertyULong->get())
            {
                tmpVal = ogPropertyULong->get();
                BLOCKSETFUNCTIONS++;
                intManager->setValue(property, tmpVal);
                BLOCKSETFUNCTIONS--;
            }
            break;
        };

        undoMgr->EndCollection(true);
    }
}
//----------------------------------------------------------------------------------------
void GeneralPropertiesViewWidget::propertyChangeTracker(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(BLOCKSETFUNCTIONS)
        return;

    Ogre::Vector2 val2;
    Ogre::Vector3 val3;
    Ogre::Vector4 val4;
    Ogre::Quaternion valQ;
    Ogre::ColourValue valc;

    QPointF vpf;
    QRectF  vrf;
    QColor col;

    OgitorsPropertyBase *ogitorProp = const_cast<OgitorsPropertyBase*>(property);

    OgitorToQtPropertyMap::const_iterator it = mOgitorToQtPropertyMap.find(ogitorProp);
    if(it != mOgitorToQtPropertyMap.end())
    {
        BLOCKSETFUNCTIONS++;
        if(it->second->propertyManager() == enumManager)
        {
            QString option = ogitorProp->getOptionName().c_str();

            QStringList values = enumManager->enumNames(it->second);
            bool found = false;
            for(int i = 0;i < values.size();i++)
                if(values[i] == option)
                {
                    enumManager->setValue(it->second, i);
                    found = true;
                    break;
                }
            if(!found)
                enumManager->setValue(it->second, -1);
        }
        else
        {
            switch(ogitorProp->getType())
            {
            case PROP_SHORT:
                intManager->setValue(it->second, static_cast<OgitorsProperty<short>*>(ogitorProp)->get());
                break;
            case PROP_UNSIGNED_SHORT:
                intManager->setValue(it->second, static_cast<OgitorsProperty<unsigned short>*>(ogitorProp)->get());
                break;
            case PROP_INT:
                intManager->setValue(it->second, static_cast<OgitorsProperty<int>*>(ogitorProp)->get());
                break;
            case PROP_UNSIGNED_INT:
                intManager->setValue(it->second, static_cast<OgitorsProperty<unsigned int>*>(ogitorProp)->get());
                break;
            case PROP_LONG:
                intManager->setValue(it->second, static_cast<OgitorsProperty<long>*>(ogitorProp)->get());
                break;
            case PROP_UNSIGNED_LONG:
                intManager->setValue(it->second, static_cast<OgitorsProperty<unsigned long>*>(ogitorProp)->get());
                break;

            case PROP_REAL:
                doubleManager->setValue(it->second, static_cast<OgitorsProperty<Ogre::Real>*>(ogitorProp)->get());
                break;
            case PROP_STRING:
                stringManager->setValue(it->second, static_cast<OgitorsProperty<Ogre::String>*>(ogitorProp)->get().c_str());
                break;
            case PROP_VECTOR2:
                val2 = static_cast<OgitorsProperty<Ogre::Vector2>*>(ogitorProp)->get();
                vpf = QPointF(val2.x, val2.y);
                vector2Manager->setValue(it->second, vpf);
                break;
            case PROP_VECTOR3:
                val3 = static_cast<OgitorsProperty<Ogre::Vector3>*>(ogitorProp)->get();
                vrf = QRectF(val3.x, val3.y, val3.z, 0);
                vector3Manager->setValue(it->second, vrf);
                break;
            case PROP_VECTOR4:
                val4 = static_cast<OgitorsProperty<Ogre::Vector4>*>(ogitorProp)->get();
                vrf = QRectF(val4.x, val4.y, val4.z, val4.w);
                vector3Manager->setValue(it->second, vrf);
                break;
            case PROP_COLOUR:
                valc = static_cast<OgitorsProperty<Ogre::ColourValue>*>(ogitorProp)->get();
                col = QColor(valc.r * 255.0f, valc.g * 255.0f, valc.b * 255.0f, valc.a * 255.0f);
                colourManager->setValue(it->second, col);
                break;
            case PROP_BOOL:
                boolManager->setValue(it->second, static_cast<OgitorsProperty<bool>*>(ogitorProp)->get());
                break;
            case PROP_QUATERNION:
                valQ = static_cast<OgitorsProperty<Ogre::Quaternion>*>(ogitorProp)->get();
                vrf = QRectF(valQ.x, valQ.y, valQ.z, valQ.w);
                quaternionManager->setValue(it->second, vrf);
                break;
            };
        }
        BLOCKSETFUNCTIONS--;
    }
}
//----------------------------------------------------------------------------------------
