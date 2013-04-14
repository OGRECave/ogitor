Dialog@     MainDialog = null;            //Our main dialog handle
ComboBox@[] layerCombos(6);               //The Combo Boxe handles that behave as Texture Selector
CheckBox@[] layerCheckBoxs(6);            //Active flag checkbox handles
LineEdit@[] layerWorldSizes(6);           //Handle to each Layer's world size line edit
LineEdit@   inputBias = null;             //Input Bias LineEdit handle
LineEdit@   inputScale = null;            //Input Scale Line Edit handle
LineEdit@   filePath = null;              //File Path Line Edit handle 
LineEdit@   heightmapNameTemplate = null; //Heightmap Name Template Line Edit handle
LineEdit@   blendmapNameTemplate = null;  //Blendmap Name Template Line Edit handle
Button@     accept = null;                //Handle of Accept Button
Button@     cancel = null;                //Handle of Cancel Button  
Button@     browse = null;                //Handle of Browse(...) Button  
Button@     help = null;                  //Handle of Help Button
string      heightmapfile = '';           //Global variable to hold value of Heightmap Name Template Line Edit
string      blendmapfile = '';            //Global variable to hold value of Blendmap Name Template Line Edit
string      filespath = '';               //Global variable to hold value of File Path Line Edit
int         SectorStartX = 1000;          //Global variable to hold value of X Axis Sector Start
int         SectorStartY = 1000;          //Global variable to hold value of Y Axis Sector Start
int         SectorEndX = -1000;           //Global variable to hold value of X Axis Sector End    
int         SectorEndY = -1000;           //Global variable to hold value of Y Axis Sector End
StringVector diffuseTexNames;             //Global variable to hold a list of Terrain Diffuse Map Texture Names
StringVector normalTexNames;              //Global variable to hold a list of Terrain Normal Map Texture Names
             
// Searches for files that match given heightmap file name template
// Also calculates MinimumX, MinimumY, MaximumX and MaximumY   
// that the file names include, we use those values to define the array   
// of Terrain Pages to look for, later we iterate this array and load 
// the heightmap files that exist. We do not care blendmap files here
// since import procedure only uploads blendmaps that exist and skip the rest
bool calculateSectorList()
{
    StringVector results;
    string search = filespath + '/' + heightmapfile;
    search.replaceString('[X]', '*');
    search.replaceString('[Y]', '*');

    system.getFileList(search, results);

    int posX = heightmapfile.find('[X]') + filespath.length() + 1;
    int posY = heightmapfile.find('[Y]') + filespath.length() + 1;
    int maxX = -1000;
    int minX = 1000;
    int maxY = -1000;
    int minY = 1000;
    int count = results.size();

    for(int x = 0;x < count;x++)
    {
        string tmp = results[x].subString(posX, 4);
        int tmpi = tmp.toInt();
        if(tmpi > maxX)
            maxX = tmpi;
        if(tmpi < minX)
            minX = tmpi;

        tmp = results[x].subString(posY + string(tmpi).length() - 3, 4);
        tmpi = tmp.toInt();
        if(tmpi > maxY)
            maxY = tmpi;
        if(tmpi < minY)
            minY = tmpi;
    }

    if((maxX - minX) >= 0 && (maxY - minY) >= 0)
    {
         SectorStartX = minX;
         SectorEndX = maxX;
         SectorStartY = minY;
         SectorEndY = maxY;
         return true;
    }
    else
        return false;
}
 
// Just a function to automate creation of:
// Layer Name, Active Checkbox, Texture DropDown and WorldSize LineEdit
ComboBox@ createLayerCombo()
{
    ComboBox@ combo = gui.createComboBox();
    combo.setFixedSize(74, 64);
    combo.setIconSize(60, 60);

    string itemname;

    int count = diffuseTexNames.size();
    for(int i = 0;i < count;i++)
    {
        itemname = diffuseTexNames[i];
        itemname.erase(itemname.length() - 12, 12);
        itemname = utils.qualifyPath('../Media/TerrainTextures/' + itemname + '.png');
        combo.addItem(Icon(itemname), '');
    }
    
    return combo;
}
 
