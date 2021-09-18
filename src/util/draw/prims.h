#pragma once

#include "util/vector.h"
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
	
	static void set_format();
	void write() const;
};

struct vertex_pos_uv : vertex {
	vec3 position;
	vec2 uv;
	
	static void set_format();
	void write() const;
};

template<typename T>
concept vertex_format = std::is_base_of_v<vertex, T>;

template<vertex_format T>
void draw_primitive(u32 prim_type, const std::vector<T> &vertices)
{
	GXBegin(prim_type, GX_VTXFMT0, vertices.size());
	for (const auto &vertex : vertices)
		vertex.write();
}

template<vertex_format T>
void draw_quads(const std::vector<T> &vertices)
{
	draw_primitive(GX_QUADS, vertices);
}

template<vertex_format T>
void draw_rect(const T &a, const T &b, align alignment = align::top_left);