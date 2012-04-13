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

#ifndef GENERIC_TEXT_EDITOR_HXX
#define GENERIC_TEXT_EDITOR_HXX

#include "generictexteditordocument.hxx"
#include "generictexteditorcodec.hxx"

#include "Ogitors.h"
#include "OgreSingleton.h"
#include "OgitorsDefinitions.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef GENERICTEXTEDITOR_EXPORT
     #define GTEExport __declspec (dllexport)
   #else
     #define GTEExport __declspec (dllimport)
   #endif
#else
   #define GTEExport
#endif

typedef std::map<std::string, ITextEditorCodecFactory*> TextCodecExtensionFactoryMap;

//-----------------------------------------------------------------------------------------

class GTEExport GenericTextEditor : public QMdiArea, public Ogre::Singleton<GenericTextEditor>
{
    Q_OBJECT

public:
    GenericTextEditor(QString editorName, QWidget *parent = 0);
    ~GenericTextEditor();

    bool displayTextFromFile(QString filePath, QString optionalData = "");
    bool displayText(QString docName, QString text, QString extension = "", QString optionalData = "");
    void moveToForeground();
    void saveAll();

    static void registerCodecFactory(QString extension, ITextEditorCodecFactory* codec);
    static void unregisterCodecFactory(QString extension);
    static ITextEditorCodecFactory* findMatchingCodecFactory(QString extensionOrFileName);

    static QStringListModel* modelFromFile(const QString& fileName);

    inline void setAllowDoubleDisplay(bool allow) {mAllowDoubleDisplay = allow;}
    inline bool isAllowDoubleDisplay() {return mAllowDoubleDisplay;}

    void onModifiedStateChanged(Ogitors::IEvent* evt);
    void onLoadStateChanged(Ogitors::IEvent* evt);

    GenericTextEditorDocument* getActiveDocument() { return mActiveDocument; }
    void setActiveDocument(GenericTextEditorDocument* document);

signals:
    void currentChanged(int);

public slots:
    void tabContentChange();
    void pasteAvailable();
    void onSave();
    void onClipboardChanged();

protected:
    bool isPathAlreadyShowing(QString filePath, GenericTextEditorDocument*& document);
    bool isDocAlreadyShowing(QString docName, GenericTextEditorDocument*& document);
    void closeEvent(QCloseEvent *event);

private slots:
    void closeTab(int index);

private:
    void closeActiveDocument();
    void addTab(GenericTextEditorDocument* newDocument, ITextEditorCodec* codec);

    static TextCodecExtensionFactoryMap mRegisteredCodecFactories;   
    bool mAllowDoubleDisplay;

    QTabWidget *mParentTabWidget;
    QTabBar *mTabBar;
    GenericTextEditorDocument *mActiveDocument;

    QToolBar   *mMainToolBar;
    QAction    *mActSave;
    QAction    *mActEditCut;
    QAction    *mActEditCopy;
    QAction    *mActEditPaste;
};

#endif

