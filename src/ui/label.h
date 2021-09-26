#pragma once

#include "ui/element.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/font_renderer.h"

namespace ui {

class label : public element {
	const font_renderer *font = nullptr;
	std::string text;
	color_rgba color = color_rgba::white;

public:
	template<typename elem_type>
	class builder_impl : public element::builder_impl<elem_type> {
	protected:
		using builder_type = element::builder_impl<elem_type>::builder_type;
		using element::builder_impl<elem_type>::instance;
		using element::builder_impl<elem_type>::builder_ref;

	public:
		builder_type &set_font(const font_renderer &font)
		{
			instance->font = &font;
			return builder_ref();
		}

		builder_type &set_text(const std::string &text)
		{
			instance->text = text;
			return builder_ref();
		}

		builder_type &set_color(const color_rgba &color)
		{
			instance->color = color;
			return builder_ref();
		}
	};

	using builder = builder_impl<label>;
	
	vec2 get_size() const override
	{
		return vec2(font->measure(text));
	}

	void draw() const override
	{
		font->draw(text, get_position(), color);
	}
};

}