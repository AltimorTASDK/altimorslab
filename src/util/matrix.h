#include <cstddef>

template<typename T, size_t N, size_t M>
class matrix {
	T m[N][M];
	
public:
	T operator[](size_t index) const
	{
		return m[index];
	}
};

using matrix4x3 = matrix<float, 4, 3>;