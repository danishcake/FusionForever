--Path class
require "Vector"

Path = {}
path_mt = {}

function Path:new(_looping)
	return setmetatable({points = {}, looping = _looping or false, time = 0, frac_time = 0, point_index = 1, position = Vector:new(0,0), mode='cosine'}, path_mt)
end

function Path:tick(_dt)
	self.time = self.time + _dt
	self.frac_time = self.frac_time + _dt

	if #self.points == 0 then
		self.position = nil
	elseif #self.points == 1 then
		self.position = points[1].p
	else
		if self.frac_time >= self.points[self.point_index].t then
			self.frac_time = 0
			self.point_index = self.point_index + 1
			if self.point_index > #self.points then
				self.point_index = #self.points
			end
		end
		if self.point_index < #self.points then
			local tf = self.frac_time / self.points[self.point_index].t

			if self.mode == 'linear' then
				self.position = self.points[self.point_index].p * (1.0 - tf) + self.points[self.point_index+1].p * tf
			else
				local cf = math.cos(tf * math.pi / 2)
				self.position = self.points[self.point_index].p * cf + self.points[self.point_index+1].p * (1.0 - cf)
			end
		else
			self.position = self.points[#self.points].p
		end
	end
	return self.position
end


function Path:getPosition()
	return self.position
end

function Path:addPoint(point, time)
	table.insert(self.points, {p=point, t=time})
end

function Path:reset()
	self.point_index = 1
	self.frac_time = 0
end

function Path:setInterpolateLinear()
	self.mode = 'linear'
end

function Path:setInterpolateCosine()
	self.mode = 'cosine'
end

path_mt.__index = Path

path_mt.__newindex = function(tbl, key, val)
	error("Table is locked to new indicies. Key " .. key .. " not found.")
end
