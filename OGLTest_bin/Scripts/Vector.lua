--Vector class

Vector = {}
vec_mt = {}

function Vector:new(_x, _y)
	return setmetatable({x=_x or 0, y=_y or 0}, vec_mt)
end

function Vector:random_ring(inner, outer)
	local angle = math.random(0, 360)
	local radius = math.random(inner, outer)
	return setmetatable({x = math.sin(math.rad(angle)) * radius, y = math.cos(math.rad(angle)) * radius}, vec_mt)
end

function Vector:from_angle(angle)
	print(angle)
	return setmetatable({x=math.sin(math.rad(angle)), y = math.cos(math.rad(angle))}, vec_mt)
end

function Vector:copy()
	return Vector:new(self.x, self.y)
end

function Vector:length()
	return math.sqrt(self.x ^ 2 + self.y ^ 2)
end

function Vector:lengthSqr()
	return self.x ^ 2 + self.y ^ 2
end

function Vector:normalise()
	local length = self:length()
	if length > 0 then
		self.x = self.x / length
		self.y = self.y / length
	end
	return self
end

function Vector:dot(vec)
	return self.x * vec.x + self.y * vec.y
end

function Vector:cross(vec)
	return self.x * vec.y - self.y * vec.x
end


--Table reuse functions - use these where possible with a long lived v3
--This prevents the GC from kicking in too much
function Vector:subtract(v1, v2, v3)
	v3.x = v2.x - v1.x
	v3.y = v2.y - v1.y
end

function Vector:add(v1, v2, v3)
	v3.x = v2.x + v1.x
	v3.y = v2.y + v1.y
end

function Vector:multiply(v1, s1, v3)
	v3.x = v1.x * s1
	v3.y = v1.y * s1
end

function Vector:divide(v1, s1, v3)
	v3.x = v1.x / s1
	v3.y = v1.y / s1
end

--Table operators - they look good but cause allocation of a new table
vec_mt.__add = function(vec1, vec2)
	return Vector:new(vec1.x + vec2.x, vec1.y + vec2.y)
end

vec_mt.__sub = function(vec1, vec2)
	return Vector:new(vec1.x - vec2.x, vec1.y - vec2.y)
end

vec_mt.__mul = function(vec1, factor)
	return Vector:new(vec1.x * factor, vec1.y * factor)
end

vec_mt.__div = function(vec1, divisor)
	return Vector:new(vec1.x / divisor, vec1.y / divisor)
end

vec_mt.__index = Vector

vec_mt.__newindex = function(tbl, key, val)
	error("Table is locked to new indicies. Key " .. key .. " not found.")
end
