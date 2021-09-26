#pragma once

#include "util/vector.h"

namespace ui {
	
enum class place {
	normal, // Place at beginning of available container space
	reverse // Place at end of available container space
};

class element {
protected:
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

		auto &set_offset(vec3 offset)
		{
			instance->offset = offset;
			return builder_ref();
		}

		auto &set_placement(place placement)
		{
			instance->placement = placement;
			return builder_ref();
		}
	};

public:
	vec3 offset;
	place placement = place::normal;
	element *parent = nullptr;

	vec3 get_position() const
	{
		if (parent != nullptr)
			return parent->get_position() + offset;
		else
			return offset;
	}

	virtual vec2 get_size() const;
	virtual void draw() const;
};

}