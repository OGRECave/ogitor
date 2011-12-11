/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2011 Andrew Fenn <andrewfenn@gmail.com> and the Ogitor Team
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

#include <QtGui/QtGui>

#include "uiterrainsquare.hxx"
#include "createterraindialog.hxx"

UITerrainSquare::UITerrainSquare(QWidget *parent, Ogre::NameValuePairList *params)
{
    this->parent = parent;
    this->params = params;
    setAcceptedMouseButtons(Qt::RightButton);
}

void UITerrainSquare::set(const signed int x, const signed int y, const QPen pen, const QBrush brush, const bool selectable)
{
    this->x = x;
    this->y = y;
    this->setPen(pen);
    this->setBrush(brush);
    this->selectable = selectable;
}

void UITerrainSquare::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!this->selectable)
        return;

    setCursor(Qt::ClosedHandCursor);
    this->setBrush(QBrush(QColor(173, 81, 44)));

    CreateTerrainDialog dlg(QApplication::activeWindow());

    if(dlg.exec() == QDialog::Accepted)
    {
        (*this->params)["init"] = "true";
        (*this->params)["pagex"] = Ogre::StringConverter::toString(this->x);
        (*this->params)["pagey"] = Ogre::StringConverter::toString(this->y);
        (*this->params)["diffuse"] = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString();
        (*this->params)["normal"] = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString();
        this->parent->close();
    } else {
        this->setBrush(QBrush(QColor(52, 51, 49)));
    }

    update();
    QGraphicsItem::mousePressEvent(event);
}

void UITerrainSquare::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!this->selectable)
        return;

    QGraphicsItem::mouseMoveEvent(event);
}

void UITerrainSquare::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!this->selectable)
        return;

    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}
