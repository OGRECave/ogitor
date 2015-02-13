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
#ifndef MATERIAL_HIGHLIGHTER_HXX
#define MATERIAL_HIGHLIGHTER_HXX

#include <QtCore/QStringListModel>

#include <QtWidgets/QWidget>

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextObjectInterface>

//-----------------------------------------------------------------------------------------

class MaterialHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MaterialHighlighter(QStringListModel* keywords, QStringListModel* enums, QStringListModel* dataTypes, 
                        QTextDocument *parent = 0);

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

protected:
    void highlightBlock(const QString &text);

private:
    void _highlightSimpleRegexList(const QString text, const QVector<HighlightingRule> rules);

    QVector<HighlightingRule> keywordRules;
    QVector<HighlightingRule> enumRules;
    QVector<HighlightingRule> dataTypeRules;
    QVector<HighlightingRule> formatRules;
    QVector<HighlightingRule> valueRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat textureFormat;
    QTextCharFormat enumFormat;
    QTextCharFormat dataTypeFormat;
    QTextCharFormat stringFormat;
};

//-----------------------------------------------------------------------------------------

#endif