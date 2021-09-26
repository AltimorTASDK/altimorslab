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

extern "C" void HSD_StateInitDirect(u32 format, u32 render_mode);
extern "C" void C_MTXOrtho(float, float, float, float, float, float, Mtx44);

extern "C" void orig_DevelopText_DrawAll(struct HSD_GObj *gobj, u32 pass);
extern "C" void hook_DevelopText_DrawAll(struct HSD_GObj *gobj, u32 pass)
{
	orig_DevelopText_DrawAll(gobj, pass);

	if (pass != 2)
		return;
		
	/*font_small.apply();
	vertex_pos_uv::set_format();
	draw_rect(
		{ 0, 0, 0 },
		{ 100, 100 },
		{ 0.f, 0.f }, { 1.f, 1.f },
		align::top_left);*/

	//HSD_StateInitDirect(GX_VTXFMT0, 2);

	/*draw_rect(
		{ 0, 0, 0 },
		{ 100, 100 },
		{ 255, 255, 255, 255 },
		align::top_left);*/
	
	/*GX_SetNumTexGens(1);
	GX_SetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GX_SetNumTevStages(1);
	GX_SetTevOrder(0, 0, 0, 4);
	GX_SetTevOp(0, 4);
	GX_SetZTexture(2, 0x11, 0);
	GX_SetNumChans(1);*/
	
	/*GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
	
	GX_SetNumTevStages(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE,
	               GX_AF_NONE);

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE,
	                   GX_DTTIDENTITY);

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
	
	constexpr auto proj = ortho_projection(0, 480, 0, 640, 0, 2);
	GX_SetCurrentMtx(0);
	GX_LoadProjectionMtx(proj.as_multidimensional(), GX_ORTHOGRAPHIC);*/

	/*GX_ClearVtxDesc();
	vertex_pos_clr_uv::set_format();
	vertex_pos_uv::set_format();*/
	auto &rs = render_state::get();
	rs.reset();

	rs.fill_rect(vec3(0, 0, 0), vec2(640, 480), color_rgba(255, 255, 255, 128), test,
	             uv_coord(0, 0), uv_coord(1, 1));
	
	font_big.draw("\x03\x0B\x03\x0B\x03GAY PRIDE WORLDWIDE\x03\x0C\x03\x0C\x03", { 640/2, 100, 0 }, align::center);

	font_small.draw("Custom font rendering w/ texture atlas", { 640/2, 480/2 - 9, 0 }, align::center);
	font_small.draw("Big pp mode engaged", { 640/2, 480/2 + 9, 0 }, align::center);
		
	/*draw_primitive<vertex_pos_clr>(GX_LINES, {
		{
			{ 0, 0, 0 },
			{ 255, 255, 255, 255 }
		},
		{
			{ 100, 100, 0 },
			{ 255, 255, 255, 255 }
		}
	});*/
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