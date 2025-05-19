#ifndef MATH_H
#define MATH_H
#include <climits>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <stdexcept>
#include <vector>

template <typename T> constexpr T EPSILON = static_cast<T>(1e-12);
namespace math {

template <typename T>
std::vector<std::vector<T>>
matrix_transpose(const std::vector<std::vector<T>> &matrix) {
    // O(n^2)
    // Transpose a matrix
    size_t rows = matrix.size();
    size_t cols = matrix.at(0).size();

    std::vector<std::vector<T>> transposed(cols, std::vector<T>(rows, 0));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            transposed.at(j).at(i) = matrix.at(i).at(j);
        }
    }
    return transposed;
}

template <typename T>
int cholesky_decomposition(const std::vector<std::vector<T>> &matrix,
                           std::vector<std::vector<T>> &L) {
    // O(n^3 / 3)
    // Works only for symmetric positive-definite matrices
    size_t n = matrix.size();
    if (n == 0 || matrix.at(0).size() != n) {
        throw std::invalid_argument("Matrix must be square and non-empty!");
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            T sum = 0;
            for (size_t k = 0; k < j; ++k) {
                sum += L.at(i).at(k) * L.at(j).at(k);
            }
            if (i == j) {
                if (matrix.at(i).at(i) - sum <= 0) {
                    throw std::runtime_error(
                        "Matrix is not positive definite!");
                    return -1;
                }
                L.at(i).at(j) = std::sqrt(matrix.at(i).at(i) - sum);
            } else {
                L.at(i).at(j) = (matrix.at(i).at(j) - sum) / L.at(j).at(j);
            }
        }
    }
    return 0;
}
template <typename T> double vector_norm(const std::vector<T> &v) {
    double sum = 0;
    for (const auto &val : v) {
        sum += val * val;
    }
    return std::sqrt(sum);
}

template <typename T> std::vector<T> normalized(const std::vector<T> &v) {
    std::vector<T> normalized(v.size());
    double norm = vector_norm(v);
    if (norm < EPSILON<T>) {
        throw std::invalid_argument("Cannot normalize a zero vector!");
        return {};
    }
    for (size_t i = 0; i < v.size(); ++i) {
        normalized.at(i) = v.at(i) / norm;
    }
    return normalized;
}

template <typename T>
std::vector<T> scalar_multiply(const std::vector<T> &v, T scalar) {
    std::vector<T> result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result.at(i) = v.at(i) * scalar;
    }
    return result;
}

template <typename T>
std::vector<std::vector<T>>
matrix_scalar_multiply(const std::vector<std::vector<T>> &matrix, T scalar) {
    std::vector<std::vector<T>> result(matrix.size(),
                                       std::vector<T>(matrix.at(0).size(), 0));
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix.at(0).size(); ++j) {
            result.at(i).at(j) = matrix.at(i).at(j) * scalar;
        }
    }
    return result;
}

template <typename T>
std::vector<T> vector_add(const std::vector<T> &v1, const std::vector<T> &v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size!");
        return {};
    }
    std::vector<T> result(v1.size());
    for (size_t i = 0; i < v1.size(); ++i) {
        result.at(i) = v1.at(i) + v2.at(i);
    }
    return result;
}

template <typename T>
std::vector<T> vector_subtract(const std::vector<T> &v1,
                               const std::vector<T> &v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size!");
        return {};
    }
    std::vector<T> result(v1.size());
    for (size_t i = 0; i < v1.size(); ++i) {
        result.at(i) = v1.at(i) - v2.at(i);
    }
    return result;
}

