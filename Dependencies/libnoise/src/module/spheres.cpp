// spheres.cpp
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

#include "misc.h"
#include "module/spheres.h"

using namespace noise::module;

Spheres::Spheres ():
  Module (GetSourceModuleCount ()),
  m_frequency (DEFAULT_SPHERES_FREQUENCY)
{
}

double Spheres::GetValue (double x, double y, double z) const
{
  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;

  double distFromCenter = sqrt (x * x + y * y + z * z);
  double distFromSmallerSphere = distFromCenter - floor (distFromCenter);
  double distFromLargerSphere = 1.0 - distFromSmallerSphere;
  double nearestDist = GetMin (distFromSmallerSphere, distFromLargerSphere);
  return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
}
