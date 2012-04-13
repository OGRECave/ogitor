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

#include "xmltexteditorcodec.hxx"
#include "generictexteditor.hxx"

//-----------------------------------------------------------------------------------------
XMLTextEditorCodec::XMLTextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon) : 
ITextEditorCodec(genTexEdDoc, docName, documentIcon)
{
}
//-----------------------------------------------------------------------------------------
QString XMLTextEditorCodec::onBeforeDisplay(QString text)
{
    return text;
}
//-----------------------------------------------------------------------------------------
void XMLTextEditorCodec::onAddHighlighter()
{
    new XMLHighlighter(mGenTexEdDoc->document());
}
//-----------------------------------------------------------------------------------------
ITextEditorCodec* XMLTextEditorCodecFactory::create(GenericTextEditorDocument* genTexEdDoc, QString docName)
{
    return new XMLTextEditorCodec(genTexEdDoc, docName, ":/icons/files.svg");
}
//-----------------------------------------------------------------------------------------
XMLHighlighter::XMLHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{    
    HighlightingRule rule;
    tagAngleBracketFormat.setForeground(Qt::darkBlue);
    nameSpaceFormat.setForeground(Qt::red);
    tagKeyFormat.setForeground(Qt::darkRed);
    tagKeyFormat.setFontWeight(QFont::Bold);
    xmlCommentFormat.setForeground(Qt::darkGreen);
    xmlDefinitionFormat.setForeground(Qt::blue);

    //rule.pattern = QRegExp("<\\/?(\\w+)>");
    //rule.format = tagKeyFormat;
    //formatRules.append(rule);

    rule.pattern = QRegExp("(<\\/?)\\w+(>)");
    rule.format = tagAngleBracketFormat;
    formatRules.append(rule);

    //rule.pattern = QRegExp("(<\\?.+\\?>)");
    //rule.format = xmlDefinitionFormat;
    //formatRules.append(rule);

    //rule.pattern = QRegExp("(xmlns.+\\\")");
    //rule.format = nameSpaceFormat;
    //formatRules.append(rule);

    commentStartExpression = QRegExp("<!--");
    commentEndExpression = QRegExp("-->");

    //rule.pattern = QRegExp("(<!--.+-->)");
    //rule.format = xmlCommentFormat;
    //formatRules.append(rule);
}
//-----------------------------------------------------------------------------------------
void XMLHighlighter::highlightBlock(const QString &text)
{
    _highlightSimpleRegexList(text, formatRules);
    
    setCurrentBlockState(0);

    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while(startIndex >= 0) 
    {
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;
        if (endIndex == -1) 
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } 
        else 
        {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, xmlCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
//-----------------------------------------------------------------------------------------
void XMLHighlighter::_highlightSimpleRegexList(const QString text, const QVector<HighlightingRule> rules)
{
    int index;
    int length;

    foreach(HighlightingRule rule, rules) 
    {
        index = 0;
        QRegExp expression(rule.pattern);

        if((index = expression.indexIn(text, index)) != -1)
        {
            for(int i = 0; i < expression.captureCount(); i++)
            {
                length = expression.cap(i).length();
                setFormat(index, length, rule.format);
                index += length;
            }
            
        
        }
            //while((index = expression.indexIn(text, index)) != -1)
            //{
            //    expression.captureCount()
            //        length = expression.matchedLength();
            //    setFormat(index, length, rule.format);
            //    index += length;
            //}
    }
}
//-----------------------------------------------------------------------------------------