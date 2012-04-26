void main()
{
    Dialog@ dlg = gui.createDialog(350,200);
    dlg.setWindowTitle('Experimental QT Dialog');
    VBoxLayout@ layout = gui.createVBoxLayout();
    layout.addWidget(gui.createButton('Button 1'),0,AlignNone);

    ComboBox@ combo = gui.createComboBox();
    combo.setFixedSize(100, 50);
    combo.setIconSize(32, 32);
    combo.addItem('Text Only');
    combo.addItem(Icon(':/icons/trash.svg'), 'With Icon');
    layout.addWidget(combo,1,AlignNone);
    
    dlg.setLayout(layout);

    dlg.exec();
    dlg.destroy();
}