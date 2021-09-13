#include "os/os.h"
#include "util/hash.h"
#include "util/vector.h"
#include <vector>

extern "C" void orig_StartMelee(void *param_1);
extern "C" void hook_StartMelee(void *param_1)
{
	/*std::vector<int> test;
	test.push_back(1);
	test.push_back(5);

	for (auto i : test)
		OSReport("i: %d\n", i);

	OSReport("hash: %08X\n", strhash("a"));*/

	vec3 test1;
	OSReport("y: %f\n", test1.y);
	OSReport("&y: %p\n", &test1.y);

	vec3 test2(0, 1, 2);
	OSReport("y: %f\n", test2.y);
	OSReport("&y: %p\n", &test2.y);

	test2 = test1;
	test2.to_tuple();

	static_assert(vec3(1.f, 1.f, 0).dot(vec3(.5f, .5f, 0)) == 1.f);

	orig_StartMelee(param_1);
}

extern "C" void orig_Scene_Main();
extern "C" void hook_Scene_Main()
{
	// Run C++ constructors after OS/library initialization
	using ctor_t = void(*)();
	extern ctor_t ctors_base;
	extern ctor_t ctors_end;

	for (auto *ctor = &ctors_base; ctor != &ctors_end; ctor++)
		(*ctor)();

	orig_Scene_Main();
}