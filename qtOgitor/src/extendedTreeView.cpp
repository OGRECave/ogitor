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

#include "extendedTreeView.hxx"
#include <QtGui/QPainter>

//-----------------------------------------------------------------------------------------
ExtendedItemDelegate::ExtendedItemDelegate(QWidget* parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------------------
void ExtendedItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();    

    QStyleOptionViewItemV4 newOption(option);
    QString iconPath = index.data(Qt::UserRole).toString();
    if(!iconPath.isEmpty())
    {
        QIcon icon(iconPath);
        painter->drawPixmap(option.rect.topLeft(), icon.pixmap(16, 16));
        newOption.rect.setX(option.rect.x() + 15);
    }
    
    QStyledItemDelegate::paint(painter, newOption, index);

    painter->restore();
}
//-----------------------------------------------------------------------------------------
ExtendedTreeWidget::ExtendedTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setItemDelegate(new ExtendedItemDelegate(parent));
}
//-----------------------------------------------------------------------------------------