#include "os/os.h"
#include "hsd/cobj.h"
#include "util/hash.h"
#include "util/matrix.h"
#include "util/vector.h"
#include "util/gc/file.h"
#include "util/draw/font.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include <ogc/gx.h>
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
		
font font_small("fonts/font_small.tex", { 16, 32 }, { 9, 14 });
font font_big("fonts/font_big.tex", { 32, 64 }, { 18, 28 });
texture test("test.tex");
texture pane("ui/pane.tex");

extern "C" void HSD_StateInitDirect(u32 format, u32 render_mode);
extern "C" void C_MTXOrtho(float, float, float, float, float, float, Mtx44);

extern "C" void orig_DevelopText_DrawAll(struct HSD_GObj *gobj, u32 pass);
extern "C" void hook_DevelopText_DrawAll(struct HSD_GObj *gobj, u32 pass)
{
	orig_DevelopText_DrawAll(gobj, pass);

	if (pass != 2)
		return;
		
	auto &rs = render_state::get();
	rs.reset();

	rs.fill_tiled_rect(vec3(100, 100, 0), vec2(100, 50), color_rgba(255, 255, 255, 255), pane);
	rs.fill_tiled_rect(vec3(200, 200, 0), vec2(16, 16), color_rgba(255, 255, 255, 255), pane);

	/*rs.fill_rect(vec3(0, 0, 0), vec2(640, 480), color_rgba(255, 255, 255, 128), test,
	             uv_coord(0, 0), uv_coord(1, 1));
	
	font_big.draw("\x03\x0B\x03\x0B\x03GAY PRIDE WORLDWIDE\x03\x0C\x03\x0C\x03", { 640/2, 100, 0 }, align::center);

	font_small.draw("Custom font rendering w/ texture atlas", { 640/2, 480/2 - 9, 0 }, align::center);
	font_small.draw("Big pp mode engaged", { 640/2, 480/2 + 9, 0 }, align::center);*/
}

struct asdf {
	asdf()
	{
		const auto proj = ortho_projection(0, 480, 0, 640, 0, 2);

		for (auto i = 0; i < 4; i++) {
			for (auto j = 0; j < 4; j++) 
				OSReport("%.4f ", proj.get(i, j));

			OSReport("\n");
		}
	}
} asdf;