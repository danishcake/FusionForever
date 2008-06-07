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
		SectionType = "SpikeLH",
		Position = {x = 25, y = 25}
	},
	{
		SectionType = "SpikeRH",
		Position = {x = -25, y = 25}
	},
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
          RotationRate = 50,
          SubSections = 
          {
            {
              SectionType = "SpunStar",
              SubSections = 
              {
				{
					SectionType = "TrackerJoint",
					OnlyWhenFiring = false,
					Position = {x = -16.18034, y = 11.755705},
					SubSections = {{SectionType = "Blaster"}},
				},
				{
					SectionType = "TrackerJoint",
					OnlyWhenFiring = false,
					Position = {x = 6.1803399, y = 19.02113},
					SubSections = {{SectionType = "Blaster"}},
				},
				{
					SectionType = "TrackerJoint",
					OnlyWhenFiring = false,
					Position = {x = -16.18034, y = -11.755705},
					SubSections = {{SectionType = "Blaster"}},
				},
				{
					SectionType = "TrackerJoint",
					OnlyWhenFiring = false,
					Position = {x = 6.1803399, y = -19.02113},
					SubSections = {{SectionType = "Blaster"}},
				},
				{
					SectionType = "TrackerJoint",
					OnlyWhenFiring = false,
					Position = {x = 20, y = 0},
					SubSections = {{SectionType = "Blaster"}},
				},
				
              }
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
