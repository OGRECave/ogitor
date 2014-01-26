Dialog@     dlg = null;
GridLayout@ dlgLayout = null;
GridLayout@ mainLayout = null;
Label@ objNameLabel = null;
LineEdit@ xEdit = null;
LineEdit@ yEdit = null;
LineEdit@ zEdit = null;
LineEdit@ rotXEdit = null;
LineEdit@ rotYEdit = null;
LineEdit@ rotZEdit = null;
LineEdit@ rotAmmount = null;
LineEdit@ cloneCountEdit = null;
Button@ okBtn = null;
Button@ cancelBtn = null;
Button@ updateBtn = null;
bool finished = false;

Vector3 offset(0, 0, 0); 
Vector3 rotationAxis(0, 1, 0);
float degrees = 0;
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

void updateClones()
{
	for(int i = 0; i < (cloneCount); i++)
	{
		BaseEditor @newObj = list[i];
		
		Quaternion q;
		q.FromAngleAxis(degrees * (i + 1), rotationAxis);
		newObj.setDerivedPosition(b.getDerivedPosition() + offset * (i + 1));
		newObj.setDerivedOrientation(q);

		EditorVector v;
		v.push_back(b);
		m.set(v);
	}
}

void onUpdateClick()
{
	string temp = xEdit.getText();
	offset.x = parseFloat(temp);
	temp = yEdit.getText();
	offset.y = parseFloat(temp);
	temp = zEdit.getText();
	offset.z = parseFloat(temp);

	temp = rotXEdit.getText();
	rotationAxis.x = parseFloat(temp);
	temp = rotYEdit.getText();
	rotationAxis.y = parseFloat(temp);
	temp = rotZEdit.getText();
	rotationAxis.z = parseFloat(temp);
	temp = rotAmmount.getText();
	degrees = parseFloat(temp) * 0.0174532925f;


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
				list.push_back(newObj);
			}
			updateClones();

			if(oldCloneCount == -1)
			{
				oldCloneCount = cloneCount;			
			}

			EditorVector v;
			v.push_back(b);
			m.set(v);
		}
	}
	else
	{
		if(oldCloneCount != cloneCount)
		{
			if(cloneCount > list.size())
			{
				// add more editors
				while(list.size() != cloneCount)
				{
					BaseEditor @newObj = root.cloneObject(b);
					list.push_back(newObj);
				}
			}
			else
			{
				// remove editors
				while(list.size() != cloneCount)
				{
					BaseEditor @ed = list[list.size()-1];
					root.destroyObject(ed, true, false);
					list.pop_back();
				}
			}

			oldCloneCount = cloneCount;
		}

		updateClones();
	}
}

void onCancelClick()
{
	for(int i = 0; i < list.size(); i++)
	{
		BaseEditor @ed = list[i];
		root.destroyObject(ed, true, false);
	}
	list.clear();
	dlg.reject();
	finished = true;
}

void createGUI(string objectName)
{
	@dlg = gui.createDialog(295, 200);
	dlg.setWindowTitle("Batch clone object");
	@dlgLayout = gui.createGridLayout();
	@mainLayout = gui.createGridLayout();
	
	mainLayout.addWidget(gui.createLabel("Selected object:"), 0, 0, 1, 1, AlignLeft);
	@objNameLabel = gui.createLabel(objectName);
	mainLayout.addWidget(objNameLabel, 0, 1, 1, 1, AlignLeft);

	// translation
	mainLayout.addWidget(gui.createLabel("Offset X"), 1, 0, 1, 1, AlignCenter);
	@xEdit = gui.createLineEdit("0");
	mainLayout.addWidget(xEdit, 2, 0, 1, 1, AlignNone);
	
	mainLayout.addWidget(gui.createLabel("Offset Y"), 1, 1, 1, 1, AlignCenter);	
	@yEdit = gui.createLineEdit("0");
	mainLayout.addWidget(yEdit, 2, 1, 1, 1, AlignNone);
	
	mainLayout.addWidget(gui.createLabel("Offset Z"), 1, 2, 1, 1, AlignCenter);	
	@zEdit = gui.createLineEdit("0");
	mainLayout.addWidget(zEdit, 2, 2, 1, 1, AlignNone);
	
	// rotation
	mainLayout.addWidget(gui.createLabel("rot X"), 3, 0, 1, 1, AlignCenter);
	@rotXEdit = gui.createLineEdit("0");
	mainLayout.addWidget(rotXEdit, 4, 0, 1, 1, AlignNone);
	
	mainLayout.addWidget(gui.createLabel("rot Y"), 3, 1, 1, 1, AlignCenter);	
	@rotYEdit = gui.createLineEdit("0");
	mainLayout.addWidget(rotYEdit, 4, 1, 1, 1, AlignNone);
	
	mainLayout.addWidget(gui.createLabel("rot Z"), 3, 2, 1, 1, AlignCenter);	
	@rotZEdit = gui.createLineEdit("0");
	mainLayout.addWidget(rotZEdit, 4, 2, 1, 1, AlignNone);

	mainLayout.addWidget(gui.createLabel("Degrees"), 5, 0, 1, 1, AlignRight);
	@rotAmmount = gui.createLineEdit("0");
	mainLayout.addWidget(rotAmmount, 5, 1, 1, 1, AlignNone);

	mainLayout.addWidget(gui.createLabel("Clone count"), 6, 0, 1, 1, AlignRight);
	@cloneCountEdit = gui.createLineEdit("1");
	mainLayout.addWidget(cloneCountEdit, 6, 1, 1, 1, AlignNone);
	
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

	list.clear();
    dlg.destroy();
}