template <typename T>
std::vector<T> vector_multiply(const std::vector<T> &v1,
                               const std::vector<T> &v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size!");
        return {};
    }
    std::vector<T> result(v1.size());
    for (size_t i = 0; i < v1.size(); ++i) {
        result.at(i) = v1.at(i) * v2.at(i);
    }
    return result;
}
template <typename T>
std::vector<std::vector<T>>
matrix_multiply(const std::vector<std::vector<T>> &A,
                const std::vector<std::vector<T>> &B) {
    size_t n = A.size();
    size_t m = A.at(0).size();
    size_t p = B.at(0).size();

    if (m != B.size()) {
        throw std::invalid_argument("Matrix dimensions do not match!");
        return {};
    }

    std::vector<std::vector<T>> result(n, std::vector<T>(p, 0));

#pragma omp parallel for collapse(2) schedule(static)
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < p; ++j) {
            T temp = 0;
#pragma omp simd reduction(+ : temp)
            for (size_t k = 0; k < m; ++k) {
                temp += A.at(i).at(k) * B.at(k).at(j);
            }
            result.at(i).at(j) = temp;
        }
    }
    return result;
}
template <typename T>
std::vector<std::vector<T>>
matrix_subtract(const std::vector<std::vector<T>> &A,
                const std::vector<std::vector<T>> &B) {
    size_t n = A.size();
    size_t m = A.at(0).size();

    if (n != B.size() || m != B.at(0).size()) {
        throw std::invalid_argument("Matrix dimensions do not match!");
        return {};
    }

    std::vector<std::vector<T>> result(n, std::vector<T>(m, 0));
#pragma omp parallel for collapse(2) schedule(static)
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            result.at(i).at(j) = A.at(i).at(j) - B.at(i).at(j);
        }
    }
    return result;
}

template <typename T>
int QR_decomposition(const std::vector<std::vector<T>> &matrix,
                     std::vector<std::vector<T>> &Q,
                     std::vector<std::vector<T>> &R) {

    std::vector<std::vector<T>> A = matrix;
    size_t n = A.size();
    size_t m = A.at(0).size();
    if (n == 0 || m == 0 || n != m) {
        throw std::invalid_argument("Matrix must be square and non-empty!");
        return -1;
    }

    std::vector<std::vector<std::vector<T>>> P(
        n - 1, std::vector<std::vector<T>>(n, std::vector<T>(n, 0)));

    for (size_t k = 0; k < n; ++k) {
        std::vector<T> a1(n - k, 0);
        std::vector<T> b1(n - k, 0);
        for (size_t i = k; i < n; ++i) {
            a1.at(i - k) = A.at(i).at(k);
            b1.at(i - k) = static_cast<T>(0.0);
        }
        double norm = vector_norm(a1);
        int sign = (a1.at(0) < static_cast<T>(0.0)) ? 1 : -1;

        std::vector<T> u =
            vector_subtract(a1, scalar_multiply(b1, norm * sign));

        // Normalize u
        std::vector<T> uNorm = normalized(u);

        std::cout << uNorm.size() << std::endl;
        for (auto &val : uNorm) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        std::vector<std::vector<T>> uTemp(n - k, std::vector<T>(1, 0));
        for (size_t i = 0; i < n - k; ++i) {
            uTemp.at(i).at(0) = uNorm.at(i);
        }
        std::vector<std::vector<T>> v = matrix_transpose(uTemp);

        std::vector<std::vector<T>> I(n - k, std::vector<T>(n - k, 0));
        for (size_t i = 0; i < n - k; ++i) {
            I.at(i).at(i) = 1;
        }

        std::vector<std::vector<T>> PTemp = matrix_subtract(
            I, matrix_scalar_multiply(matrix_multiply(v, matrix_transpose(v)),
                                      static_cast<T>(2.0)));

        std::vector<std::vector<T>> currentP(n, std::vector<T>(n, 0));
        for (size_t i = k; i < n; ++i) {
            currentP.at(i).at(i) = 1;
            for (size_t j = k; j < n; ++j) {
                currentP.at(i).at(j) = PTemp.at(i - k).at(j - k);
            }
        }
        P.at(k) = currentP;
        A = matrix_multiply(currentP, A);

        // Compute Q and R
        Q = P.at(0);
        for (size_t i = 1; i <= k; ++i) {
            Q = matrix_multiply(Q, matrix_transpose(P.at(i)));
        }

        R = currentP;
        for (size_t i = P.size() - 2; i >= 0; --i) {
            R = matrix_multiply(R, P.at(i));
        }
        R = matrix_multiply(R, A);
    }
    return 0;
}

template <typename T>
int eigen_pairs(std::vector<std::vector<T>> &A,
                std::vector<std::pair<T, std::vector<T>>> &result) {

    size_t n = A.size();

    return 0;
}
} // namespace math

#endif // MATH_H
