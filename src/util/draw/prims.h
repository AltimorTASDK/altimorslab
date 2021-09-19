#pragma once

#include "util/vector.h"
#include "os/gx.h"
#include <ogc/gx.h>

enum class align {
	top_left,
	top,
	top_right,
	left,
	center,
	right,
	bottom_left,
	bottom,
	bottom_right
};

struct vertex {
	void write() const;
};

struct vertex_pos_clr : vertex {
	vec3 position;
	color_rgba color;
	
	vertex_pos_clr(vec3 position, color_rgba color) : position(position), color(color) {}
	
	static void set_format();
	void write() const;
};

struct vertex_pos_uv : vertex {
	vec3 position;
	uv_coord uv;
	
	vertex_pos_uv(vec3 position, uv_coord uv) : position(position), uv(uv) {}
	
	static void set_format();
	void write() const;
};

template<typename T>
concept vertex_format = std::is_base_of_v<vertex, T>;

template<vertex_format T>
void draw_primitive(u32 prim_type, const std::vector<T> &vertices)
{
	GX_Begin(prim_type, GX_VTXFMT0, vertices.size());
	for (const auto &vertex : vertices)
		vertex.write();
}

template<vertex_format T>
void draw_quads(const std::vector<T> &vertices)
{
	draw_primitive(GX_QUADS, vertices);
}

void draw_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
               align alignment = align::top_left);

void draw_rect(const vec3 &origin, const vec2 &size, const uv_coord &uv1, const uv_coord &uv2,
               align alignment = align::top_left);