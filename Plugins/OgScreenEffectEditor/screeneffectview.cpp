///*/////////////////////////////////////////////////////////////////////////////////
///// An
/////    ___   ____ ___ _____ ___  ____
/////   / _ \ / ___|_ _|_   _/ _ \|  _ \
/////  | | | | |  _ | |  | || | | | |_) |
/////  | |_| | |_| || |  | || |_| |  _ <
/////   \___/ \____|___| |_| \___/|_| \_\
/////                              File
/////
///// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
////
///// The MIT License
/////
///// Permission is hereby granted, free of charge, to any person obtaining a copy
///// of this software and associated documentation files (the "Software"), to deal
///// in the Software without restriction, including without limitation the rights
///// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///// copies of the Software, and to permit persons to whom the Software is
///// furnished to do so, subject to the following conditions:
/////
///// The above copyright notice and this permission notice shall be included in
///// all copies or substantial portions of the Software.
/////
///// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///// THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////////*/
//
//#include <QtCore/QSignalMapper>
//#include <QtGui/QtGui>
//#include <QtGui/QVBoxLayout>
//#include <QtGui/QDragMoveEvent>
//#include <QtGui/QContextMenuEvent>
//#include <QtGui/QMenu>
//#include <QtGui/QMessageBox>
//#include <QtCore/QTextCodec>
//
//#include "OgitorsPrerequisites.h"
//#include "BaseEditor.h"
//#include "ScreenEffectEditor.h"
//#include "DefaultEvents.h"
//#include "EventManager.h"
//
//#include "screeneffectview.hxx"
//
//using namespace Ogitors;
//
//ScreenEffectViewWidget *mScreenEffectViewWidget = 0;
//
////----------------------------------------------------------------------------------------
//QString ConvertToQString(Ogre::UTFString& value)
//{
//    QByteArray encodedString((const char*)value.data(), value.size() * 2);
//    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
//    return codec->toUnicode(encodedString);
//}
////----------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------
//ScreenEffectViewWidget::ScreenEffectViewWidget(QWidget *parent) : QWidget(parent), mScreenEffectEditor(0)
//{
//    treeWidget = new MaterialTreeWidget(this);
//
//    QVBoxLayout *boxlayout = new QVBoxLayout(this);
//    boxlayout->setMargin(0);
//    boxlayout->addWidget(treeWidget);
//
//    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
//
//    Ogitors::EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(MaterialViewWidget, onSceneLoadStateChange));
//    Ogitors::EventManager::getSingletonPtr()->connectEvent(EventManager::GLOBAL_PREPARE_VIEW, this, true, 0, true, 0, EVENT_CALLBACK(MaterialViewWidget, onGlobalPrepareView));
//}
////----------------------------------------------------------------------------------------
//MaterialViewWidget::~MaterialViewWidget()
//{
//    destroyScene();
//    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE, this);
//    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(EventManager::GLOBAL_PREPARE_VIEW, this);
//}
////----------------------------------------------------------------------------------------
//void MaterialViewWidget::destroyScene()
//{
//    if(mMaterialEditor)
//        mMaterialEditor->destroy();
//    mMaterialEditor = 0;
//
//    if(mTechniqueEditor)
//        mTechniqueEditor->destroy();
//    mTechniqueEditor = 0;
//
//    treeWidget->clear();
//}
////----------------------------------------------------------------------------------------
//void MaterialViewWidget::selectionChanged()
//{
//    QList<QTreeWidgetItem*> list = treeWidget->selectedItems();
//    if(list.size() == 0)
//    {
//        if(OgitorsRoot::getSingletonPtr()->GetSelection())
//            OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(0);
//        return;
//    }
//
//    if(list[0]->parent() == 0)
//    {
//        if(OgitorsRoot::getSingletonPtr()->GetSelection())
//            OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(0);
//        return;
//    }
//
//    QTreeWidgetItem *selitem = list[0];
//    int level = 0;
//    int ID = selitem->whatsThis(0).toInt();
//
//    while(selitem->parent()->parent() != 0)
//    {
//        selitem = selitem->parent();
//        ++level;
//    }
//
//    QString name = selitem->text(0);
//
//    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
//
//    if(!mMaterialEditor || name.toStdString() != mMaterialEditor->getName() || level == mMaterialEditor->getPropertyLevel() || ID == mMaterialEditor->getPropertyID())
//    {
//        ogRoot->GetSelection()->setSelection(0);
//
//        if(mMaterialEditor)
//            mMaterialEditor->destroy();
//
//        CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->GetRootEditor();
//        OgitorsPropertyValueMap params;
//        OgitorsPropertyValue value;
//        value.propType = PROP_STRING;
//        value.val = name.toStdString();
//        params["name"] = value;
//
//        CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Material");
//        mMaterialEditor = (CMaterialEditor*)factory->CreateObject(&parent, params);
//        if(!mMaterialEditor)
//            return;
//
//        if(mMaterialEditor->load())
//        {
//            treeWidget->mMaterialEditor = mMaterialEditor;
//            mMaterialEditor->setPropertiesLevel(level, ID);
//            mMaterialEditor->setSelected(true);
//
//            Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialEditor->getHandle());
//            try
//            {
//                std::string resourceGroup(Ogre::ResourceGroupManager::getSingletonPtr()->findGroupContainingResource(material->getOrigin()));
//                Ogre::FileInfoListPtr fileInfoList(Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(resourceGroup, material->getOrigin()));
//                Ogre::String materialPath = fileInfoList.getPointer()->at(0).archive->getName() + "/" + fileInfoList.getPointer()->at(0).filename;
//                mMaterialEditor->setMaterialPath(materialPath);
//            }
//            catch(Ogre::Exception e)
//            {
//                QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("This is only an internal, temporary material and therefore there is no displayable code available."));
//            }
//        }
//    }
//}
////----------------------------------------------------------------------------------------
//void MaterialViewWidget::prepareView()
//{
//    treeWidget->clear();
//
//    PropertyOptionsVector *materials = OgitorsRoot::getSingletonPtr()->GetMaterialNames();
//
//    Ogre::String dumtitle = "";
//    QTreeWidgetItem* rootitem = 0;
//    QTreeWidgetItem* matitem = 0;
//    QTreeWidgetItem* techitem = 0;
//    QTreeWidgetItem* passitem = 0;
//
//    rootitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Project Materials")));
//
//    QString iconpath(std::string(Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/project.svg").c_str());
//    rootitem->setIcon(0, QIcon(iconpath));
//    QFont fnt = rootitem->font(0);
//    fnt.setBold(true);
//    rootitem->setFont(0, fnt);
//
//    for(unsigned int i = 0;i < (*materials).size();i++)
//    {
//        Ogre::MaterialPtr MatPtr = Ogre::MaterialManager::getSingletonPtr()->getByName((*materials)[i].mKey);
//        if(MatPtr.isNull())
//            continue;
//
//        matitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString((*materials)[i].mKey.c_str())));
//        matitem->setIcon(0, QIcon(":/icons/material.svg"));
//        rootitem->addChild(matitem);
//
//        for(unsigned int t = 0;t < MatPtr->getNumTechniques();t++)
//        {
//            Ogre::Technique *tech = MatPtr->getTechnique(t);
//            dumtitle = "Technique " + Ogre::StringConverter::toString(t) + ": (" + tech->getName() + ")";
//            techitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
//            techitem->setIcon(0, QIcon(":/icons/technique.svg"));
//            techitem->setWhatsThis(0, QString("%1").arg(t << 8));
//            matitem->addChild(techitem);
//
//            for(unsigned int p = 0;p < tech->getNumPasses();p++)
//            {
//                Ogre::Pass *pass = tech->getPass(p);
//                dumtitle = "Pass " + Ogre::StringConverter::toString(p) + ": (" + pass->getName() + ")";
//                passitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
//                passitem->setIcon(0, QIcon(":/icons/pass.svg"));
//                passitem->setWhatsThis(0, QString("%1").arg((t << 8) + p));
//                techitem->addChild(passitem);
//            }
//        }
//    }
//
//    treeWidget->insertTopLevelItem(0, rootitem);
//}
////----------------------------------------------------------------------------------------
//void MaterialViewWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
//{
//    Ogitors::LoadStateChangeEvent *change_event = Ogitors::event_cast<Ogitors::LoadStateChangeEvent*>(evt);
//
//    if(change_event)
//    {
//        // Reload the zone selection widget when a scene is loaded
//        LoadState state = change_event->getType();
//
//        if(state == LS_LOADED)
//            prepareView();
//        else if(state == LS_UNLOADED)
//            destroyScene();
//    }
//}
////----------------------------------------------------------------------------------------
//void MaterialViewWidget::onGlobalPrepareView(Ogitors::IEvent* evt)
//{
//    prepareView();
//}
////----------------------------------------------------------------------------------------
