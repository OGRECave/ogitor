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
    m_TagFormat.setForeground(Qt::blue);
    m_AttributeFormat.setForeground(Qt::red);
    m_CommentFormat.setForeground(Qt::darkGray);
    m_LiteralFormat.setForeground(QColor(150, 0, 210));

    // XML Tags
    rule.pattern = QRegExp("<[\\/\\w*:?\\w*][^>]*>");
    rule.format = m_TagFormat;
    m_FormatRules.append(rule);

    // String Literals
    rule.pattern = QRegExp("\\\"[^\\\"]*\\\"");
    rule.format = m_LiteralFormat;
    m_FormatRules.append(rule);

    // XML Comments
    m_CommentStartExpression = QRegExp("<!--");
    m_CommentEndExpression = QRegExp("-->");

    // XML Attributes
    rule.pattern = QRegExp("(\\S*)=");
    rule.format = m_AttributeFormat;
    m_FormatRules.append(rule);
}
//-----------------------------------------------------------------------------------------
void XMLHighlighter::highlightBlock(const QString &text)
{
    /*int index;
    int length;*/

    setCurrentBlockState(0);

    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = text.indexOf(m_CommentStartExpression);

    while(startIndex >= 0)
    {
        int endIndex = text.indexOf(m_CommentEndExpression, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + m_CommentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, m_CommentFormat);
        startIndex = text.indexOf(m_CommentStartExpression, startIndex + commentLength);
    }

    _highlightSimpleRegexList(text, m_FormatRules);
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

        while((index = expression.indexIn(text, index)) != -1)
        {
            length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index += length;
        }
    }
}
//-----------------------------------------------------------------------------------------