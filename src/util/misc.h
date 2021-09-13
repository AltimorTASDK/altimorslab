
constexpr auto align(auto value, auto alignment)
{
	// Powers of 2 only
	return (value + alignment - 1) & ~(alignment - 1);
}