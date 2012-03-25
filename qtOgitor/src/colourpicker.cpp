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

#include <QtGui/QtGui>
#include <QtGui/QPainter>
#include <QtGui/QColorDialog>
#include <QtCore/QEvent>
#include "colourpicker.hxx"

//----------------------------------------------------------------------------------
Ogre::ColourValue ColourPickerWidget::getColour()
{
   return Ogre::ColourValue(mColour.redF(), mColour.greenF(), mColour.blueF());
}
//----------------------------------------------------------------------------------
void ColourPickerWidget::paintEvent(QPaintEvent *evt)
{
   QPainter painter(this);
   painter.setClipRect(2,2,width() - 2,height() - 2);
   painter.setBrush(QBrush(mColour));
   painter.setPen(QColor(0,0,0));
   painter.fillRect(QRectF(0,0,width(),height()), mColour);
   painter.drawRect(2, 2, width() - 3, height() - 3);
}
//----------------------------------------------------------------------------------
void ColourPickerWidget::mousePressEvent(QMouseEvent * evt)
{
   if(evt->button() == Qt::LeftButton)
   {
      QColor result = QColorDialog::getColor(mColour, this);

      if(result.isValid())
      {
         mColour = result;
         update();
         Ogre::ColourValue value = Ogre::ColourValue(mColour.redF(), mColour.greenF(), mColour.blueF());
         emit colourChanged( value );
      }
   }
}
//----------------------------------------------------------------------------------