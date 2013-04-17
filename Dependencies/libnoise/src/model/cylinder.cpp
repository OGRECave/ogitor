// cylinder.cpp
//
// Copyright (C) 2003, 2004 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//

#include "mathconsts.h"
#include "model/cylinder.h"

using namespace noise;
using namespace noise::model;

Cylinder::Cylinder (): m_pModule (NULL)
{
}

Cylinder::Cylinder (const module::Module& module):
  m_pModule (&module)
{
}

double Cylinder::GetValue (double angle, double height) const
{
  assert (m_pModule != NULL);

  double x, y, z;
  x = cos (angle * DEG_TO_RAD);
  y = height;
  z = sin (angle * DEG_TO_RAD);
  return m_pModule->GetValue (x, y, z);
}
