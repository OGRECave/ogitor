/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "OgitorsPrerequisites.h"
#include "AxisGizmo.h"

using namespace Ogre;
using namespace Ogitors;

AxisGizmo::AxisGizmo()
{
}
    
AxisGizmo::~AxisGizmo()
{
}
    
void AxisGizmo::createMesh(Ogre::SceneManager *manager, Ogre::String name)
{
    Ogre::ManualObject *mMesh = manager->createManualObject("OgitorAxisGizmoManualObject");

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_LINE_LIST);
    mMesh->position(0, 0, 0);
    mMesh->position(3, 0, 0);

    mMesh->index(0);
    mMesh->index(1);
    mMesh->end();

    float const radius = 0.22f;
    float const accuracy = 8;
    float MPI = Math::PI;

    float division = (MPI / 2.0f) / 16.0f;
    float start = division * 3;
    float end = division * 14;


    int index_pos = 0;

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_LINE_STRIP);

    for(float theta = start; theta < end; theta += division) 
    {
        mMesh->position(0, 3.0f * cos(theta), 3.0f * sin(theta));
        mMesh->index(index_pos++);
    }

    mMesh->end();

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_TRIANGLE_LIST);

    mMesh->position(2.85f,     0,     0);

    for(float theta = 0; theta < 2 * MPI; theta += MPI / accuracy) 
    {
        mMesh->position(2.95f, radius * cos(theta), radius * sin(theta));
    }
    mMesh->position(3.45f,     0,     0);

    for(int inside = 1;inside < 16;inside++)
    {
        mMesh->index(0);
        mMesh->index(inside);
        mMesh->index(inside + 1);
    }
    mMesh->index(0);
    mMesh->index(16);
    mMesh->index(1);

    for(int outside = 1;outside < 16;outside++)
    {
        mMesh->index(17);
        mMesh->index(outside);
        mMesh->index(outside + 1);
    }
    mMesh->index(17);
    mMesh->index(16);
    mMesh->index(1);
    
    mMesh->end();

    //ROTATE GIZMO

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_TRIANGLE_LIST);

    Quaternion q1;
    q1.FromAngleAxis(Degree(-90), Ogre::Vector3(0,0,1));
    Quaternion q2;
    q2.FromAngleAxis(Degree(90), Ogre::Vector3(0,1,0));
    
    Ogre::Vector3 translate1(0, 3.0f * cos(end), 3.0f * sin(end));
    Ogre::Vector3 translate2(0, 3.0f * cos(start), 3.0f * sin(start) - 0.25f);

    Ogre::Vector3 pos(-0.3f,     0,     0);
    mMesh->position(q1 * pos + translate1);

    for(float theta = 0; theta < 2 * MPI; theta += MPI / accuracy) 
    {
        pos = Ogre::Vector3(-0.3f, radius * cos(theta), radius * sin(theta));
        mMesh->position(q1 * pos + translate1);
    }
    pos = Ogre::Vector3(0.3f, 0 , 0);
    mMesh->position(q1 * pos + translate1);

    pos = Ogre::Vector3(-0.3f,     0,     0);
    mMesh->position(q2 * pos + translate2);

    for(float theta = 0; theta < 2 * MPI; theta += MPI / accuracy) 
    {
        pos = Ogre::Vector3(-0.3f, radius * cos(theta), radius * sin(theta));
        mMesh->position(q2 * pos + translate2);
    }
    pos = Ogre::Vector3(0.3f, 0 , 0);
    mMesh->position(q2 * pos + translate2);

    for(int inside = 1;inside < 16;inside++)
    {
        mMesh->index(0);
        mMesh->index(inside);
        mMesh->index(inside + 1);
    }
    mMesh->index(0);
    mMesh->index(16);
    mMesh->index(1);

    for(int outside = 1;outside < 16;outside++)
    {
        mMesh->index(17);
        mMesh->index(outside);
        mMesh->index(outside + 1);
    }
    mMesh->index(17);
    mMesh->index(16);
    mMesh->index(1);
    
    for(int inside = 19;inside < 34;inside++)
    {
        mMesh->index(18);
        mMesh->index(inside);
        mMesh->index(inside + 1);
    }
    mMesh->index(18);
    mMesh->index(34);
    mMesh->index(19);

    for(int outside = 19;outside < 34;outside++)
    {
        mMesh->index(35);
        mMesh->index(outside);
        mMesh->index(outside + 1);
    }
    mMesh->index(35);
    mMesh->index(34);
    mMesh->index(19);

    mMesh->end();

    //SCALE GIZMO

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_TRIANGLE_LIST);

    mMesh->position(2.85f,     0,     0);

    for(float theta = 0; theta < 2 * MPI; theta += MPI / accuracy) 
    {
        mMesh->position(2.85f, radius * cos(theta), radius * sin(theta));
    }
    mMesh->position(3.45f,     0,     0);

    mMesh->position(3.40f,  0.20f,  0.20f);
    mMesh->position(3.40f,  0.20f, -0.20f);
    mMesh->position(3.40f, -0.20f, -0.20f);
    mMesh->position(3.40f, -0.20f,  0.20f);
    mMesh->position(3.50f,  0.20f,  0.20f);
    mMesh->position(3.50f,  0.20f, -0.20f);
    mMesh->position(3.50f, -0.20f, -0.20f);
    mMesh->position(3.50f, -0.20f,  0.20f);

    for(int inside = 1;inside < 16;inside++)
    {
        mMesh->index(0);
        mMesh->index(inside);
        mMesh->index(inside + 1);
    }
    mMesh->index(0);
    mMesh->index(16);
    mMesh->index(1);

    for(int outside = 1;outside < 16;outside++)
    {
        mMesh->index(17);
        mMesh->index(outside);
        mMesh->index(outside + 1);
    }
    mMesh->index(17);
    mMesh->index(16);
    mMesh->index(1);

    mMesh->index(18);
    mMesh->index(19);
    mMesh->index(20);
    mMesh->index(18);
    mMesh->index(20);
    mMesh->index(21);

    mMesh->index(22);
    mMesh->index(23);
    mMesh->index(24);
    mMesh->index(22);
    mMesh->index(24);
    mMesh->index(25);

    mMesh->index(18);
    mMesh->index(22);
    mMesh->index(25);
    mMesh->index(18);
    mMesh->index(25);
    mMesh->index(21);

    mMesh->index(19);
    mMesh->index(23);
    mMesh->index(24);
    mMesh->index(19);
    mMesh->index(24);
    mMesh->index(20);

    mMesh->index(18);
    mMesh->index(22);
    mMesh->index(23);
    mMesh->index(18);
    mMesh->index(23);
    mMesh->index(19);

    mMesh->index(21);
    mMesh->index(20);
    mMesh->index(24);
    mMesh->index(21);
    mMesh->index(24);
    mMesh->index(25);

    mMesh->end();

    mMesh->convertToMesh(name, "EditorResources");

    manager->destroyManualObject(mMesh);
}

void AxisGizmo::createPlaneMesh(Ogre::SceneManager *manager, Ogre::String name)
{
    Ogre::ManualObject *mMesh = manager->createManualObject("OgitorAxisPlaneGizmoManualObject");

    mMesh->begin("AxisGizmo_Material", RenderOperation::OT_TRIANGLE_LIST);

    mMesh->position( 0, 1, 0);
    mMesh->position( 1, 1, 0);
    mMesh->position( 1, 0, 0);
    mMesh->position( 0, 0, 0);

    mMesh->index(0);
    mMesh->index(1);
    mMesh->index(2);
    mMesh->index(0);
    mMesh->index(2);
    mMesh->index(3);

    mMesh->end();

    mMesh->convertToMesh(name, "EditorResources");

    manager->destroyManualObject(mMesh);
}