// Actual Import is handled in this function
void startImport()
{
    int layerCount = 1;
    while(layerCheckBoxs[layerCount].isChecked())
        layerCount++;

    string ImportErrors = '';

    float i_bias = string(inputBias.getText()).toDouble();
    float i_scale = string(inputScale.getText()).toDouble();

    EditorVector pages;
    root.getObjectList('Terrain Page Object', pages);

    for(int p = 0;p < pages.size();p++)
        root.destroyObject(pages[p], true, true);

    for(int YP = SectorStartY;YP <= SectorEndY;YP++)
    {
        for(int XP = SectorStartX;XP <= SectorEndX;XP++)
        {
             string search = filespath + '/' + heightmapfile;
             search.replaceString('[X]', string(XP));
             search.replaceString('[Y]', string(YP));
             if(system.fileExists(search))
             {
                 int index = layerCombos[0].getCurrentIndex();
                 float worldSize = string(layerWorldSizes[0].getText()).toDouble();
                 TerrainPageEditor@ pageED = root.createTerrainPage(XP, YP, diffuseTexNames[index], normalTexNames[index], worldSize);
                 pageED.importHeightMap(search, i_bias, i_scale);
                 for(int L = 1;L < layerCount;L++)
                 {
                     worldSize = string(layerWorldSizes[L].getText()).toDouble();
                     int bindex = layerCombos[L].getCurrentIndex();
                     
                     pageED.addLayer(diffuseTexNames[bindex], normalTexNames[bindex], worldSize, false);
                     
                     string blendmp = filespath + '/' + blendmapfile;
                     blendmp.replaceString('[X]', string(XP));
                     blendmp.replaceString('[Y]', string(YP));
                     blendmp.replaceString('[L]', string(L));
                     
                     if(system.fileExists(blendmp))
                     {
                         pageED.importBlendMap(L, blendmp);   
                     }
                     else
                     {
                         ImportErrors += blendmp;
                         ImportErrors += ' not found!!\n';
                     }
                 } 
             }
        }
    }

    if(ImportErrors.length() > 0)
        ImportErrors = 'Import Completed with following errors:\n\n' + ImportErrors;
    
    system.displayMessageDialog(UTFString(ImportErrors), DLGTYPE_OK);
}
 
// Handler for Browse Button Clicks, displays a directory selector
// to select a directory to look for heightmap and blendmap files
void onBrowseClick()
{
    string path = system.displayDirectorySelector(UTFString('Select Directory Containing Files'));
    if(path != '')
        filePath.setText(GUIString(path));
}
 
// Handler for Accept Button Clicks, checks if the given values are valid
// Also checks which height files exist and starts import process if user approves
void onAcceptClick()
{
    bool verified = true;
    string errors = '';
    heightmapfile = heightmapNameTemplate.getText();
    blendmapfile = blendmapNameTemplate.getText();
    filespath = filePath.getText();

    if(filespath == "")
    {
        errors = 'File path must not be empty!!\n';
        verified = false;
    }

    if((heightmapfile.find('[X]') == -1) || (heightmapfile.find('[Y]') == -1))
    {
        errors = 'Heightmap File Template must include [X] and [Y]\n';
        verified = false;
    }
    
    if((blendmapfile.find('[X]') == -1) || (blendmapfile.find('[Y]') == -1) || (blendmapfile.find('[L]') == -1))
    {
        errors += 'Blendmap File Template must include [X], [Y] and [L]';
        verified = false;
    }

    if(verified)
    {
        if(calculateSectorList())
        {
             string msg = 'Terrain files found: (%1,%2) to (%3,%4)\nWARNING: This will delete and recreate all pages in scene!!\nDo you want to continue Import?';
             if(system.displayMessageDialog(UTFString(msg.arg(SectorStartX).arg(SectorStartY).arg(SectorEndX).arg(SectorEndY)), DLGTYPE_YESNO) == DLGRET_YES)
                 {
                     MainDialog.accept();
                     startImport();
                 }
        }
        else
        {
            system.displayMessageDialog(UTFString('No matching files found!!'), DLGTYPE_OK);
        }
    }
    else
        system.displayMessageDialog(UTFString(errors), DLGTYPE_OK);
}
 
// Handler for Cancel Button Clicks, just closes the Dialog
void onCancelClick()
{
    MainDialog.reject();
}
 
// Handler for Help Button Clicks, just displays a Help Message
void onHelpClick()
{
    string helpMessage = 'Instructions:\n\n\n1 - Choose the path to load files from.\n\n';
    helpMessage += '2 - Adjust heightmap name mask.\n    * Must include [X] and [Y] to identify position of heightmap in the grid.\n    * There must be a non-number character between [X] and [Y] to separate them.\n\n';    
    helpMessage += '3 - Adjust blendmap name mask.\n    * Again must include [X] and [Y] for grid position and [L] for layer ID.\n    * Layer IDs start from 1 since Layer 0 is the BASE Layer and do not use blendmap.\n\n';
    helpMessage += '4 - Adjust inputBias and inputScale.\n    * Bias is the Minimum Height to be added to input from files.\n    * Scale is the multiplier for input values.\n    * So FINAL_HEIGHT = BIAS + ((PIXEL_VALUE / 255) * SCALE).\n    * For RAW Float32 files, FINAL_HEIGHT = BIAS + (RAW_FLOAT_VALUE * SCALE)\n\n';
    helpMessage += '5 - Activate the layers to be used.\n    * Choose Layer`s texture from combo drop down.\n    * Enter a world size for the texture. It means the texture will be repeated every <worldsize> units.\n\n';
    helpMessage += '6 - Hit OK to start import process.';
    system.displayMessageDialog(UTFString(helpMessage), DLGTYPE_OK);
}
 
