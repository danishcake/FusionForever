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
			SectionType = "WidePlate",
			Position = {x = 0, y = -12.5},
			Angle = 180
		},
		{
			SectionType = "ProngRH",
			Position = {x = 7.5, y = 15},
			SubSections = 
			{
				{
					SectionType = "HeatBeam"
				}
			}
		},
		{
			SectionType = "ProngLH",
			Position = {x = -7.5, y = 15},
			SubSections = 
			{
				{
					SectionType = "HeatBeam"
				}
			}
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