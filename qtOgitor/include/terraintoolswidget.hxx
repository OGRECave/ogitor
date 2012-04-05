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
//This program is free software; you can redistribute it and/or modify it under
//the terms of the GNU Lesser General Public License as published by the Free Software
//Foundation; either version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT
//ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License along with
//this program; if not, write to the Free Software Foundation, Inc., 59 Temple
//Place - Suite 330, Boston, MA 02111-1307, USA, or go to
//http://www.gnu.org/copyleft/lesser.txt.
////////////////////////////////////////////////////////////////////////////////*/

#ifndef TERRAINTOOLSWIDGET_HXX
#define TERRAINTOOLSWIDGET_HXX

#include <QtGui/QtGui>
#include "OgitorsPrerequisites.h"
#include "TerrainEditor.h"

class ColourPickerWidget;


class TerrainToolsWidget : public QWidget {
    Q_OBJECT;
public:
    explicit TerrainToolsWidget(QWidget *parent = 0);
    virtual ~TerrainToolsWidget();

    void updateTools();
    void updateTerrainOptions(Ogitors::ITerrainEditor *terrain);
    void switchToolWidget( const unsigned int tool );

public Q_SLOTS:
    void textureIndexChanged();
    void plantIndexChanged();
    void brushIndexChanged();
    void brushSizeValueChanged ( int value );
    void brushIntensityValueChanged ( int value );
    void paintColourChanged( Ogre::ColourValue value);

protected:
    QToolBar* toolBar;
    QToolBox* toolBox;
    QListWidget* brushWidget;
    QListWidget* texturesWidget;
    QListWidget* plantsWidget;
    QLabel*      mBrushSizeLabel;
    QSlider*     mBrushSizeSlider;
    QLabel*      mBrushIntensityLabel;
    QSlider*     mBrushIntensitySlider;
    ColourPickerWidget* mPaintColour;


    void resizeEvent(QResizeEvent* evt);

    void populateBrushes();
    void populateTextures();
    void populatePlants();

    void onSceneLoadStateChange(Ogitors::IEvent* evt);
};

#endif // TERRAINTOOLSWIDGET_HXX
