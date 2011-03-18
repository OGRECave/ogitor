void main()
{
    BaseEditor @b;
    Vector3 pos(0,0,0);
 
    EditorVector list;
    root.getObjectList('Entity Object', list);

    uint count = list.size();
    for(uint idx = 0; idx < count; ++idx) 
    {
        @b = list[idx];
        string msg = 'Object Found : ' + b.getName();
        printConsole(msg);
        CustomPropertySet @pset = b.getCustomProperties();
        if(!pset.hasProperty('physics::mass'))
        {
            pset.addProperty('physics::mass', PROP_REAL);
            pset.addProperty('physics::material', PROP_STRING);
            printConsole('Custom properties created for object.');
        }
    }
    list.clear();
}