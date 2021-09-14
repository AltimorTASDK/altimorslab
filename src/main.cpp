#include "os/os.h"
#include "util/hash.h"
#include "util/vector.h"
#include "util/gc/file.h"
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

	orig_StartMelee(param_1);
}

extern "C" void orig_HSD_ResetScene();
extern "C" void hook_HSD_ResetScene()
{
	orig_HSD_ResetScene();

	// Run C++ constructors once after HSD_ResetScene, since only then will
	// extra heaps be initialized
	using ctor_t = void(*)();
	extern ctor_t ctors_base;
	extern ctor_t ctors_end;

	static bool once = false;
	if (!once) {
		for (auto *ctor = &ctors_base; ctor != &ctors_end; ctor++)
			(*ctor)();

		once = true;
	}
}

struct asdf {
	asdf()
	{
		vec3 test1;
		OSReport("y: %f\n", test1.y);
		OSReport("&y: %p\n", &test1.y);

		vec3 test2(0, 1, 2);
		OSReport("y: %f\n", test2.y);
		OSReport("&y: %p\n", &test2.y);

		test2 = test1;
		test2.elems();

		test2 += test1;

		static_assert(vec3::dot(vec3(1, 1, 0), vec3(.5f, .5f, 0)) == 1.f);
		static_assert(vec3::cross(vec3(1, 0, 0), vec3(0, 0, 1)) == vec3(0, -1, 0));
		static_assert(vec3i(1, 0, 0).x == 1);

		dvd_file file("EfFxData.dat");
	}
} asdf;