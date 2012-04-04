void main()
{
    BaseEditor @b;
    Vector3 pos(0,0,0);
 
    EditorVector list1;
    EditorVector list2;

    // get all objects (ETYPE_BASE means all) matching 'tree*'
    root.getObjectListByName(ETYPE_BASE, 'tree.*', false, list1);

    // further refine the list with regexp, filling list2 with only 
    // those that DONT MATCH (true means inverse match)
    root.regexpByCustomProperty('physics::.*', true, list1, list2);

    uint count = list2.size();
    for(uint idx = 0; idx < count; ++idx) 
    {
        @b = list2[idx];
        string msg = 'Mathing Object Found : ' + b.getName();
        printConsole(msg);

        // Here we dont check with hasProperty anymore since 
        // we refined(removed from list) all objects that have it, with regexp        
        CustomPropertySet @pset = b.getCustomProperties();

        pset.addProperty('physics::mass', PROP_REAL);
        pset.addProperty('physics::material', PROP_STRING);
        
        printConsole('Custom properties created for object.');
    }
    list1.clear();
    list2.clear();
}