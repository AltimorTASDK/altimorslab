#pragma once

#include "ui/element.h"
#include "util/vector.h"

namespace ui {
	
enum class unit {
	pixels,
	percent,
	// Automatically size to contents
	fit,
	// Consume all remaining space in the parent container. If a
	// container has multiple fill elements, space will be evenly
	// distributed between them.
	fill
};
	
class measurement {
	friend class container;
	
public:
	unit units;
	float value;
	
private:
	// Evaluated value in pixels, may be set by parent container
	float cached_value;
	
public:
	constexpr measurement() : units(unit::pixels)
	{
	}

	constexpr measurement(float value) :
		units(unit::pixels), value(value), cached_value(value)
	{
	}

	constexpr measurement(unit units, float value=0.f) :
		units(units), value(value), cached_value(0.f)
	{
		if (units == unit::pixels)
			cached_value = value;
	}
	
	float get_cached_value() const
	{
		return cached_value;
	}
};

using measurement2 = vec_impl<vec2_base<measurement>>;

inline constexpr auto fit = measurement(unit::fit);
inline constexpr auto fill = measurement(unit::fill);

namespace literals {
	measurement operator""_px(long double value)
	{
		return measurement(unit::pixels, value);
	}

	measurement operator""_px(unsigned long long value)
	{
		return measurement(unit::pixels, (unsigned int)value);
	}

	measurement operator""_pct(long double value)
	{
		return measurement(unit::percent, value / 100);
	}

	measurement operator""_pct(unsigned long long value)
	{
		return measurement(unit::percent, (float)(unsigned int)value / 100);
	}
}
	
}