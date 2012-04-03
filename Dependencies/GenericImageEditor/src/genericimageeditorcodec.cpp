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

#include "genericimageeditorcodec.hxx"

//-----------------------------------------------------------------------------------------
GenericImageEditorCodec::GenericImageEditorCodec(GenericImageEditorDocument* genImgEdDoc, QString docName, QString documentIcon) : 
IImageEditorCodec(genImgEdDoc, docName, documentIcon)
{
}
//-----------------------------------------------------------------------------------------
QPixmap GenericImageEditorCodec::onBeforeDisplay(Ogre::DataStreamPtr stream)
{
    Ogre::Image image;
    image.load(stream);

    int w = image.getWidth();
    int h = image.getHeight();

    mBuffer = new unsigned char[4 * w * h];
    Ogre::PixelBox pb(w, h, 1, Ogre::PF_A8R8G8B8, mBuffer);

    Ogre::PixelUtil::bulkPixelConversion(image.getPixelBox(), pb);

    QImage tmpImage = QImage(mBuffer, w, h, QImage::Format_ARGB32);
    mPixmap = QPixmap(QPixmap::fromImage(tmpImage));

    return mPixmap;
}
//-----------------------------------------------------------------------------------------
QString GenericImageEditorCodec::onToolTip(QMouseEvent* event)
{
    QPoint pos = event->pos() / mScaleFactor;
    // Read the pixel in question from the pixmap into a 1x1 pixel image
    QImage image = mPixmap.copy(pos.x(), pos.y(), 1, 1).toImage();
    QColor rgb = image.pixel(0, 0);
    return QString("X: %1 Y: %2\nR: %3 G: %4 B: %5").arg(pos.x()).arg(pos.y())
        .arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
}
//-----------------------------------------------------------------------------------------
IImageEditorCodec* GenericImageEditorCodecFactory::create(GenericImageEditorDocument* genImgEdDoc, QString docName)
{
    return new GenericImageEditorCodec(genImgEdDoc, docName, ":/icons/paint.svg");
}
//-----------------------------------------------------------------------------------------