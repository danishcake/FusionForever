/* Although the C++ standard requires constructors with side effects to be run it does not specify the object files linked.
To ensure automatic list adders are called they must be referenced in a header file included in the executable */
#pragma once

extern void RegisterSections();
