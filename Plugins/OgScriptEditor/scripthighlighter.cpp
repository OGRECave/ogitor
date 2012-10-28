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

#include "scripthighlighter.hxx"

ScriptHighlighter::ScriptHighlighter(QStringListModel* keywords, QTextDocument *parent) : QSyntaxHighlighter(parent)
{    
    HighlightingRule rule;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(Qt::darkRed);

    foreach(QString pattern, keywords->stringList()) 
    {
        pattern = "\\b" + pattern + "\\b";
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        keywordRules.append(rule);
    }

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
    commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat;
    keywordRules.append(rule);

    rule.pattern = QRegExp("'[^\n]*'");
    rule.format = stringFormat;
    keywordRules.append(rule);

    rule.pattern = QRegExp("yield\\(\\)");
    rule.format = keywordFormat;
    keywordRules.append(rule);

    rule.pattern = QRegExp("sleep\\([^\n]*\\)");
    rule.format = keywordFormat;
    keywordRules.append(rule);

    rule.pattern = QRegExp("\\@");
    rule.format = keywordFormat;
    keywordRules.append(rule);
}
//-----------------------------------------------------------------------------------------
void ScriptHighlighter::highlightBlock(const QString &text)
{
    foreach(HighlightingRule rule, keywordRules) 
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while(index >= 0) 
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
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

    foreach(HighlightingRule rule, valueRules) 
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while(index >= 0) 
        {
            int length = expression.matchedLength();
            setFormat(index+length-expression.cap(1).length(), expression.cap(1).length(), rule.format);
            index = text.indexOf(expression, index + length);
        }
    }
}
//-----------------------------------------------------------------------------------------
