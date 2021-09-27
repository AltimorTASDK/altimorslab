#pragma once

#include "ui/measurement.h"
#include "util/vector.h"

namespace ui {
	
class element {
public:
	measurement2 offset;
	measurement2 size = measurement2(fit, fit);
	element *parent = nullptr;

	template<typename elem_type>
	class builder_impl {
	protected:
		std::unique_ptr<elem_type> instance = std::make_unique<elem_type>();
		using builder_type = elem_type::template builder_impl<elem_type>;
		
		auto &builder_ref()
		{
			return static_cast<builder_type&>(*this);
		}
		
	public:
		std::unique_ptr<elem_type> build()
		{
			return std::move(instance);
		}

		auto &set_offset(const measurement &x, const measurement &y)
		{
			instance->offset = measurement2(x, y);
			return builder_ref();
		}

		builder_type &set_size(const measurement &x, const measurement &y)
		{
			instance->size = measurement2(x, y);
			return builder_ref();
		}
	};
	
	using builder = builder_impl<element>;

	vec3 get_position() const
	{
		const auto cached = vec2(offset.x.get_cached_value(), offset.y.get_cached_value());
		if (parent != nullptr)
			return parent->get_position() + vec3(cached);
		else
			return vec3(cached);
	}

	vec2 get_size() const
	{
		auto result = vec2(size.x.get_cached_value(), size.y.get_cached_value());

		if (size.x.units == unit::fit || size.y.units == unit::fit) {
			const auto fit_size = get_fit_size();
			if (size.x.units == unit::fit)
				result.x = fit_size.x;
			if (size.y.units == unit::fit)
				result.y = fit_size.y;
		}
		
		return result;
	}

	// Calculate the size used by unit::fit
	virtual vec2 get_fit_size() const
	{
		return vec2::zero;
	}

	// Update positions and sizes
	virtual void update() const
	{
	}

	virtual void draw() const
	{
	}
};

}