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

#include "OgitorsPrerequisites.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OgreTerrain.h"
#include "OgreTerrainmaterialGeneratorA.h"
#include "OgreTerrainGroup.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainMaterialGeneratorA.h"
#include "Terrain/OgreTerrainGroup.h"
#endif

#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "OgreStreamSerialiser.h"
#include "SceneManagerEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::importFullTerrainFromHeightMap()
{
    UTFStringVector extlist;
    extlist.push_back(OTR("PNG Grayscale"));
    extlist.push_back("*.png");
    extlist.push_back(OTR("Raw 32bit Float File"));
    extlist.push_back("*.raw;*.ohm;*.f32;*.r32");

    Ogre::UTFString defaultPath = mSystem->GetSetting("system", "ExportTerrainPath", "");

    Ogre::String filename = mSystem->DisplayOpenDialog(OTR("Import Heightmap"), extlist, defaultPath);
    if(filename == "") 
        return;

    mSystem->SetSetting("system", "ExportTerrainPath", OgitorsUtils::ExtractFilePath(filename));

    Ogre::NameValuePairList params;
    if(!mSystem->DisplayImportHeightMapDialog(params))
        return;
    
    Ogre::Real fScale = Ogre::StringConverter::parseReal(params["scale"]);
    Ogre::Real fBias = Ogre::StringConverter::parseReal(params["bias"]);
    Ogre::String normal = params["normal"];
    Ogre::String diffuse = params["diffuse"];
    bool flipV = Ogre::StringConverter::parseBool(params["inverted"]);

    float *data = 0;
    float *flipBV = 0;

    Ogre::String namePart = OgitorsUtils::ExtractFileName(filename);
    namePart.erase(0, namePart.find("."));

    int imgW = 0;
    int imgH = 0;

    if(namePart == ".png")
    {
        std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
        Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

        Ogre::Image img;
        img.load(stream);

        data = OGRE_ALLOC_T(float, img.getWidth() * img.getHeight(), Ogre::MEMCATEGORY_GEOMETRY);
        Ogre::PixelBox pb(img.getWidth(), img.getHeight(), 1, Ogre::PF_FLOAT32_R, data);
        Ogre::PixelUtil::bulkPixelConversion(img.getPixelBox(), pb);

        imgW = img.getWidth();
        imgH = img.getHeight();
        
        img.freeMemory();
        stream.setNull();
    }
    else if(namePart == ".ohm" || namePart == ".raw" || namePart == ".f32" || namePart == ".r32")
    {
        FILE *f = fopen(filename.c_str(),"rb");
        fseek(f, 0, SEEK_END);
        int vertexNum = ftell(f) / 4;
        fseek(f, 0, SEEK_SET);
        data = OGRE_ALLOC_T(float, vertexNum, Ogre::MEMCATEGORY_GEOMETRY);
        size_t byte_read = fread(data,sizeof(float),vertexNum,f);
        fclose(f);
        imgW = imgH = sqrt((float)vertexNum);
    }

    int msize = mMapSize->get() - 1;
    int XCount = (imgW - 1) / msize;
    int YCount = (imgH - 1) / msize;

    float *dataptr = OGRE_ALLOC_T(float, mMapSize->get() * mMapSize->get(), Ogre::MEMCATEGORY_GEOMETRY);

    for(int y = 0;y < YCount;y++)
    {
        for(int x = 0;x < XCount;x++)
        {
            OgitorsPropertyValueMap creationparams;
            OgitorsPropertyValue pvalue;

            Ogre::String pagename = mPageNamePrefix->get();
            pagename += Ogre::StringConverter::toString(x - (XCount / 2));
            pagename += "x";
            pagename += Ogre::StringConverter::toString(y - (YCount / 2));

            creationparams["init"] = EMPTY_PROPERTY_VALUE;
            pvalue.propType = PROP_STRING;
            pvalue.val = Ogre::Any(pagename);
            creationparams["name"] = pvalue;
            
            Ogre::Vector3 position;
            mHandle->convertTerrainSlotToWorldPosition(x - (XCount / 2), y - (YCount / 2), &position);

            pvalue.propType = PROP_VECTOR3;
            pvalue.val = Ogre::Any(position);
            creationparams["position"] = pvalue;
            pvalue.propType = PROP_INT;
            pvalue.val = Ogre::Any(x - (XCount / 2));
            creationparams["pagex"] = pvalue;
            pvalue.propType = PROP_INT;
            pvalue.val = Ogre::Any(y - (YCount / 2));
            creationparams["pagey"] = pvalue;
            pvalue.propType = PROP_STRING;
            pvalue.val = diffuse;
            creationparams["layer0::diffusespecular"] = pvalue;
            pvalue.propType = PROP_STRING;
            pvalue.val = normal;
            creationparams["layer0::normalheight"] = pvalue;
            pvalue.propType = PROP_REAL;
            pvalue.val = Ogre::Any((Ogre::Real)100.0f);
            creationparams["layer0::worldsize"] = pvalue;

            float cval;

            for(int iy = 0;iy <= msize;iy++)
            {
                for(int ix = 0;ix <= msize;ix++)
                {
                    cval = data[(((y * msize) + iy) * imgW) + (x * msize) + ix];
                    if (flipV)
                        cval *= -1;

                    dataptr[(iy * (msize + 1)) + ix] = fBias + (cval * fScale);
                }
            }

            pvalue.propType = PROP_UNSIGNED_LONG;
            pvalue.val = Ogre::Any((unsigned long)dataptr);
            creationparams["externaldatahandle"] = pvalue;

            CTerrainPageEditor* page = (CTerrainPageEditor*)mOgitorsRoot->CreateEditorObject(this, "Terrain Page", creationparams, true, true);
        }
    }

    OGRE_FREE(data, Ogre::MEMCATEGORY_GEOMETRY);
    OGRE_FREE(dataptr, Ogre::MEMCATEGORY_GEOMETRY);

    Ogre::String msg = Ogre::StringConverter::toString(XCount) + " Horizontal, " + Ogre::StringConverter::toString(YCount) + " Vertical pages created. Lightmaps are not calculated to save time.\nPlease use Re-Light to calculate them."; 
    mSystem->DisplayMessageDialog(msg, DLGTYPE_OK);
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::exportHeightMaps()
{
    Ogre::UTFString defaultPath = mSystem->GetSetting("system", "ExportTerrainPath", "");
    Ogre::String directory = mSystem->DisplayDirectorySelector(OTR("Select a Directory to Export"), defaultPath);

    if(directory.empty())
        return;

    mSystem->SetSetting("system", "ExportTerrainPath", OgitorsUtils::ExtractFilePath(directory));

    NameObjectPairList::iterator it = mChildren.begin();

    while(it != mChildren.end())
    {
        CTerrainPageEditor *ed = static_cast<CTerrainPageEditor*>(it->second);
        ed->exportHeightMap(directory);
        it++;
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::exportCompositeMaps()
{
    Ogre::UTFString defaultPath = mSystem->GetSetting("system", "ExportTerrainPath", "");
    Ogre::String directory = mSystem->DisplayDirectorySelector(OTR("Select a Directory to Export"), defaultPath);

    if(directory.empty())
        return;

    mSystem->SetSetting("system", "ExportTerrainPath", OgitorsUtils::ExtractFilePath(directory));

    NameObjectPairList::iterator it = mChildren.begin();

    while(it != mChildren.end())
    {
        CTerrainPageEditor *ed = static_cast<CTerrainPageEditor*>(it->second);
        ed->exportCompositeMap(directory);
        it++;
    }
}
//-----------------------------------------------------------------------------------------
