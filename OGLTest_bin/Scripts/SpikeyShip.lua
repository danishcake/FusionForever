Ship = {
	SectionType = "SquareCore",
	Name = "SpikeyShip",
	Health = 1000,
	Angle = 0,
	AI = {AIType = "RotatingAI", AIRotationRate = 0.5},
	SubSections = 
	{
		{
			SectionType = "ProngLH",
			Position = {x = -10, y = 5},
		},
		{
			SectionType = "ProngRH",
			Position = {x = 10, y = 5},
		},
		{
			SectionType = "WingRH",
			Position = {x = 7.5, y = 0},
			SubSections = 
			{
				{
					SectionType = "RigidArm",
					Delay = 0.5,
					SubSections = 
					{
						{
							SectionType = "Blaster",
							Angle = 90,
						},
						{
							SectionType = "RigidArm",
							Delay = 0.5,
							SubSections = 
							{
								{
									SectionType = "Blaster",
									Angle = 90,
								},
							},
						},
					},
				},
			},
		},
		{
			SectionType = "WingLH",
			Position = {x = -7.5, y =0},
		},
		{
			SectionType = "SweptWingLH",
			Position = {x = -7.5, y = -5},
		},
		{
			SectionType = "SweptWingRH",
			Position = {x = 7.5, y = -5},
		},
		{
			SectionType = "SemiCircle",
			Position = {x = 0, y = -5},
			Angle = 180
		}
	}
}