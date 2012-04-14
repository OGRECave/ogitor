Q_ID = quat(1,0,0,0)
V_ID = vector3(0,0,0)
V_NORTH = vector3(0, 1,0)
V_SOUTH = vector3(0,-1,0)
V_WEST  = vector3(-1,0,0)
V_EAST  = vector3( 1,0,0)
V_UP    = vector3(0,0, 1)
V_DOWN  = vector3(0,0,-1)

local n = 0
function dump(x) n = n + 1; print(n, type(x),x) end

local v2 = vector3(1,2,3)
dump(v2)
dump(v2+v2)
dump(2*v2)
dump(v2*2)
dump(v2/2)
dump(v2*v2)
dump(dot(v2,v2))
dump(norm(v2))
dump(-v2)

local q = quat(1,0,0,0)
dump(q)

dump(q * v2)

local q2 = quat(180,vector3(0,0,1))
dump(q2)
dump(q2 * v2)

local q3 = q * q2
dump(q3)
dump(q3 * v2)
dump(inv(q3) * q3 * v2)

dump(v2==v2)
dump(q2==q2)
dump(q2~=q3)

dump(#v2)
dump(#q2)

print(unpack(v2))
print(unpack(q2))

dump(quat(90, V_UP) * (V_NORTH + V_EAST))
dump(quat(V_NORTH, V_EAST))
dump(quat(V_NORTH, V_EAST) * V_NORTH)

dump(V_NORTH.y)
local v = V_NORTH
v.y = 666
dump(v)

print "Done."
