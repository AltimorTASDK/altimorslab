#include "util/draw/prims.h"
#include "os/gx.h"
#include "os/os.h"
#include "util/misc.h"
#include "util/vector.h"
#include <ogc/gx.h>
#include <concepts>
#include <vector>

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

void vertex_pos_clr::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
}

void vertex_pos_uv::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

static vec3 alignment_offset(const vec2 &size, align alignment)
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

void draw_rect(const vec3 &origin, const vec2 &size, const color_rgba &color, align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);
	
	draw_quads<vertex_pos_clr>({
		{ aligned,                           color },
		{ aligned + vec3(size.x, 0,      0), color },
		{ aligned + vec3(size.x, size.y, 0), color },
		{ aligned + vec3(0,      size.y, 0), color }
	});
}

void draw_rect(const vec3 &origin, const vec2 &size, const uv_coord &uv1, const uv_coord &uv2,
               align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);
	
	draw_quads<vertex_pos_uv>({
		{ aligned,                           uv1                    },
		{ aligned + vec3(size.x, 0,      0), uv_coord(uv2.u, uv1.v) },
		{ aligned + vec3(size.x, size.y, 0), uv2                    },
		{ aligned + vec3(0,      size.y, 0), uv_coord(uv1.u, uv2.v) }
	});
}