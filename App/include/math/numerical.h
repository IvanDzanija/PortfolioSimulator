#ifndef NUMERICAL_H
#define NUMERICAL_H
#include <chrono>
#include <climits>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <random>
#include <stdexcept>
#include <vector>

const size_t MAX_ITERATIONS = 1000;
template <typename T> constexpr T EPSILON = static_cast<T>(1e-6);
namespace math {

template <typename T>
std::vector<std::vector<T>>
matrix_transpose(const std::vector<std::vector<T>> &matrix) {
    // O(n^2)
    // Transpose a matrix
    size_t rows = matrix.size();
    size_t cols = matrix.at(0).size();

    std::vector<std::vector<T>> transposed(cols, std::vector<T>(rows));

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
    if (norm <= EPSILON<T>) {
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
                                       std::vector<T>(matrix.at(0).size()));
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
        std::cerr << "First matrix dimensions: " << n << 'x' << m << std::endl;
        std::cerr << "Second matrix dimensions: " << B.size() << 'x' << p
                  << std::endl;
        throw std::invalid_argument("Matrix dimensions do not match!");
        return {};
    }

    std::vector<std::vector<T>> result(n, std::vector<T>(p));

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

    std::vector<std::vector<T>> result(n, std::vector<T>(m));
#pragma omp parallel for collapse(2) schedule(static)
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            result.at(i).at(j) = A.at(i).at(j) - B.at(i).at(j);
        }
    }
    return result;
}

