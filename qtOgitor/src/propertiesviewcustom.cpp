/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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

#include <QtGui/QDragMoveEvent>
#include <QtGui/QContextMenuEvent>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenu>
#include <QtGui/QIcon>

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "propertiesviewcustom.hxx"
#include "complexproperties.hxx"
#include "tinyxml.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsUndoManager.h"
#include "BaseEditor.h"
#include "userdefined.hxx"

using namespace Ogitors;

extern QString ConvertToQString(Ogre::UTFString& value);

//----------------------------------------------------------------------------------------
CustomPropertiesViewWidget::CustomPropertiesViewWidget(QWidget *parent) :
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
    propertiesWidget = new CustomTreeBrowser();
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

    buttonModify = new QPushButton(tr("Modify"), this);
    buttonImport = new QPushButton(tr("Import"), this);
    buttonImportIn = new QPushButton(tr("Import In"), this);
    buttonExport = new QPushButton(tr("Export"), this);
    buttonModify->setEnabled(false);
    buttonImport->setEnabled(false);
    buttonImportIn->setEnabled(false);
    buttonExport->setEnabled(false);

    QGridLayout *butlayout = new QGridLayout();
    butlayout->setMargin(0);
    butlayout->addWidget(buttonModify, 0, 0);
    butlayout->addWidget(buttonImport, 0, 1);
    butlayout->addWidget(buttonExport, 1, 0);
    butlayout->addWidget(buttonImportIn, 1, 1);

    QVBoxLayout *boxlayout = new QVBoxLayout();
    boxlayout->setMargin(0);
    boxlayout->addWidget(propertiesWidget);
    boxlayout->addLayout(butlayout);
    boxlayout->setSpacing(2);
    boxlayout->setStretch(0, 1);
    boxlayout->setStretch(1, 0);
    setLayout(boxlayout);

    mQtToOgitorPropertyMap.clear();
    mOgitorToQtPropertyMap.clear();
    mPropertyConnections.clear();

    connect(buttonModify, SIGNAL(clicked()), this, SLOT(modifyClicked()));
    connect(buttonImport, SIGNAL(clicked()), this, SLOT(importClicked()));
    connect(buttonImportIn, SIGNAL(clicked()), this, SLOT(importInClicked()));
    connect(buttonExport, SIGNAL(clicked()), this, SLOT(exportClicked()));
}
//----------------------------------------------------------------------------------------
CustomPropertiesViewWidget::~CustomPropertiesViewWidget()
{
    mQtToOgitorPropertyMap.clear();
    mOgitorToQtPropertyMap.clear();

    for(unsigned int pidx = 0;pidx < mPropertyConnections.size();pidx++)
        delete mPropertyConnections[pidx];
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::OnPropertySetRebuilt(OgitorsPropertySet* set)
{
    PresentPropertiesView(mLastObject);
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::modifyClicked()
{
    if(!mLastObject)
        return;

    OgitorsCustomPropertySet set;
    mLastObject->getCustomProperties()->cloneSet(set);

    UserDefinedDialog dlg(QApplication::activeWindow(), &set);

    if(dlg.exec() == QDialog::Accepted)
    {
        mLastObject->getCustomProperties()->initFromSet(*(dlg.getSet()));
    }
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::importClicked()
{
    QString theList = tr("Custom Property Template Files (*.ctp)");
    Ogre::String templatesPath = OgitorsUtils::QualifyPath(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Templates");

    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Import Custom Properties Template"), templatesPath.c_str(), theList, 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif
    if(!path.isEmpty())
    {
        TiXmlDocument docImport(path.toStdString().c_str());

        if(!docImport.LoadFile()) 
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error Loading Template File").toStdString(), DLGTYPE_OK);
            return;
        }

        TiXmlElement* element = 0;
        element = docImport.FirstChildElement("CUSTOMPROPERTIES");
        if(!element)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error Loading Template File").toStdString(), DLGTYPE_OK);
            return;
        }

        OgitorsCustomPropertySet set;
        OgitorsUtils::ReadCustomPropertySet(element, &set);
        mLastObject->getCustomProperties()->initFromSet(set);
    }
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::importInClicked()
{
    QString theList = tr("Custom Property Template Files (*.ctp)");
    Ogre::String templatesPath = OgitorsUtils::QualifyPath(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Templates");

    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Import Custom Properties Template"), templatesPath.c_str(), theList, 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif
    if(!path.isEmpty())
    {
        TiXmlDocument docImport(path.toStdString().c_str());

        if(!docImport.LoadFile()) 
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error Loading Template File").toStdString(), DLGTYPE_OK);
            return;
        }

        TiXmlElement* element = 0;
        element = docImport.FirstChildElement("CUSTOMPROPERTIES");
        if(!element)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error Loading Template File").toStdString(), DLGTYPE_OK);
            return;
        }

        OgitorsCustomPropertySet set;
        OgitorsUtils::ReadCustomPropertySet(element, &set);
        mLastObject->getCustomProperties()->addFromSet(set);
    }
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::exportClicked()
{
    QString theList = tr("Custom Property Template Files (*.ctp)");
    Ogre::String templatesPath = OgitorsUtils::QualifyPath(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Templates");

    QString path = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Export Custom Properties Template"), templatesPath.c_str(), theList, 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif
    if(!path.isEmpty())
    {
        Ogre::String customStr = OgitorsUtils::GetCustomPropertySaveString(mLastObject->getCustomProperties(), 0);
        std::ofstream outfile(path.toStdString().c_str());
        outfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        outfile << customStr.c_str();
        outfile.close();
    }
}
//----------------------------------------------------------------------------------------
OgitorsPropertyBase *CustomPropertiesViewWidget::getOgitorProperty(QtProperty *property)
{
    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
        return it->second;
    else
        return 0;
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::PresentPropertiesView(CBaseEditor* object)
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
        mLastObject->getCustomProperties()->removeListener(this);
    
    if(object != 0)
    {
        DisplayObjectProperties(object);

        object->getCustomProperties()->addListener(this);
    }

    mLastObject = object;

    buttonModify->setEnabled(object != 0);
    buttonImport->setEnabled(object != 0);
    buttonImportIn->setEnabled(object != 0);
    buttonExport->setEnabled(object != 0 && !object->getCustomProperties()->isEmpty());
    
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
void CustomPropertiesViewWidget::createProperty(QtProperty *group, QString name, OgitorsPropertyBase *property)
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
        property->connect(OgitorsSignalFunction::from_method<CustomPropertiesViewWidget, &CustomPropertiesViewWidget::propertyChangeTracker>(this), *connection);
        mPropertyConnections.push_back(connection);

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
QtProperty *CustomPropertiesViewWidget::getPropertyGroup(Ogre::String& name, QtProperty *defaultGroup)
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
void CustomPropertiesViewWidget::OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property)
{
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property)
{
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::DisplayObjectProperties(CBaseEditor* object)
{
    lastGroup = 0;
    lastGroupName = "";
    
    OgitorsPropertyVector props = object->getCustomProperties()->getPropertyVector();
    
    Ogre::String strDisplayName;

    const OgitorsPropertyDef *propDef;

    for(unsigned int i = 0;i < props.size();i++)
    {
        propDef = props[i]->getDefinition();

        strDisplayName = propDef->getDisplayName();
        QtProperty* propertyGroup = getPropertyGroup(strDisplayName, 0);
        createProperty(propertyGroup, strDisplayName.c_str(), props[i]);
    }
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::enumValueChanged(QtProperty *property, int val)     
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
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::boolValueChanged(QtProperty *property, bool val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<bool> *ogProperty = static_cast<OgitorsProperty<bool>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::stringValueChanged(QtProperty *property, const QString& val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::String> *ogProperty = static_cast<OgitorsProperty<Ogre::String>*>(it->second);
        Ogre::String ogreStr = val.toStdString();

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::colourValueChanged(QtProperty *property, const QColor& val)
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::ColourValue> *ogProperty = static_cast<OgitorsProperty<Ogre::ColourValue>*>(it->second);
        Ogre::ColourValue ogreCol(val.redF(), val.greenF(), val.blueF(), val.alphaF());

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::doubleValueChanged(QtProperty *property, double val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Real> *ogProperty = static_cast<OgitorsProperty<Ogre::Real>*>(it->second);
        Ogre::Real ogreVal = (Ogre::Real)val;

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::quaternionValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = val.value<QRectF>();
    Ogre::Quaternion qval(rval.height(), rval.x(), rval.y(), rval.width());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Quaternion> *ogProperty = static_cast<OgitorsProperty<Ogre::Quaternion>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::vector4ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = val.value<QRectF>();
    Ogre::Vector4 qval(rval.x(), rval.y(), rval.width(),rval.height());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector4> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector4>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::vector3ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QRectF rval = val.value<QRectF>();
    Ogre::Vector3 qval(rval.x(), rval.y(), rval.width());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector3> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector3>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::vector2ValueChanged(QtProperty *property, const QVariant& val)      
{
    if(BLOCKSETFUNCTIONS)
        return;

    QPointF rval = val.value<QPointF>();
    Ogre::Vector2 qval(rval.x(), rval.y());

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        OgitorsProperty<Ogre::Vector2> *ogProperty = static_cast<OgitorsProperty<Ogre::Vector2>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + ogProperty->getDefinition()->getDisplayName() + " change");
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
void CustomPropertiesViewWidget::intValueChanged(QtProperty *property, int val) 
{
    if(BLOCKSETFUNCTIONS)
        return;

    int tmpVal;

    OgitorsProperty<int> *ogPropertyInt;

    QtToOgitorPropertyMap::const_iterator it = mQtToOgitorPropertyMap.find(property);
    if(it != mQtToOgitorPropertyMap.end())
    {
        ogPropertyInt = static_cast<OgitorsProperty<int>*>(it->second);

        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        undoMgr->BeginCollection(mLastObject->getName() + "'s Custom Property : " + it->second->getDefinition()->getDisplayName() + " change");
        ogPropertyInt->set(val);
        undoMgr->EndCollection(true);

        if(val != ogPropertyInt->get())
        {
            tmpVal = ogPropertyInt->get();
            BLOCKSETFUNCTIONS++;
            intManager->setValue(property, tmpVal);
            BLOCKSETFUNCTIONS--;
        }
    }
}
//----------------------------------------------------------------------------------------
void CustomPropertiesViewWidget::propertyChangeTracker(const OgitorsPropertyBase* property, Ogre::Any value)
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
            case PROP_INT:
                intManager->setValue(it->second, static_cast<OgitorsProperty<int>*>(ogitorProp)->get());
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
