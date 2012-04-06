void main()
{
    BaseEditor @b = root.findObject('Barrel#0', 0);

    if( b !is null )
    {
        Vector3 pos;
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        if(b.hasProperty('position'))
        {
            PropertyVector3 @pp = cast<PropertyVector3@>(b.getProperty('position'));
            pp.set(pos);
            b.setSelected(true);
            ViewportEditor @v = root.getViewport();
            v.focusSelectedObject();
        }
    }
}