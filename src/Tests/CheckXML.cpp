#include "XMLSection.h"
#include "Core.h"
#include <UnitTest++.h>

TEST(XMLSectionTests)
{
	XMLSection* section = XMLSection::CreateXMLSection("TestSection");
	
	CHECK(section);
	delete section;
}

/* Tests a ship formed of all hardcoded sections and a TestSection */
TEST(XMLShipTests)
{
	Core_ptr core = Core::CreateCore("TestShip");
	CHECK(core);
	delete core;
} 
