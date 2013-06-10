void main()
{
    Vector3 pos;
    Quaternion orient;

    pos = root.getViewport().getCamera().getDerivedPosition();
    orient = root.getViewport().getCamera().getDerivedOrientation();
    
    root.getSelection().setDerivedOrientation(orient);
    root.getSelection().setDerivedPosition(pos);
    
    string msg = 'Pos: (%1 %2 %3) Orient: (%4 %5 %6 %7)'; 
    printConsole(msg.arg(pos.x).arg(pos.y).arg(pos.z).arg(orient.w).arg(orient.x).arg(orient.y).arg(orient.z));
}