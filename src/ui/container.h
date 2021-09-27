#pragma once

#include "ui/element.h"
#include "ui/measurement.h"
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
	using base_class = element;

	std::vector<std::unique_ptr<element>> children;

protected:
	vec2 padding;
	float spacing;
	stack stacking;

public:
	template<typename elem_type>
	class builder_impl : public base_class::builder_impl<elem_type> {
	protected:
		using builder_type = base_class::builder_impl<elem_type>::builder_type;
		using base_class::builder_impl<elem_type>::instance;
		using base_class::builder_impl<elem_type>::builder_ref;

	public:
		builder_type &set_padding(float x, float y)
		{
			instance->padding = vec2(x, y);
			return builder_ref();
		}

		builder_type &set_spacing(float spacing)
		{
			instance->spacing = spacing;
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

private:
	float calc_offset(const measurement &offset) const
	{
		switch (offset.units) {
		case unit::percent: return get_size().x * offset.value;
		case unit::pixels:  return offset.value;
		default:            PANIC("Invalid measurement unit for offset");
		}
	}
	
	vec2 get_inner_size() const
	{
		return get_size() - padding * 2;
	}
	
	void update_child_offsets_vertical() const
	{
		auto offset = padding;

		for (const auto &child : children) {
			child->offset.x.cached_value = offset.x + calc_offset(child->offset.x);
			child->offset.y.cached_value = offset.y + calc_offset(child->offset.y);
			offset.y += child->get_size().y + spacing;
		}
	}

	void update_child_offsets_horizontal() const
	{
		auto offset = padding;

		for (const auto &child : children) {
			child->offset.x.cached_value = offset.x + calc_offset(child->offset.x);
			child->offset.y.cached_value = offset.y + calc_offset(child->offset.y);
			offset.x += child->get_size().x + spacing;
		}
	}
	
	void update_child_offsets() const
	{
		switch (stacking) {
		case stack::vertical:   return update_child_offsets_vertical();
		case stack::horizontal: return update_child_offsets_horizontal();
		default:                PANIC("Invalid stack constant");
		}
	}

	void update_child_sizes_vertical() const
	{
		auto fill_count = 0;

		for (const auto &child : children) {
			if (child->size.y.units == unit::fill)
				fill_count++;
		}
		
		if (fill_count == 0)
			return;
		
		const auto total_child_size = get_total_child_size();
		const auto remaining_space = get_inner_size().y - total_child_size.y;
		const auto fill_size = std::max(0.f, remaining_space / fill_count);
		
		for (const auto &child : children) {
			if (child->size.x.units == unit::fill)
				child->size.x.cached_value = get_inner_size().x;
			if (child->size.y.units == unit::fill)
				child->size.y.cached_value = fill_size;
		}
	}
	
	void update_child_sizes_horizontal() const
	{
		auto fill_count = 0;

		for (const auto &child : children) {
			if (child->size.x.units == unit::fill)
				fill_count++;
		}
		
		if (fill_count == 0)
			return;
		
		const auto total_child_size = get_total_child_size();
		const auto remaining_space = get_inner_size().x - total_child_size.x;
		const auto fill_size = std::max(0.f, remaining_space / fill_count);
		
		for (const auto &child : children) {
			if (child->size.x.units == unit::fill)
				child->size.x.cached_value = fill_size;
			if (child->size.y.units == unit::fill)
				child->size.y.cached_value = get_inner_size().y;
		}
	}
	
	void update_child_sizes() const
	{
		for (const auto &child : children) {
			if (child->size.x.units == unit::percent)
				child->size.x.cached_value = get_size().x * child->size.x.value;
			if (child->size.y.units == unit::percent)
				child->size.y.cached_value = get_size().y * child->size.y.value;
		}

		switch (stacking) {
		case stack::vertical:   return update_child_sizes_vertical();
		case stack::horizontal: return update_child_sizes_horizontal();
		default:                PANIC("Invalid stack constant");
		}
	}

	vec2 get_total_child_size_vertical() const
	{
		vec2 result;
		for (const auto &child : children) {
			if (child->size.x.units != unit::fill)
				result.x = std::max(result.x, child->get_size().x);

			if (child->size.y.units != unit::fill)
				result.y += child->get_size().y;
		}
		
		if (children.size() != 0)
			result.y += (children.size() - 1) * spacing;

		return result;
	}

	vec2 get_total_child_size_horizontal() const
	{
		vec2 result;
		for (const auto &child : children) {
			if (child->size.x.units != unit::fill)
				result.x += child->get_size().x;

			if (child->size.y.units != unit::fill)
				result.y = std::max(result.y, child->get_size().y);
		}
		
		if (children.size() != 0)
			result.x += (children.size() - 1) * spacing;

		return result;
	}
	
	vec2 get_total_child_size() const
	{
		switch (stacking) {
		case stack::vertical:   return get_total_child_size_vertical();
		case stack::horizontal: return get_total_child_size_horizontal();
		default:                PANIC("Invalid stack constant");
		}
	}
	
public:
	vec2 get_fit_size() const override
	{
		return get_total_child_size() + padding * 2;
	}

	void update() const override
	{
		update_child_sizes();
		update_child_offsets();

		for (const auto &child : children)
			child->update();
	}

	void draw() const override
	{
		for (const auto &child : children)
			child->draw();
	}
};

}