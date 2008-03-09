Ship = {
	SectionType = "SquareCore",
	Name = "SpaceStation",
	Health = 1000,
	Angle = 90,
	Position = {x = 0, y = 10},
	AI = {AIType = "RotatingAI", AIRotationRate = 0.5},
	--AI = {AIType = "KeyboardAI"},
	SubSections = 
	{
	  {
	    SectionType = "HomingMissileLauncher"
	  },
	  {
      SectionType = "LongRigidArm", Health = 50000,
      Position = {x = 0, y = 10},
      Angle = 0,      
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = -30,
      Position = {x = 10 * math.sin(math.rad(30)), y = 10 * math.cos(math.rad(30))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}         
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = -60,
      Position = {x = 10 * math.sin(math.rad(60)), y = 10 * math.cos(math.rad(60))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = -90,
      Position = {x = 10 * math.sin(math.rad(90)), y = 10 * math.cos(math.rad(90))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = -120,
      Position = {x = 10 * math.sin(math.rad(120)), y = 10 * math.cos(math.rad(120))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = -140,
      Position = {x = 10 * math.sin(math.rad(150)), y = 10 * math.cos(math.rad(150))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = 30,
      Position = {x = 10 * math.sin(math.rad(-30)), y = 10 * math.cos(math.rad(-30))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = 60,
      Position = {x = 10 * math.sin(math.rad(-60)), y = 10 * math.cos(math.rad(-60))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = 90,
      Position = {x = 10 * math.sin(math.rad(-90)), y = 10 * math.cos(math.rad(-90))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = 120,
      Position = {x = 10 * math.sin(math.rad(-120)), y = 10 * math.cos(math.rad(-120))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    },
    {
      SectionType = "LongRigidArm", Health = 50000,
      Angle = 140,
      Position = {x = 10 * math.sin(math.rad(-150)), y = 10 * math.cos(math.rad(-150))},
      SubSections = {{SectionType = "WidePlate", Health = 50000}}      
    }
	}
}