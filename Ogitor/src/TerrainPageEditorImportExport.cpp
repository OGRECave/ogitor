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

#include "OgitorsPrerequisites.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OgreTerrain.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainMaterialGenerator.h"
#include "Terrain/OgreTerrainGroup.h"
#endif

#include "OgreStreamSerialiser.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "tinyxml.h"
#include "OgitorsUndoManager.h"

using namespace Ogitors;


int getNumLayers(Ogre::PixelFormat pf)
{
    switch(pf)
    {
    case Ogre::PF_L8:
    case Ogre::PF_L16:
    case Ogre::PF_FLOAT16_R:
    case Ogre::PF_FLOAT32_R:
    case Ogre::PF_DEPTH:
    case Ogre::PF_A8:return 1;
    case Ogre::PF_R5G6B5:
    case Ogre::PF_B5G6R5:
    case Ogre::PF_X8R8G8B8:
    case Ogre::PF_X8B8G8R8:
    case Ogre::PF_R8G8B8:
    case Ogre::PF_B8G8R8:
    case Ogre::PF_FLOAT16_RGB:
    case Ogre::PF_FLOAT32_RGB:
    case Ogre::PF_SHORT_RGB:
    case Ogre::PF_PVRTC_RGB2:
    case Ogre::PF_PVRTC_RGB4:
    case Ogre::PF_R3G3B2:return 3;
    case Ogre::PF_A4R4G4B4:
    case Ogre::PF_A1R5G5B5:
    case Ogre::PF_A8R8G8B8:
    case Ogre::PF_A8B8G8R8:
    case Ogre::PF_B8G8R8A8:
    case Ogre::PF_R8G8B8A8:
    case Ogre::PF_A2R10G10B10:
    case Ogre::PF_A2B10G10R10:
    case Ogre::PF_DXT1:
    case Ogre::PF_DXT2:
    case Ogre::PF_DXT3:
    case Ogre::PF_DXT4:
    case Ogre::PF_DXT5:
    case Ogre::PF_FLOAT16_RGBA:
    case Ogre::PF_FLOAT32_RGBA:
    case Ogre::PF_SHORT_RGBA:
    case Ogre::PF_PVRTC_RGBA2:
    case Ogre::PF_PVRTC_RGBA4:return 4;
    }
    return 0;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::importHeightMap(Ogre::String filename, Ogre::Real fBias, Ogre::Real fScale)
{
    if(!mHandle || !mLoaded->get())
        return false;

    Ogre::StringUtil::trim(filename);

    if(filename.empty())
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Raw 32bit Float File"));
        extlist.push_back("*.raw;*.ohm;*.f32;*.r32");
        extlist.push_back(OTR("PNG Grayscale"));
        extlist.push_back("*.png");
        filename = mSystem->DisplayOpenDialog(OTR("Import Heightmap"),extlist);
        if(filename == "") 
            return false;
    }

    bool flipV = false;
    float *flipBV = 0;

    if(fBias == 0.0f && fScale == 0.0f)
    {
        Ogre::NameValuePairList params;
        params["check1"] = "true";
        if(!mSystem->DisplayImportHeightMapDialog(params))
            return false;

        fScale = Ogre::StringConverter::parseReal(params["input1"]);
        fBias = Ogre::StringConverter::parseReal(params["input2"]);
        flipV = Ogre::StringConverter::parseBool(params["inputCheckV"]);
    }

    float *mHeightData = mHandle->getHeightData();

    Ogre::String namePart = OgitorsUtils::ExtractFileName(filename);
    namePart.erase(0, namePart.find("."));

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Import Heightmap");

    Ogre::Rect rect(0,0,mHandle->getSize(), mHandle->getSize());
    _notifyModification(-1, rect);
    _notifyEndModification();


    if(namePart == ".ohm" || namePart == ".raw" || namePart == ".f32" || namePart == ".r32")
    {
        size_t vertexNum = mHandle->getSize() * mHandle->getSize();
        float *data = OGRE_ALLOC_T(float, vertexNum, Ogre::MEMCATEGORY_GEOMETRY);

        try
        {
            FILE *f = fopen(filename.c_str(),"rb");
            size_t byte_read = fread(data,sizeof(float),vertexNum,f);
            fclose(f);
        }
        catch(...)
        {
            OGRE_FREE(data, Ogre::MEMCATEGORY_GEOMETRY);
            return false;
        }

        if(flipV)
        {
            flipBV = OGRE_ALLOC_T(float, mHandle->getSize(), Ogre::MEMCATEGORY_GEOMETRY);

            int linelength = mHandle->getSize() * sizeof(float);

            for(int fj = 0;fj < mHandle->getSize();fj++)
            {
                memcpy(flipBV, data + (fj * linelength), linelength);
                memcpy(data + (fj * linelength), data + ((mHandle->getSize() - fj - 1) * linelength), linelength);
                memcpy(data + ((mHandle->getSize() - fj - 1) * linelength), flipBV, linelength);
            }

            OGRE_FREE(flipBV, Ogre::MEMCATEGORY_GEOMETRY);
        }
        
        for(unsigned int px = 0;px < vertexNum;px++)
            mHeightData[px] = fBias + (data[px] * fScale);

        OGRE_FREE(data, Ogre::MEMCATEGORY_GEOMETRY);
    }
    else if(namePart == ".png")
    {
        std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
        Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

        Ogre::Image img;
        img.load(stream);
        img.resize(mHandle->getSize(), mHandle->getSize());
        
        if(flipV)
            img.flipAroundY();

        Ogre::ColourValue cval;

        for(int y = 0;y < mHandle->getSize();y++)
        {
            for(int x = 0;x < mHandle->getSize();x++)
            {
                cval = img.getColourAt(x, y, 0);
                *mHeightData = fBias + (cval.r * fScale);
                ++mHeightData;
            }
        }
        
        stream.setNull();
    }
    else
        return false;
    
    Ogre::Rect drect(0, 0, mHandle->getSize(), mHandle->getSize());
    mHandle->dirtyRect(drect);
    mHandle->update();

    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::importBlendMap(int layerID, Ogre::String filename)
{
    if(!mHandle || !mLoaded->get())
        return false;

    if(layerID >= mLayerCount->get())
        return false;

    Ogre::StringUtil::trim(filename);

    if(filename.empty())
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Image Files"));
        extlist.push_back("*.png;*.jpg;*.tga");
        filename = mSystem->DisplayOpenDialog(OTR("Import Blendmap"),extlist);
        if(filename == "") 
            return false;
    }

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Import Blendmap");

    Ogre::Rect rect(0,0,mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
    _notifyModification(layerID, rect);
    _notifyEndModification();

    Ogre::TerrainLayerBlendMap *mBlendMap;
    float *mBlendData;

    mBlendMap = mHandle->getLayerBlendMap(layerID);
    mBlendData = mBlendMap->getBlendPointer();

    std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

    Ogre::Image img;
    img.load(stream);
    img.resize(mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
    
    Ogre::ColourValue cval;

    for(int y = 0;y < mHandle->getLayerBlendMapSize();y++)
    {
        for(int x = 0;x < mHandle->getLayerBlendMapSize();x++)
        {
            cval = img.getColourAt(x, y, 0);
            *mBlendData = cval.r;
            ++mBlendData;
        }
    }

    Ogre::Rect drect(0, 0, mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
    mBlendMap->dirtyRect(drect);
    mBlendMap->update();

    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
 
    stream.setNull();
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::importBlendMap(Ogre::String filename)
{
    if(!mHandle || !mLoaded->get())
        return false;

    if(mLayerCount->get() < 2)
        return false;

    Ogre::StringUtil::trim(filename);

    if(filename.empty())
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Image Files"));
        extlist.push_back("*.png;*.jpg;*.tga");
        filename = mSystem->DisplayOpenDialog(OTR("Import Blendmap"),extlist);
        if(filename == "") 
            return false;
    }

    std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

    Ogre::Image img;
    img.load(stream);
    img.resize(mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
    
    Ogre::ColourValue cval;

    int maxlayer = getNumLayers(img.getFormat()) + 1;

    maxlayer = std::min(maxlayer, mLayerCount->get()); 

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Import Blendmap");

    Ogre::Rect rect(0,0,mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
    _notifyModification(1, rect);
    _notifyEndModification();

    for(int i = 1;i < maxlayer;i++)
    {
        Ogre::TerrainLayerBlendMap *mBlendMap;
        float *mBlendData;

        mBlendMap = mHandle->getLayerBlendMap(i);
        mBlendData = mBlendMap->getBlendPointer();

        for(int y = 0;y < mHandle->getLayerBlendMapSize();y++)
        {
            for(int x = 0;x < mHandle->getLayerBlendMapSize();x++)
            {
                cval = img.getColourAt(x, y, 0);
                switch(i)
                {
                case 1:*mBlendData = cval.r;break;
                case 2:*mBlendData = cval.g;break;
                case 3:*mBlendData = cval.b;break;
                case 4:*mBlendData = cval.a;break;
                }
               
                ++mBlendData;
            }
        }

        Ogre::Rect drect(0, 0, mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
        mBlendMap->dirtyRect(drect);
        mBlendMap->update();
    }

    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);

    stream.setNull();
    
    return true;
}
//-----------------------------------------------------------------------------------------
/* EXAMPLE Values HS - HE  SS - SE  SKW / SKWAZM 
grass  30-150   0-20   0 / 90 
soil   50-160  20-60  -1 / 90
rock  150-200   0-60  -1 / 90 
steep 150-400  30-90   0 / 90
snow  200-400   0-60  -3 / 90
*/
inline float _calculateBlendFactor(float h, Ogre::Vector3 &normal, CalcBlendData& data )
{
    //slope of current point (the y value of the normal)
    float slope=normal.y;

    //the skew denominator
    float skewDenom = normal.x * normal.x + normal.z * normal.z;

    //are we to do skewing... 
        //if it is a flat terrain there is no need
    bool doSkew;

    //if skew denominator is "almost" 0 then terrain is flat
    if (skewDenom > 0.00001) 
    {
        //turn the skewing on and calculate the denominator
        doSkew = true;
        skewDenom = 1.0f / sqrt(skewDenom);
    }
    else 
        doSkew=false;

    //factor for this material (for clarity)
    float factor=1.0; 

    //first check - elevation
    float elv_max = data.he;
    float elv_min = data.hs;

    //are we to do skewing ?
    if (doSkew) 
    {
        //calculate 2D skew vector
        float skx = cos(data.skwazm * Ogre::Math::PI / 180.0f);
        float sky = sin(data.skwazm * Ogre::Math::PI / 180.0f);

        //skew scale value
        float scale = ((normal.x * skx) + (normal.y * sky)) * skewDenom;
            
        //adjust elevation limits
        elv_max += data.skw * scale;
        elv_min += data.skw * scale;
    }
    
    //current elevation release
    float rel = data.hr; //RELEASE AMOUNT

    //if outside limit elevation AND release skip this one
    if ((elv_max + rel) < h) 
        return 0.0f;
    if ((elv_min - rel) > h) 
        return 0.0f;
        
    if (elv_max < h) 
    {
        // we are in release compute the factor
        factor = 1.0f - (h - elv_max) / rel;
    }
        
    if (elv_min > h) 
    {
        // we are in release compute the factor
        factor = 1.0f - (elv_min - h) / rel;
    }

    //now check the slopes...

    //slope release
    float srel = cos((Ogre::Math::PI / 2.0f) - (data.sr * Ogre::Math::PI / 180.f));

    //calculate min and max slope
    float minslope = cos(data.ss * Ogre::Math::PI / 180.0f);
    float maxslope = cos(data.se * Ogre::Math::PI / 180.0f);

    //reverse?
    if (minslope > maxslope) 
    {
        float t = maxslope;
        maxslope = minslope;
        minslope = t;
    }

    //this slope is not supported for this type
    if (slope < (minslope - srel)) 
        return 0.0f;
    if (slope > (maxslope + srel)) 
        return 0.0f;

    //release?
    if (slope > maxslope) 
        factor *= 1.0f - ( slope - maxslope) / srel;
    if (slope < minslope) 
        factor *= 1.0f - (minslope - slope) / srel;

    return factor;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::calculateBlendMap()
{
    if(!mHandle || !mHandle->isLoaded())
        return false;

    Ogre::NameValuePairList params;
    
    if(mSystem->DisplayCalculateBlendMapDialog(params))
    {
        Ogre::Rect rect(0,0,mHandle->getLayerBlendMapSize(), mHandle->getLayerBlendMapSize());
        
        OgitorsUndoManager::getSingletonPtr()->BeginCollection("Calculate Blendmap");
 
        _notifyModification(1, rect); 
        _notifyEndModification();
        
        std::vector<CalcBlendData> layerdata;

        int i;
        for(i = 1;i < 6;i++)
        {
            Ogre::String ids = Ogre::StringConverter::toString(i) + "::";
            if(!(params[ids + "img"].empty()))
            {
                Ogre::String mTextureDiffuse = params[ids + "img"];
                Ogre::String mTextureNormal = "";
    
                int pos = mTextureDiffuse.find(";");
                if(pos != -1)
                {
                    mTextureNormal = mTextureDiffuse.substr(pos + 1,mTextureDiffuse.length() - pos + 1);
                    mTextureDiffuse = mTextureDiffuse.erase(pos,mTextureDiffuse.length() - pos);
                }

                if(i < mLayerCount->get())
                    _changeLayer(i, mTextureDiffuse, mTextureNormal, 20.0f);
                else
                    _createLayer(i, mTextureDiffuse, mTextureNormal, 20.0f);

                CalcBlendData data;

                data.hs = Ogre::StringConverter::parseReal(params[ids + "hs"]);
                data.he = Ogre::StringConverter::parseReal(params[ids + "he"]);
                data.hr = Ogre::StringConverter::parseReal(params[ids + "hr"]);
                data.ss = Ogre::StringConverter::parseReal(params[ids + "ss"]);
                data.se = Ogre::StringConverter::parseReal(params[ids + "se"]);
                data.sr = Ogre::StringConverter::parseReal(params[ids + "sr"]);
                data.skw = Ogre::StringConverter::parseReal(params[ids + "skw"]);
                data.skwazm = Ogre::StringConverter::parseReal(params[ids + "skwazm"]);

                if(data.hs > data.he)
                    std::swap(data.hs, data.he);
                
                if(data.ss > data.se)
                    std::swap(data.ss, data.se);

                layerdata.push_back(data);

            }
            else
                break;
        }

        while(i < mLayerCount->get())
            _deleteLayer(i);

        if(layerdata.size() > 0)
        {
            _notifyModification(1, rect); 
            _notifyEndModification();

            int blendSize = mHandle->getLayerBlendMapSize();
            Ogre::Real steppingWorld = (Ogre::Real)mHandle->getWorldSize() / ((Ogre::Real)blendSize * 2.0f);
            Ogre::Real stepping = 1.0f / (Ogre::Real)blendSize;
            Ogre::Real halfStepping = stepping / 2.0f;
            Ogre::Real quarterStepping = stepping / 4.0f;

            float influence_sum[6];
            influence_sum[0] = 0.0f;
            
            Ogre::TerrainLayerBlendMap *mBlendMaps[5];
            float *mBlendDatas[5];
            
            for(unsigned int l = 0;l < layerdata.size();l++)
            {
                mBlendMaps[l] = mHandle->getLayerBlendMap(l + 1);
                mBlendDatas[l] = mBlendMaps[l]->getBlendPointer();
            }

            for(int y = 0;y < blendSize;y++)
            {
                int blendPositionY = (blendSize - y - 1) * blendSize;
                for(int x = 0;x < blendSize;x++)
                {
                    Ogre::Real centerH = mHandle->getHeightAtTerrainPosition(((Ogre::Real)x * stepping) + halfStepping, ((Ogre::Real)y * stepping) + halfStepping);
                    Ogre::Real topH = mHandle->getHeightAtTerrainPosition(((Ogre::Real)x * stepping) + halfStepping, ((Ogre::Real)y * stepping) + halfStepping - quarterStepping);
                    Ogre::Real bottomH = mHandle->getHeightAtTerrainPosition(((Ogre::Real)x * stepping) + halfStepping, ((Ogre::Real)y * stepping) + halfStepping + quarterStepping);
                    Ogre::Real leftH = mHandle->getHeightAtTerrainPosition(((Ogre::Real)x * stepping) + halfStepping - quarterStepping, ((Ogre::Real)y * stepping) + halfStepping);
                    Ogre::Real rightH = mHandle->getHeightAtTerrainPosition(((Ogre::Real)x * stepping) + halfStepping + quarterStepping, ((Ogre::Real)y * stepping) + halfStepping);

                    Ogre::Vector3 dv1(0,topH - bottomH, steppingWorld);
                    Ogre::Vector3 dv2(steppingWorld, leftH - rightH, 0);
                    dv1 = dv1.crossProduct(dv2).normalisedCopy();

                    for(unsigned int current_layer = 0;current_layer < layerdata.size();current_layer++)
                    {
                        float infl = _calculateBlendFactor(centerH, dv1, layerdata[current_layer]);
                        influence_sum[current_layer + 1] = influence_sum[current_layer] + infl;
                        infl = infl / std::max(influence_sum[current_layer + 1], 0.001f);
                        influence_sum[current_layer + 1] = influence_sum[current_layer] + infl;
                        mBlendDatas[current_layer][blendPositionY + x] = infl;
                    }
                }
            }
            
            Ogre::Rect maprect(0,0, blendSize, blendSize);

            for(unsigned int l = 0;l < layerdata.size();l++)
            {
                mBlendMaps[l]->dirtyRect(maprect);
                mBlendMaps[l]->update();
            }
        }

        OgitorsUndoManager::getSingletonPtr()->EndCollection(true);

        return true;
    }
    
    return false;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::exportHeightMap(Ogre::String path, Ogre::String filename, Ogre::Real fMin, Ogre::Real fMax)
{
    if(path.empty())
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Raw 32bit Float File"));
        extlist.push_back("*.raw;*.ohm;*.f32;*.r32");
        extlist.push_back(OTR("PNG Grayscale"));
        extlist.push_back("*.png");
        filename = mSystem->DisplaySaveDialog(OTR("Export Heightmap"),extlist);
        if(filename == "") 
            return false;
    }
    else
    {
        if(filename.empty())
            filename = "PageX" + Ogre::StringConverter::toString(mPageX->get()) + "Y" + Ogre::StringConverter::toString(mPageY->get()) + ".f32";

        filename = OgitorsUtils::QualifyPath(path + "/" + filename);
    }

    bool unload = !mLoaded->get();

    load(false);

    float *data = mHandle->getHeightData();
    int numvertexes = mHandle->getSize() * mHandle->getSize();

    Ogre::String namePart = OgitorsUtils::ExtractFileName(filename);
    namePart.erase(0, namePart.find("."));

    if(namePart == ".ohm" || namePart == ".raw" || namePart == ".f32" || namePart == ".r32")
    {
        std::ofstream stream(filename.c_str(), std::ios::binary);
        stream.write((char*)data, sizeof(float) * numvertexes);
        stream.close();
    }
    else if(namePart == ".png")
    {
        Ogre::Image img;
        unsigned short *idata = OGRE_ALLOC_T(unsigned short, numvertexes, Ogre::MEMCATEGORY_RESOURCE);

        if(fMin == 0.0f && fMax == 0.0f)
        {
            Ogre::NameValuePairList params;
            params["title"] = "Heightmap Export Parameters";
            params["input1"] = "Min Height :";
            params["input2"] = "Max Height :";
            params["input1value"] = "0";
            params["input2value"] = "0";
            if(!mSystem->DisplayImportHeightMapDialog(params))
                return false;
            
            fMin = Ogre::StringConverter::parseReal(params["input1"]);
            fMax = Ogre::StringConverter::parseReal(params["input2"]);

            if(fMin == 0.0f && fMax == 0.0f)
                return false;
        }


        float scale = fMax - fMin;
        if(scale == 0.0f)
            scale = 1.0f;

        scale = 65535.0f / scale;

        for(int px = 0;px < numvertexes;px++)
        {
            float val = std::max(data[px], fMin);
            val = std::min(val, fMax);
            idata[px] = (unsigned short)((val - fMin) * scale);
        }

        img.loadDynamicImage((Ogre::uchar*)idata, mHandle->getSize(), mHandle->getSize(), Ogre::PF_L16);
        img.save(filename);

        OGRE_FREE(idata, Ogre::MEMCATEGORY_RESOURCE);
    }
    else
    {
        mSystem->DisplayMessageDialog(OTR("This File Format not Supported"), DLGTYPE_OK);
        
        if(unload)
            unLoad();

        return false;
    }

    if(unload)
        unLoad();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::exportCompositeMap(Ogre::String path, Ogre::String filename)
{
    if(path.empty())
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("PNG"));
        extlist.push_back("*.png");
        filename = mSystem->DisplaySaveDialog(OTR("Export Compositemap"),extlist);
        if(filename == "") 
            return false;
    }
    else
    {
        if(filename.empty())
            filename = "PageX" + Ogre::StringConverter::toString(mPageX->get()) + "Y" + Ogre::StringConverter::toString(mPageY->get()) + "_composite.png";

        filename = OgitorsUtils::QualifyPath(path + "/" + filename);
    }

    bool unload = !mLoaded->get();

    load(false);

    float *data = mHandle->getHeightData();
    int numvertexes = mHandle->getSize() * mHandle->getSize();

    Ogre::String namePart = OgitorsUtils::ExtractFileName(filename);
    namePart.erase(0, namePart.find("."));

    if(namePart == ".png")
    {
        Ogre::Image img;

        mHandle->getCompositeMap()->convertToImage(img);

        img.save(filename);
    }
    else
    {
        mSystem->DisplayMessageDialog(OTR("This File Format not Supported"), DLGTYPE_OK);
        
        if(unload)
            unLoad();

        return false;
    }

    if(unload)
        unLoad();

    return true;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