template <typename T> void vector_print(const std::vector<T> &v) {
    for (const auto &val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}
template <typename T>
void matrix_print(const std::vector<std::vector<T>> &matrix) {
    for (const auto &row : matrix) {
        for (const auto &val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}
template <typename T>
bool is_upper_triangular(const std::vector<std::vector<T>> &mat) {
    size_t rows = mat.size();
    for (size_t i = 1; i < rows; ++i) {
        for (size_t j = 0; j < i; ++j) {
            if (mat.at(i).at(j) > EPSILON<T>) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
bool is_lower_triangular(const std::vector<std::vector<T>> &mat) {
    size_t n = mat.size();

    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (mat.at(i).at(j) > EPSILON<T>) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
std::vector<std::vector<T>> vector_to_matrix_vert(const std::vector<T> &vec) {
    size_t n = vec.size();
    std::vector<std::vector<T>> ret(n, std::vector<T>(1));
    for (size_t i = 0; i < n; ++i) {
        ret.at(i).at(0) = vec.at(i);
    }
    return ret;
}
template <typename T>
std::vector<std::vector<T>> vector_to_matrix_horz(const std::vector<T> &vec) {
    size_t n = vec.size();
    std::vector<std::vector<T>> ret(1, std::vector<T>(n));
    for (size_t i = 0; i < n; ++i) {
        ret.at(0).at(i) = vec.at(i);
    }
    return ret;
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
        n - 1, std::vector<std::vector<T>>(n, std::vector<T>(n)));

    for (size_t k = 0; k < n - 1; ++k) {
        std::vector<T> a1(n - k);
        std::vector<T> b1(n - k, static_cast<T>(0.0));
        for (size_t i = k; i < n; ++i) {
            a1.at(i - k) = A.at(i).at(k);
        }
        b1.at(0) = static_cast<T>(1.0);

        double norm = vector_norm(a1);
        int sign = (a1.at(0) < static_cast<T>(0.0)) ? 1 : -1;

        std::vector<T> u =
            vector_subtract(a1, scalar_multiply(b1, norm * sign));

        std::vector<T> uNorm = normalized(u);

        std::vector<std::vector<T>> uTemp(n - k, std::vector<T>(1));
        for (size_t i = 0; i < n - k; ++i) {
            uTemp.at(i).at(0) = uNorm.at(i);
        }

        std::vector<std::vector<T>> v = matrix_transpose(uTemp);

        std::vector<std::vector<T>> I(n - k, std::vector<T>(n - k));
        for (size_t i = 0; i < n - k; ++i) {
            I.at(i).at(i) = 1;
        }

        std::vector<std::vector<T>> PTemp =
            matrix_subtract(I, matrix_scalar_multiply(matrix_multiply(uTemp, v),
                                                      static_cast<T>(2.0)));

        std::vector<std::vector<T>> currentP(n, std::vector<T>(n));
        for (size_t i = 0; i < n; ++i) {
            currentP.at(i).at(i) = 1;
        }
        for (size_t i = k; i < n; ++i) {
            for (size_t j = k; j < n; ++j) {
                currentP.at(i).at(j) = PTemp.at(i - k).at(j - k);
            }
        }
        P.at(k) = currentP;
        A = matrix_multiply(currentP, A);
    }

    // Compute Q and R
    Q = P.at(0);
    for (auto it = std::next(P.begin()); it != P.end(); it = std::next(it)) {
        Q = matrix_multiply(Q, matrix_transpose(*it));
    }

    R = *P.rbegin();
    for (auto it = std::next(P.rbegin()); it != P.rend(); it = std::next(it)) {
        R = matrix_multiply(R, *it);
    }
    R = matrix_multiply(R, matrix);
    return 0;
}

template <typename T> int normalize(std::vector<T> &v) {
    double norm = vector_norm(v);
    if (norm <= EPSILON<T>) {
        throw std::invalid_argument("Cannot normalize a zero vector!");
        return -1;
    }
    for (size_t i = 0; i < v.size(); ++i) {
        v.at(i) /= norm;
    }
    return 0;
}

template <typename T>
int matrix_inverse(const std::vector<std::vector<T>> &input,
                   std::vector<std::vector<T>> &output) {
    int n = input.size();

    // Check for square matrix
    if (n == 0 || input[0].size() != n) {
        return -1; // Not a square matrix
    }

    std::vector<std::vector<T>> A = input;
    std::vector<std::vector<T>> I(
        n, std::vector<T>(n, T(0))); // Use T instead of double

    // Initialize identity matrix
    for (int i = 0; i < n; ++i) {
        I[i][i] = T(1); // Use T(1) instead of 1.0
    }

    for (int i = 0; i < n; ++i) {
        // Partial pivoting
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                maxRow = k;
            }
        }

        // Check for singular matrix
        if (std::abs(A[maxRow][i]) < T(1e-14)) {
            // std::cerr << "Matrix is singular or nearly singular." <<
            // std::endl;
        }

        std::swap(A[i], A[maxRow]);
        std::swap(I[i], I[maxRow]);

        T diag = A[i][i];
#pragma omp parallel for
        for (int j = 0; j < n; ++j) {
            A[i][j] /= diag;
            I[i][j] /= diag;
        }

#pragma omp parallel for
        for (int k = 0; k < n; ++k) {
            if (k == i)
                continue;
            T factor = A[k][i];
            for (int j = 0; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
                I[k][j] -= factor * I[i][j];
            }
        }
    }

    output = I;
    return 0; // Success
}

template <typename T> int set_to_identity(std::vector<std::vector<T>> &matrix) {
    size_t n = matrix.size();
    if (n == 0) {
        std::cerr << "Matrix is empty!" << std::endl;
        return 1;
    }
    size_t m = matrix.at(0).size();
    if (n != m) {
        std::cerr << "Matrix is not square!" << std::endl;
        return 1;
    }
    for (size_t i = 0; i < n; ++i) {
        matrix.at(i).at(i) = 1;
    }
    return 0;
}

template <typename T>
std::vector<T> eigen_values(const std::vector<std::vector<T>> &matrix) {
    std::vector<std::vector<T>> A = matrix;
    size_t n = A.size();
    size_t m = A.at(0).size();
    if (n == 0 || m == 0 || n != m) {
        throw std::invalid_argument("Matrix must be square and non-empty!");
        return {};
    }

    // Matrix has to also be symmetric since this is the only guarantee that
    // eigenvalues are real

    std::vector<T> results(n);
    std::vector<std::vector<T>> Q(n, std::vector<T>(n));
    std::vector<std::vector<T>> R(n, std::vector<T>(n));

    size_t iterations = 0;
    while (iterations < MAX_ITERATIONS * MAX_ITERATIONS) {
        if (QR_decomposition(A, Q, R)) {
            throw std::runtime_error("QR decomposition failed!");
            return {};
        }
        A = matrix_multiply(R, Q);

        if (is_upper_triangular(A)) {
            break;
        }
        ++iterations;
    }
    // if (iterations == MAX_ITERATIONS) {
    //     matrix_print(A);
    // }
    for (size_t i = 0; i < n; ++i) {
        results.at(i) = A.at(i).at(i);
    }

    sort(results.begin(), results.end(), std::greater<>());
    return results;
}

template <typename T>
int eigen_pairs(const std::vector<std::vector<T>> &matrix,
                std::vector<std::pair<T, std::vector<T>>> &results) {
    std::vector<std::vector<T>> A = matrix;
    size_t n = A.size();
    size_t m = A.at(0).size();

    std::vector<T> eig_vals = eigen_values(matrix);
    for (size_t i = 0; i < n; ++i) {
        results.at(i).first = eig_vals.at(i);
    }

    std::vector<T> eigenvector(n);
    std::vector<T> prev_eigenvector(n);
    std::vector<std::vector<T>> temp(n, std::vector<T>(n));
    std::vector<std::vector<T>> identity(n, std::vector<T>(n));

    size_t iterations = 0;
    for (size_t i = 0; i < n; ++i) {

        for (size_t j = 0; j < n; ++j) {
            eigenvector.at(j) = static_cast<T>(1.0);
            identity.at(j).at(j) = 1;
        }

        iterations = 0;
        T delta = static_cast<T>(1e6);
        T deltaT = static_cast<T>(1e-9);
        while (iterations < MAX_ITERATIONS && delta > deltaT) {
            prev_eigenvector = eigenvector;
            temp = matrix_subtract(
                A, matrix_scalar_multiply(identity, results.at(i).first));

            if (matrix_inverse(temp, temp) != 0) {
                throw std::runtime_error("Matrix inversion failed!");
                return -1;
            }

            auto extra_slow =
                matrix_multiply(temp, vector_to_matrix_vert(prev_eigenvector));
            for (size_t j = 0; j < n; ++j) {
                eigenvector.at(j) = extra_slow.at(j).at(0);
            }
            if (normalize(eigenvector) != 0) {
                throw std::runtime_error("Normalization failed!");
                return -1;
            }
            delta = vector_norm(vector_subtract(eigenvector, prev_eigenvector));
            ++iterations;
        }
        results.at(i).second = eigenvector;
    }
    return 0;
}
} // namespace math

#endif // NUMERICAL_H
