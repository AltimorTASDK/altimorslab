#include "os/os.h"
#include "hsd/cobj.h"
#include "util/hash.h"
#include "util/vector.h"
#include "util/gc/file.h"
#include "util/draw/prims.h"
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
		
//texture font_small("fonts/font_small.tex");
texture test("test.tex");

extern "C" void HSD_StateInitDirect(u32 format, u32 render_mode);

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
	HSD_StateInitDirect(GX_VTXFMT0, 2);
	Mtx mtx;
	auto *cobj = HSD_CObjGetCurrent();
	HSD_CObjGetViewingMtx(cobj, &mtx);
	GX_LoadPosMtxImm(mtx, GX_PNMTX0);
	GX_SetCullMode(GX_CULL_NONE);

	/*draw_rect(
		{ 0, 0, 0 },
		{ 100, 100 },
		{ 255, 255, 255, 255 },
		align::top_left);*/
	
	test.apply();
	
	/*GX_SetNumTexGens(1);
	GX_SetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GX_SetNumTevStages(1);
	GX_SetTevOrder(0, 0, 0, 4);
	GX_SetTevOp(0, 4);
	GX_SetZTexture(2, 0x11, 0);
	GX_SetNumChans(1);*/
	
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_DTTIDENTITY);
	
	/*GXTexObj tex_obj;
	GX_InitTexObj(&tex_obj, (void*)
	"\xFF\x00\x00\xFF" "\x00\x00\xFF\xFF" "\x00\xFF\x00\xFF" "\xFF\x00\x00\x80"
	"\xFF\x00\x00\xFF" "\x00\x00\xFF\xFF" "\x00\xFF\x00\xFF" "\xFF\x00\x00\x80"
	"\xFF\x00\x00\xFF" "\x00\x00\xFF\xFF" "\x00\xFF\x00\xFF" "\xFF\x00\x00\x80"
	"\xFF\x00\x00\xFF" "\x00\x00\xFF\xFF" "\x00\xFF\x00\xFF" "\xFF\x00\x00\x80"
	, 4, 4, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_TRUE);
	GX_LoadTexObj(&tex_obj, GX_TEXMAP0);*/
		
	vertex_pos_uv::set_format();
	draw_rect(
		{ 0, 0, 0 },
		{ 100, 100 },
		{ 0.f, 0.f }, { 2.f, 2.f },
		align::top_left);
		
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

/*struct asdf {
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

		asset_file file("fonts/font_small.tex");
		OSReport("first word: %02X %02X %02X %02X\n",
			file.data[0],
			file.data[1],
			file.data[2],
			file.data[3]);
	}
} asdf;*/