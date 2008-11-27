#include "SquareCore.h"
#include <UnitTest++.h>
#include <vector>
#include "SortFunctors.h"

using namespace std;

TEST(CheckSortFunctors)
{
	vector<Core_ptr> cores;
	for(int i = 0; i < 20; i ++)
	{
		Core_ptr n_core = new SquareCore(NULL);
		n_core->SetPosition(Vector3f(static_cast<float>(i), static_cast<float>(2 * i), 0));
		cores.push_back(n_core);
	}

	Core_ptr reference_core = new SquareCore(NULL);
	reference_core->SetPosition(Vector3f(10,0,0));
	sort(cores.begin(), cores.end(), RelativeRangeSort<Core_ptr, Core_ptr>(reference_core));
	//Should be sorted short to long
	for(int i = 0; i < 19; i++)
	{
		float shorter = (cores[i]->GetGlobalPosition() - reference_core->GetGlobalPosition()).length();
		float longer = (cores[i+1]->GetGlobalPosition() - reference_core->GetGlobalPosition()).length();
		CHECK(shorter <= longer);
	}


}