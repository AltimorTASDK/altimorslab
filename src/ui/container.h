#pragma once

#include "ui/element.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include <concepts>
#include <memory>
#include <vector>

namespace ui {
	
enum class stack {
	vertical, // div style
	horizontal // span style
};

class container : public element {
	std::vector<std::unique_ptr<element>> children;

protected:
	vec2 padding;
	vec2 size;
	stack stacking;

public:
	template<typename elem_type>
	class builder_impl : public element::builder_impl<elem_type> {
	protected:
		using builder_type = element::builder_impl<elem_type>::builder_type;
		using element::builder_impl<elem_type>::instance;
		using element::builder_impl<elem_type>::builder_ref;

	public:
		builder_type &set_size(const vec2 &size)
		{
			instance->size = size;
			return builder_ref();
		}

		builder_type &set_padding(const vec2 &padding)
		{
			instance->padding = padding;
			return builder_ref();
		}

		builder_type &set_stacking(stack stacking)
		{
			instance->stacking = stacking;
			return builder_ref();
		}

		template<std::convertible_to<element> T>
		builder_type &add_child(std::unique_ptr<T> &&child)
		{
			instance->add_child(std::move(child));
			return builder_ref();
		}
	};

	using builder = builder_impl<container>;
	
	template<std::convertible_to<element> T>
	void add_child(std::unique_ptr<T> &&child)
	{
		child->parent = this;
		children.push_back(std::move(child));
	}

	vec2 get_size() const override
	{
		return size;
	}

	void update_child_offsets_vertical() const
	{
		auto min = padding;
		auto max = size - padding;

		for (const auto &child : children) {
			if (child->placement != place::normal)
				continue;

			child->offset = vec3(min);
			min.y += child->get_size().y + padding.y;
		}

		for (const auto &child : children) {
			if (child->placement != place::reverse)
				continue;

			max.y -= child->get_size().y;
			if (max.y < min.y)
				max.y = min.y;
			
			child->offset = vec3(min.x, max.y, 0);
			max.y -= padding.y;
		}
	}

	void update_child_offsets_horizontal() const
	{
		auto min = padding;
		auto max = size - padding;

		for (const auto &child : children) {
			if (child->placement != place::normal)
				continue;

			child->offset = vec3(min);
			min.x += child->get_size().x + padding.x;
		}

		for (const auto &child : children) {
			if (child->placement != place::reverse)
				continue;

			max.x -= child->get_size().x;
			if (max.x < min.x)
				max.x = min.x;
			
			child->offset = vec3(max.x, min.y, 0);
			max.x -= padding.x;
		}
	}
	
	void update_child_offsets() const
	{
		switch (stacking) {
		case stack::vertical:
			update_child_offsets_vertical();
			break;
		case stack::horizontal:
			update_child_offsets_horizontal();
			break;
		}
	}

	void draw() const override
	{
		update_child_offsets();
		
		auto &rs = render_state::get();
		rs.push_scissor();
		rs.restrict_scissor(vec2i(get_position()), vec2i(size));
		
		for (const auto &child : children)
			child->draw();
			
		rs.pop_scissor();
	}
};

}