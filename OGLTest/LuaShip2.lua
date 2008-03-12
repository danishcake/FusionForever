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
		Position = {x = 11, y = 5},
		Angle = -60
	},
	{
		SectionType = "WidePlate",
		Position = {x = 11, y = -5},
		Angle = -120
	},
	{
		SectionType = "WidePlate",
		Position = {x = -11, y = 5},
		Angle = 60
	},
	{
		SectionType = "WidePlate",
		Position = {x = -11, y = -5},
		Angle = 120
	},
	
    {
      SectionType = "LongRigidArm",
      Position = {x = 0, y = -10},
      Angle = 180,
      SubSections = 
      {
        {
          SectionType = "SpinningJoint",
          --Position = {x = 0, y = 0},
          RotationRate = 90,
          SubSections = 
          {
            {
              SectionType = "Blaster",
              Position = {x = 0, y = 5}
            }
          }			
        }
      }
    },
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
			SectionType = "JointAngles",
			Position = {x = 0, y = 10},
			FirstAngle = 10,
			SecondAngle = -10,
			TransitionTime = 0.2,
			PauseTime = 0.3,
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