void main()
{
    if(!root.isSceneLoaded())
    {
         system.displayMessageDialog(UTFString('This script requires a loaded scene!!!'), DLGTYPE_OK);
         return;
    }

    if(root.getTerrainEditor() is null)
    {
         system.displayMessageDialog(UTFString('This script requires a Terrain Group in scene!!!'), DLGTYPE_OK);
         return;
    }

    @MainDialog = gui.createDialog(460,320);
    MainDialog.setWindowTitle('Import Terrain From Files');
    GridLayout@ layout = gui.createGridLayout();

    root.getTerrainDiffuseTextureNames(diffuseTexNames);
    root.getTerrainNormalTextureNames(normalTexNames);

    for(int i = 0;i < 6;i++)
    {
      string layname = 'Base';
      if(i > 0)
      {
          layname = i;
          @(layerCheckBoxs[i]) = gui.createCheckBox('Active');
          layout.addWidget(layerCheckBoxs[i],1,i, AlignCenter);
      }
      
      layout.addWidget(gui.createLabel('Layer ' + layname + ' :'),0,i, AlignCenter);

      @(layerCombos[i]) = createLayerCombo(); 
      layout.addWidget(layerCombos[i],2,i, AlignNone);
      
      HBoxLayout@ hblay = gui.createHBoxLayout();
      @(layerWorldSizes[i]) = gui.createLineEdit('25');
      hblay.addWidget(gui.createLabel('Size:'), 0, AlignNone);
      hblay.addWidget(layerWorldSizes[i], 1, AlignNone);
      layout.addLayout(hblay, 3, i, AlignNone);
    }
 
    layout.addWidget(gui.createLabel('Files Path :'),4,0, 1, 2, AlignLeft);
    layout.addWidget(gui.createLabel('Heightmap Name Template:'),5,0, 1, 2, AlignLeft);
    layout.addWidget(gui.createLabel('Blendmap Name Template:'),6,0, 1, 2, AlignLeft);

    @filePath = gui.createLineEdit('');
    @heightmapNameTemplate = gui.createLineEdit('heightmap_[X]x[Y].png');
    @blendmapNameTemplate = gui.createLineEdit('blendmap_[X]x[Y]_[L].png');

    layout.addWidget(filePath,4,2, 1, 3, AlignNone);
    layout.addWidget(heightmapNameTemplate,5,2, 1, 4, AlignNone);
    layout.addWidget(blendmapNameTemplate,6,2, 1, 4, AlignNone);

    @inputBias = gui.createLineEdit('0');
    @inputScale = gui.createLineEdit('1');

    @accept = gui.createButton('Accept');
    @cancel = gui.createButton('Cancel');
    @browse = gui.createButton('...');
    @help = gui.createButton('Help');

    layout.addWidget(gui.createLabel('Input Bias:'),7,2,AlignNone);
    layout.addWidget(inputBias,7,3,AlignNone);
    layout.addWidget(gui.createLabel('Input Scale:'),7,4,AlignNone);
    layout.addWidget(inputScale,7,5,AlignNone);

    layout.addWidget(browse,4,5,AlignLeft);
    layout.addWidget(accept,9,2,AlignNone);
    layout.addWidget(cancel,9,3,AlignNone);
    layout.addWidget(help,9,5,AlignNone);

    layout.setRowStretch(0,0);
    layout.setRowStretch(1,0);
    layout.setRowStretch(2,0);
    layout.setRowStretch(3,0);
    layout.setRowStretch(4,0);
    layout.setRowStretch(5,0);
    layout.setRowStretch(6,0);
    layout.setRowStretch(7,0);
    layout.setRowStretch(8,1);
    layout.setRowStretch(9,0);

    MainDialog.setLayout(layout);

    browse.connect('click()', 'void onBrowseClick()');
    accept.connect('click()', 'void onAcceptClick()');
    cancel.connect('click()', 'void onCancelClick()');
    help.connect('click()', 'void onHelpClick()');

    MainDialog.exec();

    MainDialog.destroy();
}