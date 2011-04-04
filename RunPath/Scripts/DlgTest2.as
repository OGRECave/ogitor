Dialog@     MainDialog = null;
ComboBox@[] layerCombos(6);
CheckBox@[] layerCheckBoxs(6);
LineEdit@   filePath = null;  
LineEdit@   heightmapNameTemplate = null;  
LineEdit@   blendmapNameTemplate = null;  
Button@     accept = null;  
Button@     cancel = null;  
Button@     browse = null;  
string      heightmapfile = '';
string      blendmapfile = '';
string      filespath = '';
int         SectorStartX = 1000;    
int         SectorStartY = 1000;    
int         SectorEndX = -1000;    
int         SectorEndY = -1000;   
StringVector diffuseTexNames;
StringVector normalTexNames;

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

void startImport()
{
    int layerCount = 1;
    while(layerCheckBoxs[layerCount].isChecked())
        layerCount++;

    string ImportErrors = '';

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
                 TerrainPageEditor@ pageED = root.createTerrainPage(XP, YP, diffuseTexNames[index], normalTexNames[index]);
                 pageED.importHeightMap(search);
                 for(int L = 1;L < layerCount;L++)
                 {
                     int bindex = layerCombos[L].getCurrentIndex();
                     pageED.addLayer(diffuseTexNames[bindex], normalTexNames[bindex], false);
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
        ImportErrors = 'Import Completed with following errors:\n' + ImportErrors;
    
    system.displayMessageDialog(UTFString(ImportErrors), DLGTYPE_OK);
}

void onBrowseClick()
{
    string path = system.displayDirectorySelector(UTFString('Select Directory Containing Files'));
    if(path != '')
        filePath.setText(GUIString(path));
}

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
             string msg = 'Terrain files found: (%1,%2) to (%3,%4)\nDo you want to Import?';
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

void onCancelClick()
{
    MainDialog.reject();
}

void main()
{
    @MainDialog = gui.createDialog(460,250);
    MainDialog.setWindowTitle('Import Terrain From Files');
    GridLayout@ layout = gui.createGridLayout();

    root.getTerrainDiffuseTextureNames(diffuseTexNames);
    root.getTerrainNormalTextureNames(normalTexNames);

    for(int i = 0;i < 6;i++)
    {
      @(layerCombos[i]) = createLayerCombo(); 
      layout.addWidget(layerCombos[i],1,i, AlignNone);

      string layname = 'Base';
      if(i > 0)
      {
          layname = i;
          @(layerCheckBoxs[i]) = gui.createCheckBox('Active');
          layout.addWidget(layerCheckBoxs[i],2,i, AlignCenter);
      }
      
      layout.addWidget(gui.createLabel('Layer ' + layname + ' :'),0,i, AlignCenter);
    }
 
    layout.addWidget(gui.createLabel('Files Path :'),3,0, 1, 2, AlignLeft);
    layout.addWidget(gui.createLabel('Heightmap Name Template:'),4,0, 1, 2, AlignLeft);
    layout.addWidget(gui.createLabel('Blendmap Name Template:'),5,0, 1, 2, AlignLeft);

    @filePath = gui.createLineEdit('');
    @heightmapNameTemplate = gui.createLineEdit('heightmap_[X]x[Y].png');
    @blendmapNameTemplate = gui.createLineEdit('blendmap_[X]x[Y]_[L].png');

    layout.addWidget(filePath,3,2, 1, 3, AlignNone);
    layout.addWidget(heightmapNameTemplate,4,2, 1, 4, AlignNone);
    layout.addWidget(blendmapNameTemplate,5,2, 1, 4, AlignNone);

    @accept = gui.createButton('Accept');
    @cancel = gui.createButton('Cancel');
    @browse = gui.createButton('...');

    layout.addWidget(browse,3,5,AlignLeft);
    layout.addWidget(accept,7,2,AlignNone);
    layout.addWidget(cancel,7,3,AlignNone);

    layout.setRowStretch(0,0);
    layout.setRowStretch(1,0);
    layout.setRowStretch(2,0);
    layout.setRowStretch(3,0);
    layout.setRowStretch(4,0);
    layout.setRowStretch(5,0);
    layout.setRowStretch(6,1);
    layout.setRowStretch(7,0);

    MainDialog.setLayout(layout);

    browse.connect('click()', 'void onBrowseClick()');
    accept.connect('click()', 'void onAcceptClick()');
    cancel.connect('click()', 'void onCancelClick()');

    MainDialog.exec();

    MainDialog.destroy();
}