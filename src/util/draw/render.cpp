#include "util/draw/render.h"
#include "os/gx.h"
#include "os/os.h"
#include "util/matrix.h"
#include "util/misc.h"
#include "util/vector.h"
#include "util/draw/texture.h"
#include <ogc/gx.h>
#include <concepts>
#include <vector>

render_state render_state::instance;

static void write_vector(const vector_type auto &vector)
{
	std::apply([](auto ...values) { gx_fifo->write(values...); }, vector.elems());
}

void vertex_pos_clr::write() const
{
	write_vector(position);
	write_vector(color);
}

void vertex_pos_uv::write() const
{
	write_vector(position);
	write_vector(uv);
}

void vertex_pos_clr_uv::write() const
{
	write_vector(position);
	write_vector(color);
	write_vector(uv);
}

void vertex_pos_clr::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
}

void vertex_pos_clr_uv::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

void vertex_pos_uv::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

vec3 alignment_offset(const vec2 &size, align alignment)
{
	switch (alignment) {
	case align::top_left:
		return vec3::zero;
	case align::top:
		return -vec3(size.x / 2, 0, 0);
	case align::top_right:
		return -vec3(size.x, 0, 0);
	case align::left:
		return -vec3(0, size.y / 2, 0);
	case align::center:
		return -vec3(size.x / 2, size.y / 2, 0);
	case align::right:
		return -vec3(size.x, size.y / 2, 0);
	case align::bottom_left:
		return -vec3(0, size.y, 0);
	case align::bottom:
		return -vec3(size.x / 2, size.y, 0);
	case align::bottom_right:
		return -vec3(size.x, size.y, 0);
	}

	PANIC("Invalid align constant");
}

void render_state::reset()
{
	current_vertex_fmt = -1;
	current_tex_obj = nullptr;

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
	
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
	
	GX_SetCullMode(GX_NONE);

	constexpr auto proj = ortho_projection(0, 480, 0, 640, 0, 2);
	GX_SetCurrentMtx(0);
	GX_LoadProjectionMtx(proj.as_multidimensional(), GX_ORTHOGRAPHIC);
	GX_LoadPosMtxImm(matrix3x4::identity.as_multidimensional(), GX_PNMTX0);
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
                             align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);
	
	render_state::draw_quads<vertex_pos_clr>({
		{ aligned,                           color },
		{ aligned + vec3(size.x, 0,      0), color },
		{ aligned + vec3(size.x, size.y, 0), color },
		{ aligned + vec3(0,      size.y, 0), color }
	});
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const texture &tex,
                             const uv_coord &uv1, const uv_coord &uv2, align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);
	
	tex.apply();
	
	render_state::draw_quads<vertex_pos_uv>({
		{ aligned,                           uv1                    },
		{ aligned + vec3(size.x, 0,      0), uv_coord(uv2.u, uv1.v) },
		{ aligned + vec3(size.x, size.y, 0), uv2                    },
		{ aligned + vec3(0,      size.y, 0), uv_coord(uv1.u, uv2.v) }
	});
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
                             const texture &tex, const uv_coord &uv1, const uv_coord &uv2,
                             align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);
	
	tex.apply();
	
	render_state::draw_quads<vertex_pos_clr_uv>({
		{ aligned,                           color, uv1                    },
		{ aligned + vec3(size.x, 0,      0), color, uv_coord(uv2.u, uv1.v) },
		{ aligned + vec3(size.x, size.y, 0), color, uv2                    },
		{ aligned + vec3(0,      size.y, 0), color, uv_coord(uv1.u, uv2.v) }
	});
}