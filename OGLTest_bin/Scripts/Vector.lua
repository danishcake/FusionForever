--Vector class

Vector = {}
vec_mt = {}

function Vector:new(_x, _y)
	return setmetatable({x=_x or 0, y=_y or 0}, vec_mt)
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
	self.x = self.x / length
	self.y = self.y / length
	return self
end

function Vector:dot(vec)
	return self.x * vec.x + self.y * vec.y
end

function Vector:cross(vec)
	return self.x * vec.y - self.y * vec.x
end

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
