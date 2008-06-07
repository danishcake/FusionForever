Ship = {
	SectionType = "SquareCore",
	Name = "Missile Platform",
	Health = 80000,
	Angle = 0,
	Position = {x = 0, y = 0},
	AI = {AIType = "RotatingAI", AIRotationRate = 0},
	SubSections = 
	{
		{
			SectionType = "HomingMissileLauncher",
			Position = {x = 0, y = 10},
		},
		{
			SectionType = "HomingMissileLauncher",
			Position = {x = 0, y = -10},
			Angle = 180
		},
		{
			SectionType = "WidePlate",
			Angle = 0,
			Position = {x = 20 * math.sin(math.rad(0)), y = 20 * math.cos(math.rad(0))},
			Health = 9000
		},
		{
			SectionType = "WidePlate",
			Angle = 90,
			Position = {x = 20 * math.sin(math.rad(90)), y = 20 * math.cos(math.rad(90))},
			Health = 9000
		},
		{
			SectionType = "WidePlate",
			Angle = 180,
			Position = {x = 20 * math.sin(math.rad(180)), y = 20 * math.cos(math.rad(180))},
			Health = 9000
		},
		{
			SectionType = "WidePlate",
			Angle = 270,
			Position = {x = 20 * math.sin(math.rad(270)), y = 20 * math.cos(math.rad(270))},
			Health = 9000
		},
		{
			SectionType = "RightAngleTriangle",
			Angle = 0,
			Position = {x = 20 * math.sin(math.rad(0+45)), y = 20 * math.cos(math.rad(0+45))},
			Health = 9000
		},
		{
			SectionType = "RightAngleTriangle",
			Angle = 90,
			Position = {x = 20 * math.sin(math.rad(90+45)), y = 20 * math.cos(math.rad(90+45))},
			Health = 9000
		},
		{
			SectionType = "RightAngleTriangle",
			Angle = 180,
			Position = {x = 20 * math.sin(math.rad(180+45)), y = 20 * math.cos(math.rad(180+45))},
			Health = 9000
		},
		{
			SectionType = "RightAngleTriangle",
			Angle = 270,
			Position = {x = 20 * math.sin(math.rad(270+45)), y = 20 * math.cos(math.rad(270+45))},
			Health = 9000
		},
	},
}

initial_index = table.getn(Ship.SubSections)

for i = 1, 2 do

	Ship.SubSections[i + initial_index] = {}
	Ship.SubSections[i + initial_index].SectionType = "Swarmer"
	Ship.SubSections[i + initial_index].Angle = 0
	Ship.SubSections[i + initial_index].Position = {x = -30 + i * 20, y = 10 }
	Ship.SubSections[i + initial_index].Health = 2000
	Ship.SubSections[i + initial_index].FiringDelay = i * 0.45
	
	Ship.SubSections[i + 2 + initial_index] = {}
	Ship.SubSections[i + 2 + initial_index].SectionType = "Swarmer"
	Ship.SubSections[i + 2 + initial_index].Angle = 180
	Ship.SubSections[i + 2 + initial_index].Position = {x = -30 + i * 20, y = -10 }
	Ship.SubSections[i + 2 + initial_index].Health = 2000
	Ship.SubSections[i + 2 + initial_index].FiringDelay = i * 0.45

	Ship.SubSections[i + 4 + initial_index] = {}
	Ship.SubSections[i + 4 + initial_index].SectionType = "Swarmer"
	Ship.SubSections[i + 4 + initial_index].Angle = 90
	Ship.SubSections[i + 4 + initial_index].Position = {x = 10 , y = -30 + i * 20 }
	Ship.SubSections[i + 4 + initial_index].Health = 2000
	Ship.SubSections[i + 4 + initial_index].FiringDelay = i * 0.45
end

