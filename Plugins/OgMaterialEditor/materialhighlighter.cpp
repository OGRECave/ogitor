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
#include "materialhighlighter.hxx"

MaterialHighlighter::MaterialHighlighter(QStringListModel* keywords, QStringListModel* enums, 
                                         QStringListModel* dataTypes, QTextDocument *parent) : 
                                         QSyntaxHighlighter(parent)
{    
    HighlightingRule rule;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    enumFormat.setForeground(Qt::darkMagenta);
    dataTypeFormat.setForeground(Qt::darkCyan);
    dataTypeFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(Qt::darkRed);
    commentFormat.setForeground(Qt::darkGreen);

    foreach(QString pattern, keywords->stringList()) 
    {
        pattern = "\\b" + pattern + "\\b";
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        keywordRules.append(rule);
    }

    foreach(QString pattern, enums->stringList()) 
    {
        pattern = "\\b" + pattern + "\\b";
        rule.pattern = QRegExp(pattern);
        rule.format = enumFormat;
        enumRules.append(rule);
    }

    foreach(QString pattern, dataTypes->stringList()) 
    {
        pattern = "\\b" + pattern + "\\b";
        rule.pattern = QRegExp(pattern);
        rule.format = dataTypeFormat;
        dataTypeRules.append(rule);
    }

    rule.pattern = QRegExp("\"([a-zA-Z0-9\\.\\-\\_]+)\\s*([a-zA-Z0-9-]*)\"");
    rule.format = stringFormat;
    formatRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat;
    formatRules.append(rule);

    rule.pattern = QRegExp("texture[^_]([a-zA-Z0-9\\.\\-\\_]+)*([a-zA-Z0-9-]*)");
    textureFormat.setForeground(Qt::darkGray);
    textureFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.format = textureFormat;
    valueRules.append(rule);

    rule.pattern = QRegExp("ambient[^_]([0-9].+)$");
    textureFormat.setForeground(Qt::darkGray);
    textureFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.format = textureFormat;
    valueRules.append(rule);

    rule.pattern = QRegExp("diffuse[^_]([0-9].+)$");
    textureFormat.setForeground(Qt::darkGray);
    textureFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.format = textureFormat;
    valueRules.append(rule);

    rule.pattern = QRegExp("specular[^_]([0-9].+)$");
    textureFormat.setForeground(Qt::darkGray);
    textureFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.format = textureFormat;
    valueRules.append(rule);

    rule.pattern = QRegExp("emissive[^_]([0-9].+)$");
    textureFormat.setForeground(Qt::darkGray);
    textureFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.format = textureFormat;
    valueRules.append(rule);
}
//-----------------------------------------------------------------------------------------
void MaterialHighlighter::highlightBlock(const QString &text)
{
    int index;
    int length;

    _highlightSimpleRegexList(text, keywordRules);
    _highlightSimpleRegexList(text, enumRules);
    _highlightSimpleRegexList(text, dataTypeRules);
    
    foreach(HighlightingRule rule, valueRules) 
    {
        index = 0;
        QRegExp expression(rule.pattern);

        while((index = expression.indexIn(text, index)) != -1)
        {
            length = expression.matchedLength();
            setFormat(index + length - expression.cap(1).length(), expression.cap(1).length(), rule.format);
            index += length;
        }
    }

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
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    _highlightSimpleRegexList(text, formatRules);
}
//-----------------------------------------------------------------------------------------
void MaterialHighlighter::_highlightSimpleRegexList(const QString text, const QVector<HighlightingRule> rules)
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
