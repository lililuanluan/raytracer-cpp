/**
 * transformers.h
 *
 * By Sebastian Raaphorst, 2018.
 */

#pragma once

#include <array>
#include <iomanip>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <utility>

#include "common.h"

namespace raytracer::transformers {
    template<typename T, size_t N>
    constexpr T dot_product(const std::array<T, N> &a1, const std::array<T, N> &a2) {
        T t{};
        for (size_t i = 0; i < N; ++i)
            t += a1[i] * a2[i];
        return t;
    }

    namespace helpers {
        /** Transpose matrix helpers **/
        template<typename T, size_t R, size_t C, size_t... Indices>
        constexpr std::array<T, R>
        transpose_row_helper(const std::array<std::array<T, C>, R> &m, size_t col, std::index_sequence<Indices...>) {
            return {{m[Indices][col]...}};
        }

        template<typename T, size_t R, size_t C, size_t... Indices>
        constexpr std::array<std::array<T, R>, C>
        transpose_helper(const std::array<std::array<T, C>, R> &m, std::index_sequence<Indices...>) {
            return {{transpose_row_helper<T, R, C>(m, Indices, std::make_index_sequence<R>{})...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        vector_opadd_helper(const std::array<T, N> &t1, const std::array<T, N> &t2, std::index_sequence<Indices...>) {
            return {{t1[Indices] + t2[Indices]...}};
        }

        template<typename T, size_t R, size_t C, size_t... Indices>
        constexpr std::array<std::array<T, C>, R>
        matrix_opadd_helper(const std::array<std::array<T, C>, R> &m1, const std::array<std::array<T, C>, R> &m2,
                            std::index_sequence<Indices...>) {
            return {{m1[Indices] + m2[Indices]...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        vector_opdiff_helper(const std::array<T, N> &t1, const std::array<T, N> &t2, std::index_sequence<Indices...>) {
            return {{t1[Indices] - t2[Indices]...}};
        }

        template<typename T, size_t R, size_t C, size_t... Indices>
        constexpr std::array<std::array<T, C>, R>
        matrix_opdiff_helper(const std::array<std::array<T, C>, R> &m1, const std::array<std::array<T, C>, R> &m2,
                             std::index_sequence<Indices...>) {
            return {{m1[Indices] - m2[Indices]...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        vector_opmult_helper(const std::array<T, N> &t1, const std::array<T, N> &t2, std::index_sequence<Indices...>) {
            return {{t1[Indices] * t2[Indices]...}};
        }

        template<typename T, size_t R, size_t C1, size_t C2, size_t... Indices>
        constexpr std::array<T, C2>
        matrix_row_mult_helper(const std::array<std::array<T, C1>, R> &m1, const std::array<std::array<T, C1>, C2> &m2,
                               size_t row, std::index_sequence<Indices...>) {
            return {{raytracer::transformers::dot_product<T, C1>(m1[row], m2[Indices])...}};
        }

        template<typename T, size_t R, size_t C1, size_t C2, size_t... Indices>
        constexpr std::array<std::array<T, C2>, R>
        matrix_mult_helper(const std::array<std::array<T, C1>, R> &m1, const std::array<std::array<T, C1>, C2> &m2t,
                           std::index_sequence<Indices...>) {
            return {{matrix_row_mult_helper<T, R, C1, C2>(m1, m2t, Indices, std::make_index_sequence<C2>{})...}};
        }

        template<typename T, size_t R, size_t C1, size_t C2>
        constexpr std::array<std::array<T, C2>, R>
        mat_mult(const std::array<std::array<T, C1>, R> &m1, const std::array<std::array<T, C2>, C1> &m2) {
            return matrix_mult_helper<T, R, C1, C2>(m1, transpose<T, C1, C2>(m2), std::make_index_sequence<R>{});
        }

        template<typename F, typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        scalar_opmult_helper(const F f, const std::array<T, N> &t, std::index_sequence<Indices...>) {
            return {{f * t[Indices]...}};
        }

        template<typename F, typename T, size_t R, size_t C, size_t... Indices>
        constexpr typename std::enable_if_t<std::is_arithmetic_v<F>, const std::array<std::array<T, C>, R>>
        matrix_opmult_helper(const F f, const std::array<std::array<T, C>, R> &m1, std::index_sequence<Indices...>) {
            return {{f * m1[Indices]...}};
        }

        template<typename T, size_t R, size_t C, size_t... Indices>
        constexpr std::array<T, R>
        matrix_vector_opmult_helper(const std::array<std::array<T, C>, R> &m, const std::array<T, C> &v,
                                    std::index_sequence<Indices...>) {
            return {{dot_product<T, C>(m[Indices], v)...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        vector_opdiv_helper(const std::array<T, N> &t1, const std::array<T, N> &t2, std::index_sequence<Indices...>) {
            return {{(t1[Indices] / t2[Indices])...}};
        }

        template<typename T, size_t N>
        constexpr std::array<T, N> operator/(const std::array<T, N>& a, const T& scalar) {
            std::array<T, N> result;
            for (size_t i = 0; i < N; ++i)
                result[i] = a[i] / scalar;
            return result;
        }

        template<typename F, typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        scalar_opdiv_helper(const std::array<T, N> &t, const F f, std::index_sequence<Indices...>) {
            return {{(t[Indices] / f)...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N> vector_neg_helper(const std::array<T, N> &t, std::index_sequence<Indices...>) {
            return {{-t[Indices]...}};
        }

        template<typename T, size_t N, size_t... Indices>
        constexpr std::array<T, N>
        initializer_list_to_array_helper(const std::initializer_list<T> lst, std::index_sequence<Indices...>) {
            return {{lst.begin()[Indices]...}};
        }
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<std::array<T, C>, R> make_diagonal_matrix(T nondiag, T diag) {
        std::array<std::array<T, C>, R> arr{};
        for (size_t r = 0; r < R; ++r)
            for (size_t c = 0; c < C; ++c)
                arr[r][c] = (r == c) ? diag : nondiag;
        return arr;
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<std::array<T, C>, R> make_uniform_matrix(T fill) {
        return make_diagonal_matrix<T, R, C>(fill, fill);
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<std::array<T, R>, C> transpose(const std::array<std::array<T, C>, R> &m) {
        return helpers::transpose_helper<T, R, C>(m, std::make_index_sequence<C>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> operator+(const std::array<T,N> &t1, const std::array<T,N> &t2) {
        return helpers::vector_opadd_helper<T,N>(t1, t2, std::make_index_sequence<N>{});
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<std::array<T, C>, R> operator+(const std::array<std::array<T, C>, R> &t1, const std::array<std::array<T, C>, R> &t2) {
        return helpers::matrix_opadd_helper<T,R,C>(t1, t2, std::make_index_sequence<R>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> operator-(const std::array<T,N> &t1, const std::array<T,N> &t2) {
        return helpers::vector_opdiff_helper<T,N>(t1, t2, std::make_index_sequence<N>{});
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<std::array<T, C>, R> operator-(const std::array<std::array<T, C>, R> &m1, const std::array<std::array<T, C>, R> &m2) {
        return helpers::matrix_opdiff_helper<T,R,C>(m1, m2, std::make_index_sequence<R>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> operator*(const std::array<T,N> &t1, const std::array<T,N> &t2) {
        return helpers::vector_opmult_helper<T,N>(t1, t2, std::make_index_sequence<N>{});
    }

    template<typename F, typename T, size_t N>
    constexpr typename std::enable_if_t<std::is_arithmetic_v<F>, std::array<T,N>>
    operator*(const F f, const std::array<T,N> &t) {
        return helpers::scalar_opmult_helper<F,T,N>(f, t, std::make_index_sequence<N>{});
    }

    template<typename F, typename T, size_t R, size_t C>
    constexpr typename std::enable_if_t<std::is_arithmetic_v<F>, const std::array<std::array<T, C>, R>>
    operator*(const F f, const std::array<std::array<T, C>, R> &m) {
        return helpers::matrix_opmult_helper<F,T,R,C>(f, m, std::make_index_sequence<R>{});
    }

    template<typename T, size_t R, size_t C>
    constexpr std::array<T, R> operator*(const std::array<std::array<T, C>, R> &m, const std::array<T, C> &v) {
        return helpers::matrix_vector_opmult_helper<T,R,C>(m, v, std::make_index_sequence<R>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> operator/(const std::array<T,N> &t1, const std::array<T,N> &t2) {
        return helpers::vector_opdiv_helper<T,N>(t1, t2, std::make_index_sequence<N>{});
    }

    template<typename F, typename T, size_t N>
    constexpr typename std::enable_if_t<std::is_arithmetic_v<F>, std::array<T,N>>
    operator/(const std::array<T,N> &t, const F f) {
        return helpers::scalar_opdiv_helper<F,T,N>(t, f, std::make_index_sequence<N>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> operator-(const std::array<T,N> &t) {
        return helpers::vector_neg_helper<T,N>(t, std::make_index_sequence<N>{});
    }

    template<typename T, size_t N>
    constexpr std::array<T,N> initializer_list_to_array(const std::initializer_list<T> lst) {
        return helpers::initializer_list_to_array_helper<T,N>(lst, std::make_index_sequence<N>{});
    }
}
