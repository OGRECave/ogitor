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

#ifndef I_TEXT_EDITOR_CODEC_HXX
#define I_TEXT_EDITOR_CODEC_HXX

#include <QtCore/QString>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QToolBar>

//----------------------------------------------------------------------------------------

class GenericTextEditorDocument;

//----------------------------------------------------------------------------------------

/*!  
* A class that forms the base that all editor codecs have to inherit from. Its methods are
* then called from the GenericTextEditor.
*/
class ITextEditorCodec
{
public:
    ITextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon)
    {
        mGenTexEdDoc            = genTexEdDoc;
        mDocName                = docName;
        mDocumentIcon           = documentIcon;
        mUseDefaultSaveLogic    = true;
    }

    /**
    * Called before the text passed as an argument is called for the first time
    * @param text the content that was loaded
    * @return the the that is now going to be displayed. If no changes are needed to be made, 
    *         then just return the content that was passed to the function              
    */
    virtual QString onBeforeDisplay(QString text) = 0;

    /**
    * Called after the content was displayed for the first time            
    */
    virtual void    onAfterDisplay(){};

    /**
    * Called when a save request was issued, but only if the member variable mUseDefaultSaveLogic
    * was set to true. Otherwise the default saving logic of the generic text editor will be used.
    */
    virtual void    onSave(){};

    /**
    * Called after each successful save, regardless of whether it was executed by the codec's save
    * logic or whether the default save logic from the editor was used.
    */
    virtual void    onAfterSave(){};

    /**
    * Called when a context menu request was triggered.
    */
    virtual void    onContextMenu(QContextMenuEvent* event){};

    /**
    * Called when a key press event was triggered. Pay attention that some events are already processed
    * by the generic text editor itself, such as syntax completion if an completer was set for the codec.
    */
    virtual void    onKeyPressEvent(QKeyEvent* event){};
    
    /**
    * Called once on document creation, offering the possibility to add syntax highlighting to the document. 
    * Please simply call the addHighlighter method from mGenTexEdDoc .
    */
    virtual void    onAddHighlighter(){};

    /**
    * Called once on document creation, offering the possibility to add syntax completion to the document. 
    * Please simply call the addCompleter method from mGenTexEdDoc .
    */
    virtual void    onAddCompleter(){};

    /**
    * Called whenever a document that is already open in one tab is requested to be displayed again
    * and therefore will be moved to the foreground. 
    */
    virtual void    onDisplayRequest(){};

    /**
    * Called whenever the document this codec instance is attached to is made visible due to a tab change.
    */
    virtual void    onTabChange(){};

    /**
    * Returns a custom toolbar instance for this codec.
    */
    virtual QToolBar* getCustomToolBar(){ return 0; };

    /**
    * Called whenever the document this codec instance is attached to is being closed. At this stage 
    * at runtime the document was already requested to be saved if there were unsaved changes, so
    * no need to do this again in this method.
    */
    virtual void    onClose(){};

    QString         getDocumentIcon() {return mDocumentIcon;}
    void            setOptionalData(QString optionalData){mOptionalData = optionalData;};
    bool            isUseDefaultSaveLogic(){return mUseDefaultSaveLogic;};

protected:
    GenericTextEditorDocument*  mGenTexEdDoc;
    QString                     mDocName;
    QString                     mDocumentIcon;
    QString                     mOptionalData;
    bool                        mUseDefaultSaveLogic;
};

//----------------------------------------------------------------------------------------

class ITextEditorCodecFactory
{
public:
    virtual ITextEditorCodec* create(GenericTextEditorDocument* genTexEdDoc, QString docName) = 0;
};

//----------------------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------------------