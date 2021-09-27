#pragma once

#include "ui/element.h"
#include "ui/container.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include <concepts>
#include <memory>
#include <vector>

namespace ui {

class pane : public container {
	using base_class = container;
	
	inline static const auto default_tex = texture("ui/pane.tex");
	const texture &tex = default_tex;
	
public:
	template<typename elem_type>
	class builder_impl : public base_class::builder_impl<elem_type> {
	protected:
		using builder_type = base_class::builder_impl<elem_type>::builder_type;
		using base_class::builder_impl<elem_type>::instance;
		using base_class::builder_impl<elem_type>::builder_ref;

	public:
		builder_type &set_texture(const texture &tex)
		{
			instance->tex = tex;
			return builder_ref();
		}
	};

	using builder = builder_impl<pane>;

	void draw() const override
	{
		auto &rs = render_state::get();
		rs.fill_tiled_rect(get_position(), get_size(), tex);

		rs.push_scissor();
		rs.restrict_scissor(vec2i(get_position()), vec2i(get_size()));

		container::draw();

		rs.pop_scissor();
	}
};

}