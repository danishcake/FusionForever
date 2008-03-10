Ship = {
	SectionType = "SquareCore",
	Name = "SpikeyShip",
	Health = 1000,
	Angle = 0,
	Position = {x = 0, y = 10},
	AI = {AIType = "RotatingAI", AIRotationRate = 0.5},
	SubSections = 
	{
		{
		SectionType = "Spike",
		Position = {x = 0, y = 10},
		Angle = 0
		},
		{
		SectionType = "Spike",
		Position = {x = 8, y = 10},
		Angle = 0
		},
		{
		SectionType = "Spike",
		Position = {x = -8, y = 10},
		Angle = 0
		},
		{
		SectionType = "Spike",
		Position = {x = 0, y = -10},
		Angle = 180
		},
		{
		SectionType = "Spike",
		Position = {x = 8, y = -10},
		Angle = 180
		},
		{
		SectionType = "Spike",
		Position = {x = -8, y = -10},
		Angle = 180
		},
	}
}