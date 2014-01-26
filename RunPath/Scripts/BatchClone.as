Dialog@     dlg = null;
GridLayout@ dlgLayout = null;
GridLayout@ mainLayout = null;
Label@ objNameLabel = null;
LineEdit@ xEdit = null;
LineEdit@ yEdit = null;
LineEdit@ zEdit = null;
LineEdit@ cloneCountEdit = null;
Button@ okBtn = null;
Button@ cancelBtn = null;
Button@ updateBtn = null;
bool finished = false;

Vector3 offset(0, 0, 0); 
Vector3 rotation(0, 1, 0);
float rot = 0;
int cloneCount = 5;
int oldCloneCount = -1;
EditorVector list;
MultiSelection @m = null;
BaseEditor @b = null;

void onOKClick()
{
	if(list.size() != 0)
	{
		m.set(list);
	}

	dlg.accept();
	finished = true;
}

void onUpdateClick()
{
	string temp = xEdit.getText();
	offset.x = parseFloat(temp);
	temp = yEdit.getText();
	offset.y = parseFloat(temp);
	temp = zEdit.getText();
	offset.z = parseFloat(temp);

	temp = cloneCountEdit.getText();
	cloneCount = parseInt(temp);

	if(list.size() == 0)
	{
		if(m.isSingle())
		{
			string objType = b.getTypeName();
		
			for(int i = 1; i < (cloneCount + 1); i++)
			{
				BaseEditor @newObj = root.cloneObject(b);
				newObj.setDerivedPosition(b.getDerivedPosition() + offset * i);
				list.push_back(newObj);
			}
		}
		EditorVector v;
		v.push_back(b);
		m.set(v);
	}
	else
	{
		if(oldCloneCount != cloneCount)
		{
			// TODO: update here

			oldCloneCount = cloneCount;
		}
		else
		{
			for(int i = 0; i < (cloneCount); i++)
			{
				BaseEditor @newObj = list[i];
				newObj.setDerivedPosition(b.getDerivedPosition() + offset * (i + 1));

				EditorVector v;
				v.push_back(b);
				m.set(v);
			}
		}
	}
}

void onCancelClick()
{
	dlg.reject();
	finished = true;
}

void createGUI(string objectName)
{
	@dlg = gui.createDialog(295, 160);
	dlg.setWindowTitle("Batch clone object");
	@dlgLayout = gui.createGridLayout();
	@mainLayout = gui.createGridLayout();
	
	@xEdit = gui.createLineEdit("0");
	mainLayout.addWidget(xEdit, 1, 1, 1, 1, AlignNone);
		
	@yEdit = gui.createLineEdit("0");
	mainLayout.addWidget(yEdit, 2, 1, 1, 1, AlignNone);
		
	@zEdit = gui.createLineEdit("0");
	mainLayout.addWidget(zEdit, 3, 1, 1, 1, AlignNone);
	
	@cloneCountEdit = gui.createLineEdit("1");
	mainLayout.addWidget(cloneCountEdit, 4, 1, 1, 1, AlignNone);

	@objNameLabel = gui.createLabel(objectName);
	mainLayout.addWidget(objNameLabel, 0, 1, 1, 1, AlignCenter);

	mainLayout.addWidget(gui.createLabel("Selected object:"), 0, 0, 1, 1, AlignRight);
	mainLayout.addWidget(gui.createLabel("Offset X"), 1, 0, 1, 1, AlignRight);
	mainLayout.addWidget(gui.createLabel("Offset Y"), 2, 0, 1, 1, AlignRight);
	mainLayout.addWidget(gui.createLabel("Offset Z"), 3, 0, 1, 1, AlignRight);
	mainLayout.addWidget(gui.createLabel("Clone count"), 4, 0, 1, 1, AlignRight);
	
	@okBtn = gui.createButton("OK");
    dlgLayout.addWidget(okBtn, 1, 0, 1, 1, AlignNone);

    @updateBtn = gui.createButton("Update");
    dlgLayout.addWidget(updateBtn, 1, 2, 1, 1, AlignNone);

	@cancelBtn = gui.createButton("Cancel");
	dlgLayout.addWidget(cancelBtn, 1, 1, 1, 1, AlignNone);

	dlgLayout.addLayout(mainLayout, 0, 0, 1, 3, AlignCenter);
	
	okBtn.connect('click()', 'void onOKClick()');
    updateBtn.connect('click()', 'void onUpdateClick()');
    cancelBtn.connect('click()', 'void onCancelClick()');
}

void main()
{
	@m = root.getSelection();
    @b = m.getFirstObject();
	
	if(b == null || !m.isSingle())
	{
		system.displayMessageDialog(UTFString("Please select a single object first!"), DLGTYPE_OK);
		return;
	}
	else
	{
		createGUI(b.getName());
	}

	dlg.setLayout(dlgLayout);
	dlg.show();
	
	while(!finished)
	{
		gui.processEvents();
	}

    dlg.destroy();
}
