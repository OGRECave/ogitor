/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include <QtGui/QVBoxLayout>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "userdefined.hxx"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"


using namespace Ogitors; 

int typeToName[] = {-1, -1, 0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, 8};

//---------------------------------------------------------------------------------------
void TreeBrowser::contextMenuEvent(QContextMenuEvent *evt)
{
    mContextItem = currentItem();
    if(mContextItem == 0)
        return;

    QtProperty *property = mContextItem->property();

    if(property->whatsThis().toInt() > 5)
    {
        QMenu *contextMenu = new QMenu(this);
        contextMenu->addAction(tr("Remove Option"), this, SLOT(removeOption()));
        contextMenu->exec(QCursor::pos());
        delete contextMenu;
    }

    evt->accept();
}
//----------------------------------------------------------------------------------------
void TreeBrowser::removeOption()
{
    QtProperty *property = mContextItem->property();
    int optionId = (property->whatsThis().toInt() - 6) / 2;

    PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(mDefinition->getOptions());
    (*options).erase((*options).begin() + optionId); 
    static_cast<UserDefinedDialog*>(mContainerDialog)->listSelectionChanged();
}
//----------------------------------------------------------------------------------------
UserDefinedDialog::UserDefinedDialog(QWidget *parent, OgitorsCustomPropertySet *set) :
    QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    BLOCKSETFUNCTIONS = 0;
    mPropertyTypes.append("Integer");
    mPropertyTypes.append("Float");
    mPropertyTypes.append("String");
    mPropertyTypes.append("Vector2");
    mPropertyTypes.append("Vector3");
    mPropertyTypes.append("Vector4");
    mPropertyTypes.append("Colour");
    mPropertyTypes.append("Boolean");
    mPropertyTypes.append("Quaternion");
    mPropertyTypes.append("Material");
    mPropertyTypes.append("Mesh");
    mPropertyTypes.append("Target");

    mCurrentDisplayedProperty = 0;

    setupUi(this);
    mIconLabel->setScaledContents(true);
    mIconLabel->setPixmap(QPixmap(":/icons/light.png"));
    mBtnUp->setIcon(QIcon(":/icons/uparrow.svg"));
    mBtnDown->setIcon(QIcon(":/icons/downarrow.svg"));

    groupManager = new QtGroupPropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    enumManager = new QtEnumPropertyManager(this);

    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);

    mPropertyList = new QTableWidget(this);
    mPropertyList->setColumnCount(2);
    QStringList labels;
    labels.append(tr("Property Name"));
    labels.append(tr("Property Type"));
    mPropertyList->setHorizontalHeaderLabels(labels);
    mPropertyList->setAlternatingRowColors(true);
    mPropertyList->setColumnWidth(0,144);
    mPropertyList->setColumnWidth(1,80);
    mPropertyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mPropertyList->setSelectionBehavior(QAbstractItemView::SelectRows);

    mPropertiesWidget = new TreeBrowser(this);
    mPropertiesWidget->setFactoryForManager(enumManager, comboBoxFactory);
    mPropertiesWidget->setFactoryForManager(stringManager, lineEditFactory);
    mPropertiesWidget->setAlternatingRowColors(true);
    mPropertiesWidget->setIndentation(10);

    mLayout->addWidget(mPropertyList);

    mAddOptionButton = new QPushButton(tr("Add Option"), this);
    
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(mPropertiesWidget);
    vlayout->addWidget(mAddOptionButton);
    vlayout->setStretch(0,1);
    vlayout->setStretch(1,0);
    
    mLayout->addLayout(vlayout);

    connect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(stringValueChanged(QtProperty *, const QString &)));
    connect(enumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(enumValueChanged(QtProperty *, int)));
    connect(mPropertyList, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionChanged()));
    connect(mBtnUp, SIGNAL(clicked()), this, SLOT(upClicked()));
    connect(mBtnDown, SIGNAL(clicked()), this, SLOT(downClicked()));
    connect(mBtnDelete, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(mBtnNew, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(mAddOptionButton, SIGNAL(clicked()), this, SLOT(addOptionClicked()));

    mBtnUp->setEnabled(false);
    mBtnDown->setEnabled(false);
    mBtnDelete->setEnabled(false);
    mAddOptionButton->setEnabled(false);

    mSet = set;
    DisplayProperties();
}
//----------------------------------------------------------------------------------------
UserDefinedDialog::~UserDefinedDialog()
{
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::DisplayProperties()
{
    OgitorsPropertyVector vec = mSet->getPropertyVector();
    mPropertyList->clearContents();
    mPropertyList->setRowCount(vec.size());
    for(unsigned int i = 0;i < vec.size();i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(QString(vec[i]->getName().c_str()));
        mPropertyList->setItem(i, 0, item);
        int proptype = typeToName[vec[i]->getType()];
        if(vec[i]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_MATERIAL)
            proptype = 9;
        else if(vec[i]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_MESH)
            proptype = 10;
        else if(vec[i]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_TARGET)
            proptype = 11;

        item = new QTableWidgetItem(mPropertyTypes[proptype]);
        mPropertyList->setItem(i, 1, item);
        mPropertyList->setRowHeight(i, 18);
    }
    if(vec.size() > 0)
        mPropertyList->selectRow(0);
    else
        UpdateInterface();
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::UpdateInterface()
{
    QTableWidgetItem *item = mPropertyList->currentItem();
    if(item)
    {
        mBtnUp->setEnabled(item->row() > 0);
        mBtnDown->setEnabled(item->row() < (mPropertyList->rowCount() - 1));
        mBtnDelete->setEnabled(true);
    }
    else
    {
        mBtnUp->setEnabled(false);
        mBtnDown->setEnabled(false);
        mBtnDelete->setEnabled(false);

        mPropertiesWidget->clear();
        stringManager->clear();
        enumManager->clear();
        groupManager->clear();
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::listSelectionChanged()
{
    UpdateInterface();

    QTableWidgetItem *item = mPropertyList->currentItem();
    BLOCKSETFUNCTIONS++;
    
    mPropertiesWidget->clear();
    stringManager->clear();
    groupManager->clear();
    enumManager->clear();
    
    if(item)
    {
        int row = item->row();
        OgitorsPropertyVector vec = mSet->getPropertyVector();
        
        if((int)vec.size() > row)
        {
            mCurrentDisplayedProperty = vec[row];
            
            QtProperty *prop = stringManager->addProperty(tr("Property Name"));
            prop->setWhatsThis("0");
            stringManager->setValue(prop, QString(vec[row]->getName().c_str()));
            mPropertiesWidget->addProperty(prop);
            
            prop = enumManager->addProperty(tr("Property Type"));
            prop->setWhatsThis("1");
            enumManager->setEnumNames(prop, mPropertyTypes);

            int proptype = typeToName[vec[row]->getType()];
            if(vec[row]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_MATERIAL)
                proptype = 9;
            else if(vec[row]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_MESH)
                proptype = 10;
            else if(vec[row]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_TARGET)
                proptype = 11;

            enumManager->setValue(prop, proptype);

            mPropertiesWidget->addProperty(prop);
            mAddOptionButton->setEnabled(false);
            switch(vec[row]->getType())
            {
            case PROP_VECTOR2:
                prop = stringManager->addProperty(tr("Field Name X"));
                prop->setWhatsThis("2");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(0).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name Y"));
                prop->setWhatsThis("3");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(1).c_str()));
                mPropertiesWidget->addProperty(prop);
                break;
            case PROP_VECTOR3:
                prop = stringManager->addProperty(tr("Field Name X"));
                prop->setWhatsThis("2");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(0).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name Y"));
                prop->setWhatsThis("3");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(1).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name Z"));
                prop->setWhatsThis("4");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(2).c_str()));
                mPropertiesWidget->addProperty(prop);
                break;
            case PROP_VECTOR4:
                prop = stringManager->addProperty(tr("Field Name X"));
                prop->setWhatsThis("2");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(0).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name Y"));
                prop->setWhatsThis("3");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(1).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name Z"));
                prop->setWhatsThis("4");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(2).c_str()));
                mPropertiesWidget->addProperty(prop);
                prop = stringManager->addProperty(tr("Field Name W"));
                prop->setWhatsThis("5");
                stringManager->setValue(prop, QString(vec[row]->getDefinition()->getFieldName(3).c_str()));
                mPropertiesWidget->addProperty(prop);
                break;
            case PROP_STRING:
            case PROP_INT:
            case PROP_REAL: mAddOptionButton->setEnabled(true && (vec[row]->getDefinition()->getAutoOptionType() == AUTO_OPTIONS_NONE));break;
            };

            const OgitorsPropertyDef *definition = vec[item->row()]->getDefinition();
            const PropertyOptionsVector *options = definition->getOptions();

            mPropertiesWidget->setDefinition(definition);

            if(options && definition->getAutoOptionType() == AUTO_OPTIONS_NONE)
            {
                QtProperty *optGroup = groupManager->addProperty(tr("Options"));
                for(unsigned int o = 0;o < options->size();o++)
                {
                    prop = stringManager->addProperty(tr("Key %1").arg(o));
                    prop->setWhatsThis(QString("%1").arg((o * 2) + 6));
                    stringManager->setValue(prop, QString((*options)[o].mKey.c_str()));
                    optGroup->addSubProperty(prop);

                    prop = stringManager->addProperty(tr("Value %1").arg(o));
                    prop->setWhatsThis(QString("%1").arg((o * 2) + 7));
                    OgitorsPropertyValue propVal;
                    propVal.propType = definition->getType();
                    propVal.val = (*options)[o].mValue;
                    stringManager->setValue(prop, QString(OgitorsUtils::GetValueString(propVal).c_str()));
                    optGroup->addSubProperty(prop);
                }
                mPropertiesWidget->addProperty(optGroup);
            }
        }
    }
    BLOCKSETFUNCTIONS--;
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::upClicked()
{
    QTableWidgetItem *item = mPropertyList->currentItem();
    if(item)
    {
        int row = item->row() - 1;

        OgitorsPropertyVector vec = mSet->getPropertyVector();
        OgitorsPropertyVector vec2;
        vec2.clear();
        int i;
        for(i = 0;i < row;i++)
            vec2.push_back(vec[i]);

        vec2.push_back(vec[row + 1]);
        vec2.push_back(vec[row]);

        for(i = row + 2;i < (int)vec.size();i++)
            vec2.push_back(vec[i]);

        mSet->_setPropertyVector(vec2);

        mPropertyList->insertRow(row);
        mPropertyList->setRowHeight(row, 18);
        QTableWidgetItem *move1 = mPropertyList->takeItem(row + 2, 0);
        QTableWidgetItem *move2 = mPropertyList->takeItem(row + 2, 1);
        mPropertyList->setItem(row, 0, move1);       
        mPropertyList->setItem(row, 1, move2);
        mPropertyList->removeRow(row + 2);
        mPropertyList->selectRow(row);
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::downClicked()
{
    QTableWidgetItem *item = mPropertyList->currentItem();
    if(item)
    {
        int row = item->row() + 2;

        OgitorsPropertyVector vec = mSet->getPropertyVector();
        OgitorsPropertyVector vec2;
        vec2.clear();
        int i;
        for(i = 0;i < row - 2;i++)
            vec2.push_back(vec[i]);

        vec2.push_back(vec[row - 1]);
        vec2.push_back(vec[row - 2]);
        for(i = row;i < (int)vec.size();i++)
            vec2.push_back(vec[i]);

        mSet->_setPropertyVector(vec2);

        mPropertyList->insertRow(row);
        mPropertyList->setRowHeight(row, 18);
        QTableWidgetItem *move1 = mPropertyList->takeItem(row - 2, 0);
        QTableWidgetItem *move2 = mPropertyList->takeItem(row - 2, 1);
        mPropertyList->setItem(row, 0, move1);       
        mPropertyList->setItem(row, 1, move2);
        mPropertyList->removeRow(row - 2);
        mPropertyList->selectRow(row - 1);
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::addClicked()
{
    Ogre::String propertyName;
    int i = 0;

    while(mSet->hasProperty("Property#" + Ogre::StringConverter::toString(i)))
        i++;

    propertyName = "Property#" + Ogre::StringConverter::toString(i);

    int row = mPropertyList->rowCount();
    mPropertyList->insertRow(row);
    
    QTableWidgetItem *item = new QTableWidgetItem(QString(propertyName.c_str()));
    mPropertyList->setItem(row, 0, item);
    item = new QTableWidgetItem("String");
    mPropertyList->setItem(row, 1, item);
    mPropertyList->setRowHeight(row, 18);

    mSet->addProperty(propertyName, PROP_STRING);
    mPropertyList->selectRow(row);
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::deleteClicked()
{
    QTableWidgetItem *item = mPropertyList->currentItem();
    if(item)
    {
        int row = item->row();

        OgitorsPropertyVector vec = mSet->getPropertyVector();
        mSet->removeProperty(vec[row]->getName());

        mPropertyList->removeRow(row);
        
        UpdateInterface();
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::addOptionClicked()
{
    QTableWidgetItem *item = mPropertyList->currentItem();
    if(item)
    {
        OgitorsPropertyVector vec = mSet->getPropertyVector();
        OgitorsPropertyDef *definition = const_cast<OgitorsPropertyDef*>(vec[item->row()]->getDefinition());
        PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(definition->getOptions());

        if(!options)
        {
            options = new PropertyOptionsVector;
        }

        int optNum = options->size();
        Ogre::String optNo = Ogre::StringConverter::toString(optNum);
        switch(definition->getType())
        {
        case PROP_STRING: options->push_back(PropertyOption("option #" + optNo, Ogre::Any(optNo)));break;
        case PROP_INT: options->push_back(PropertyOption("option #" + optNo, Ogre::Any(optNum)));break;
        case PROP_REAL: options->push_back(PropertyOption("option #" + optNo, Ogre::Any((Ogre::Real)optNum)));break;
        }
        
        definition->setOptions(options);

        listSelectionChanged();
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::stringValueChanged(QtProperty *property, const QString& val)
{
    if(BLOCKSETFUNCTIONS || !mCurrentDisplayedProperty)
        return;
    
    int i = property->whatsThis().toInt();
    QTableWidgetItem *item = mPropertyList->currentItem();
    int row = item->row();

    OgitorsPropertyDef *def = const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition());

    switch(i)
    {
    case 0: mSet->changePropertyName(mCurrentDisplayedProperty->getName(), val.toStdString());
            mPropertyList->item(row, 0)->setText(val);
            break;
    case 2: def->setFieldName(0, val.toStdString());
            break;
    case 3: def->setFieldName(1, val.toStdString());
            break;
    case 4: def->setFieldName(2, val.toStdString());
            break;
    case 5: def->setFieldName(3, val.toStdString());
            break;
    }

    if(i > 5)
    {
        int optID = (i - 6) / 2;
        int optType = (i - 6) & 0x1;

        PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(def->getOptions());

        switch(optType)
        {
        case 0: {
                   for(unsigned int o = 0; o < options->size();o++)
                   {
                       if(o == optID)
                           continue;

                       if((*options)[o].mKey == val.toStdString())
                       {
                           BLOCKSETFUNCTIONS++;
                           stringManager->setValue(property, (*options)[optID].mKey.c_str());
                           BLOCKSETFUNCTIONS--;
                           return;
                       }
                   }
                   (*options)[optID].mKey = val.toStdString();
                   break;
                }
        case 1: {
                   (*options)[optID].mValue = OgitorsPropertyValue::createFromString(def->getType(), val.toStdString()).val;
                    OgitorsPropertyValue propVal;
                    propVal.propType = def->getType();
                    propVal.val = (*options)[optID].mValue;
                    BLOCKSETFUNCTIONS++;
                    stringManager->setValue(property, QString(OgitorsUtils::GetValueString(propVal).c_str()));
                    BLOCKSETFUNCTIONS--;
                    break;
                }
        }
    }
}
//----------------------------------------------------------------------------------------
void UserDefinedDialog::enumValueChanged(QtProperty *property, int val)
{
    if(BLOCKSETFUNCTIONS || !mCurrentDisplayedProperty)
        return;
    
    int i = property->whatsThis().toInt();
    QTableWidgetItem *item = mPropertyList->currentItem();
    int row = item->row();

    if(i == 1)
    {
        OgitorsPropertyType pType;
        if(val == 0)
            pType = PROP_INT;
        else if(val == 1)
            pType = PROP_REAL;
        else if(val == 2)
            pType = PROP_STRING;
        else if(val == 3)
            pType = PROP_VECTOR2;
        else if(val == 4)
            pType = PROP_VECTOR3;
        else if(val == 5)
            pType = PROP_VECTOR4;
        else if(val == 6)
            pType = PROP_COLOUR;
        else if(val == 7)
            pType = PROP_BOOL;
        else if(val == 8)
            pType = PROP_QUATERNION;
        else if(val == 9)
            pType = PROP_STRING;
        else if(val == 10)
            pType = PROP_STRING;
        else if(val == 11)
            pType = PROP_STRING;

        mSet->changePropertyType(mCurrentDisplayedProperty->getName(), pType);
        mPropertyList->item(row, 1)->setText(mPropertyTypes[val]);
        listSelectionChanged();
        if(val == 9)
        {
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setAutoOptionType(AUTO_OPTIONS_MATERIAL);
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setOptions(OgitorsRoot::GetMaterialNames());
        }
        else if(val == 10)
        {
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setAutoOptionType(AUTO_OPTIONS_MESH);
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setOptions(OgitorsRoot::GetModelNames());
        }
        else if(val == 11)
        {
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setAutoOptionType(AUTO_OPTIONS_TARGET);
            const_cast<OgitorsPropertyDef*>(mCurrentDisplayedProperty->getDefinition())->setOptions(OgitorsRoot::GetAutoTrackTargets());
        }

        listSelectionChanged();
    }
}
//----------------------------------------------------------------------------------------

