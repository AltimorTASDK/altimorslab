#pragma once

#include "ui/element.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/font_renderer.h"

namespace ui {

class label : public element {
	using base_class = element;

	const font_renderer &font = fonts::small;
	std::string text;
	color_rgba color = color_rgba::white;

public:
	template<typename elem_type>
	class builder_impl : public base_class::builder_impl<elem_type> {
	protected:
		using builder_type = base_class::builder_impl<elem_type>::builder_type;
		using base_class::builder_impl<elem_type>::instance;
		using base_class::builder_impl<elem_type>::builder_ref;

	public:
		builder_type &set_font(const font_renderer &font)
		{
			instance->font = font;
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
	
	vec2 get_fit_size() const override
	{
		return vec2(font.measure(text));
	}

	void draw() const override
	{
		font.draw(text, get_position(), color);
	}
};

}