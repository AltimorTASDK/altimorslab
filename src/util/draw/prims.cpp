#include "util/draw/prims.h"
#include "os/gx.h"
#include "util/misc.h"
#include "util/vector.h"
#include <ogc/gx.h>
#include <concepts>

void write_vector(const auto &vector)
{
	std::apply([](auto ...values) { gx_fifo->write(values...); }, vector.elems());
}

void write_vertex(const vec3 &position, const color_rgba &color)
{
	write_vector(position);
	write_vector(color);
}

void write_vertices(const color_rgba &color, const auto &...vertices)
{
	(write_vertex(vertices, color), ...);
}

void draw_quad(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &d, const color_rgba &color)
{
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	write_vertices(color, a, b, c, d);
}