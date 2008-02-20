Ship = {
	SectionType = "SquareCore",
	Name = "SpaceShip1",
	Health = 2000,
	Angle = 90,
	Position = {x = 0, y = 10},
	AI = {AIType = "RotatingAI", AIRotationRate = 0.5},
	--AI = {AIType = "KeyboardAI"},
	SubSections = 
	{
		{
			SectionType = "HeatBeam",
			Position = {x = 10, y = 5},
			Angle = -10
		},
		{
			SectionType = "HeatBeam",
			Position = {x = -10, y = 5},
			Angle = 10
		},
		{
			SectionType = "Blaster"
		},
		{
			SectionType = "SpinningJoint",
			Position = {x = 10, y = 0},
			RotationRate = 90,
			SubSections = 
			{
				{
					SectionType = "Blaster",
					Position = {x = 0, y = 5}
				}
			}			
		},
		{
			SectionType = "SpinningJoint",
			Position = {x = -10, y = 0},
			RotationRate = -90,
			SubSections = 
			{
				{
					SectionType = "Blaster",
					Position = {x = 0, y = 5}
				}
			}			
		},
		{
			SectionType = "JointAngles",
			Position = {x = 0, y = 10},
			FirstAngle = 30,
			SecondAngle = -30,
			TransitionTime = 1,
			PauseTime = 1,
			SubSections = 
			{
				{
					SectionType = "HeatBeam",
					Position = {x = 0, y =5}
				}
			}
		}
	}
}