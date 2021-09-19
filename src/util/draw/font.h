#include "util/vector.h"
#include "util/draw/prims.h"
#include "util/draw/texture.h"
#include <cmath>

class font {
	const texture tex;

	const vec2i cell_size;
	const vec2i cell_count;
	const vec2i char_size;

	// Offset from the top left of the cell to the center of the char
	const vec3 cell_padding;
	
	std::tuple<uv_coord, uv_coord> get_char_uv(char c) const
	{
		const auto cell_x = c % cell_count.x;
		const auto cell_y = c / cell_count.x;
		const auto scale = uv_coord(1.f / cell_count.x, 1.f / cell_count.y);
		return std::make_tuple(
			uv_coord(cell_x, cell_y) * scale,
			uv_coord(cell_x + 1, cell_y + 1) * scale);
	}

public:
	font(const std::string &path, vec2i cell_size, vec2i char_size) :
		tex(path),
		cell_size(cell_size),
		cell_count((float)tex.width() / cell_size.x, (float)tex.height() / cell_size.y),
		char_size(char_size),
		cell_padding(vec3(cell_size - char_size) / 2)
	{
	}

	vec2i measure(const std::string &text) const
	{
		return { char_size.x * text.size(), char_size.y };
	}
	
	void draw(const std::string &text, const vec3 &origin,
	          align alignment = align::top_left) const
	{
		auto pos = origin - cell_padding + alignment_offset(vec2(measure(text)), alignment);
		
		tex.apply();
		
		for (auto c : text) {
			const auto [uv1, uv2] = get_char_uv(c);
			draw_rect(pos, vec2(cell_size), uv1, uv2);
			pos.x += char_size.x;
		}
	}
};