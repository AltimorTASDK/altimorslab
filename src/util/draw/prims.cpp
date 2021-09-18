#include "util/draw/prims.h"
#include "os/gx.h"
#include "os/os.h"
#include "util/misc.h"
#include "util/vector.h"
#include <ogc/gx.h>
#include <concepts>
#include <vector>

static void write_vector(const auto &vector)
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

template<vector_type T>
static auto vector_min_max(const T &a, const T &b)
{
	const auto min = vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	const auto max = vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	return std::make_tuple(min, max);
}

template<vertex_format T>
void draw_rect(const T &a, const T &b, align alignment)
{
	const auto [min, max] = vector_min_max(a.position, b.position);
}