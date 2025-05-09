#ifndef MATH_H
#define MATH_H
#include <cmath>
#include <vector>

namespace math {

template <typename T>
std::vector<std::vector<T>>
matrix_transpose(const std::vector<std::vector<T>> &matrix);

template <typename T>
std::vector<std::vector<T>>
cholesky_decomposition(const std::vector<std::vector<T>> &matrix);

} // namespace math

#endif // MATH_H
