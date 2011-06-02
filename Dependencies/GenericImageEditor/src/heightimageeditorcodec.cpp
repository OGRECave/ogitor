/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "heightimageeditorcodec.hxx"

float mapToRange(float val, float minInput, float maxInput, float minOutput, float maxOutput)
{
    if (val < minInput || val > maxInput)
        return 0.0f;

    float inputRange = maxInput - minInput;
    float outputRange = maxOutput - minOutput;

    //Map input to 0..1 range
    float temp = 1.0f - ((val - minInput) / inputRange);

    //Map 0..1 range to output
    return temp * outputRange + minOutput;
}
//-----------------------------------------------------------------------------------------
void HSVtoARGB( unsigned char *color, float h, float s, float v )
{
	int i;
	float f, p, q, t;
    color[3] = 255;

    if( s == 0 ) {
		// achromatic (grey)
        color[0] = color[1] = color[2] = (v * 255.0f);
		return;
	}
	
    h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:		
			color[2] = v * 255.0f;
			color[1] = p * 255.0f;
			color[0] = q * 255.0f;
			break;
		case 1:
			color[2] = v * 255.0f;
		    color[1] = t * 255.0f;
			color[0] = p * 255.0f;
			break;
		case 2:
			color[2] = q * 255.0f;
		    color[1] = v * 255.0f;
			color[0] = p * 255.0f;
			break;
		case 3:
			color[2] = p * 255.0f;
			color[1] = v * 255.0f;
			color[0] = t * 255.0f;
			break;
		case 4:
			color[2] = p * 255.0f;
			color[1] = q * 255.0f;
			color[0] = v * 255.0f;
			break;
		default:
			color[2] = t * 255.0f;
			color[1] = p * 255.0f;
			color[0] = v * 255.0f;
			break;
	}
}
//-----------------------------------------------------------------------------------------
HeightImageEditorCodec::HeightImageEditorCodec(QScrollArea* scrollArea, QString docName, QString documentIcon) : 
IImageEditorCodec(scrollArea, docName, documentIcon)
{
}
//-----------------------------------------------------------------------------------------
QPixmap* HeightImageEditorCodec::onBeforeDisplay(Ogre::DataStreamPtr stream)
{
    double file_len = stream->size() / 4;

    unsigned int map_size = sqrt(file_len);

    float *buf = new float [map_size * map_size];

    stream->read(buf, map_size * map_size * 4);
    
    float max_h = -1000000.0f;
    float min_h = 1000000.0f;

    for(unsigned int i = 0;i < map_size * map_size;i++)
    {
        if(buf[i] > max_h)
            max_h = buf[i];
        if(buf[i] < min_h)
            min_h = buf[i];
    }

    float diff = max_h - min_h;

    if(diff > 0.0f)
    {
        unsigned char *colbuf = (unsigned char *)buf;
        int pos = 0;
        
        for(unsigned int i = 0;i < map_size * map_size;i++)
        {
            float tval = mapToRange(buf[i], min_h, max_h, 60, 360);

            HSVtoARGB(&colbuf[pos], tval, 1.0f, 1.0f);

            pos += 4;
        }
    }
    else
        memset(buf, 0xFF, map_size * map_size * 4);

    QImage pImg((unsigned char *)buf, map_size, map_size, QImage::Format_ARGB32);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(pImg));

    delete [] buf;

    return pixmap;
}
//-----------------------------------------------------------------------------------------
IImageEditorCodec* HeightImageEditorCodecFactory::create(QScrollArea* scrollArea, QString docName)
{
    return new HeightImageEditorCodec(scrollArea, docName, ":/icons/paint.svg");
}
//-----------------------------------------------------------------------------------------