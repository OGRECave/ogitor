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
#ifdef OGITOR_MESHMAGICK_INTERFACE

#ifndef __MAGICKWIDGET_HXX__
#define __MAGICKWIDGET_HXX__

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <QtCore/QSettings>
#include <Ogre.h>
#include "qttreepropertybrowser.h"

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
class TiXmlElement;

#if defined(Q_WS_X11)
#include <QtGui/QX11Info>
#endif

class MagickDisplayWidget : public QWidget
{
    Q_OBJECT;

public:
    MagickDisplayWidget(QWidget *parent=0);

    virtual ~MagickDisplayWidget();

    void setImage(const QString& file);

protected:
    QPixmap *mImage;

    void paintEvent(QPaintEvent* evt);
    void dragEnterEvent(QDragEnterEvent *evt);
    void dropEvent(QDropEvent *evt);
};


class MagickWidget : public QWidget
{
    Q_OBJECT;
    friend class MagickDisplayWidget;

public:
    MagickWidget(QWidget *parent=0);

    virtual ~MagickWidget();

public Q_SLOTS:
    void openMesh();
    void saveMesh();
    void saveMeshAs();
    void toolTransform();
    void toolScale();
    void toolRotate();

protected:
    MagickDisplayWidget     *mDisplayWidget;
    QToolBar                *mToolBar;
    QAction                 *actOpen;
    QAction                 *actSave;
    QAction                 *actSaveAs;
    QAction                 *actMove;
    QAction                 *actRotate;
    QAction                 *actScale;

    QtTreePropertyBrowser   *propertiesWidget;
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

    QString                  mMeshFileName;

    void createActions();
    void loadMesh(Ogre::MeshPtr pMesh);
};

#endif // __MAGICKWIDGET_HXX__

#endif // OGITOR_MESHMAGICK_INTERFACE