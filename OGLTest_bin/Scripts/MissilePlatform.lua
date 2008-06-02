Ship = {
	SectionType = "SquareCore",
	Name = "Missile Platform",
	Health = 8000,
	Angle = 0,
	Position = {x = 0, y = 0},
	AI = {AIType = "RotatingAI", AIRotationRate = 0},
	SubSections = 
	{
		{
			SectionType = "Blaster"
		},
	},
}

for i = 0, 5 do
	Ship.SubSections[i] = {}
	Ship.SubSections[i].SectionType = "Swarmer"
	Ship.SubSections[i].Angle = 0
	Ship.SubSections[i].Position = {x = -12.5 + i * 4, y = 10 }
	Ship.SubSections[i].Health = 2000
	
	Ship.SubSections[i + 5] = {}
	Ship.SubSections[i + 5].SectionType = "Swarmer"
	Ship.SubSections[i + 5].Angle = 180
	Ship.SubSections[i + 5].Position = {x = -12.5 + i * 4, y = -10 }
	Ship.SubSections[i + 5].Health = 2000

	Ship.SubSections[i + 10] = {}
	Ship.SubSections[i + 10].SectionType = "Swarmer"
	Ship.SubSections[i + 10].Angle = 90
	Ship.SubSections[i + 10].Position = {x = 10 , y = -12.5 + i * 4 }
	Ship.SubSections[i + 10].Health = 2000

end