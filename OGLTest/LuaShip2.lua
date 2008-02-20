Ship = {
	SectionType = "SquareCore",
	Name = "SpaceShip1",
	Health = 300,
	Angle = 90,
	Position = {x = 0, y = 10},
	AI = {AIType = "RotatingAI", AIRotationRate = 0.5},
	--AI = {AIType = "KeyboardAI"},
	SubSections = 
	{
		{
			SectionType = "HeatBeam",
			Angle = 90
		},
		{
			SectionType = "Blaster"
		},
		{
			SectionType = "SpinningJoint",
			Position = {x = 0, y = 20},
			RotationRate = 90,
			SubSections = 
			{
				{
					SectionType = "Blaster"
				}
			}
			
		}
	}
}