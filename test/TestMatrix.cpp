/**
 * TestMatrix.cpp
 *
 * By Sebastian Raaphorst, 2018.
 */

#include "catch.hpp"

#include "common.h"
#include "matrix.h"

using namespace raytracer;

/// Matrices we will reuse multiple times.
const Matrix<double, 2, 3> m1 = {{0, 1, 2},
                                 {3, 4, 5}};
const Matrix<double, 3, 2> m2 = {{0, 3},
                                 {1, 4},
                                 {2, 5}};

TEST_CASE("Matrix should be constructible via initializer list and indexable", "[Matrix][initializer_list][index]") {
    Matrix<double,4,4> m = {{ 1,    2,    3,    4},
                            { 5.1,  6.1,  7.1,  8.1},
                            { 9,   10,   11,   12},
                            {13.5, 14.5, 15.5, 16.5}};

    REQUIRE(ALMOST_EQUALS(m[0][0],  1));
    REQUIRE(ALMOST_EQUALS(m[0][3],  4));
    REQUIRE(ALMOST_EQUALS(m[1][0],  5.1));
    REQUIRE(ALMOST_EQUALS(m[1][2],  7.1));
    REQUIRE(ALMOST_EQUALS(m[2][2], 11));
    REQUIRE(ALMOST_EQUALS(m[3][0], 13.5));
    REQUIRE(ALMOST_EQUALS(m[3][2], 15.5));
}

TEST_CASE("Matrix should know its size", "[Matrix][size]") {
    REQUIRE(m1.row_count() == 2);
    REQUIRE(m1.column_count() == 3);
    REQUIRE(m2.row_count() == 3);
    REQUIRE(m2.column_count() == 2);
}

TEST_CASE("Matrix should be able to determine equality", "[Matrix][equality]") {
    REQUIRE(m1 == m1);
    REQUIRE(m2 == m2);
    REQUIRE(m1 != m1 + m1);
    REQUIRE(m1 == (m1 + m1)/2);
}

TEST_CASE("Matrix comparison equality should be able to tolerate slight offsets", "[Matrix][equality]") {
    REQUIRE(m1 == m1 - EPSILON/2 * matrix_constants::ones<double, 2, 3>);
    REQUIRE(matrix_constants::I<double, 3> == matrix_constants::I<double, 3> + EPSILON/2 * matrix_constants::I<double, 3>);
}

TEST_CASE("Matrix should be transposable", "[Matrix][transpose]") {
    REQUIRE(m1.transpose() == m2);
}

TEST_CASE("Matrix transpose should be self-inverting", "[Matrix][transpose]") {
    REQUIRE(m1.transpose().transpose() == m1);
}

TEST_CASE("Matrix multiplication should multiply compatibly sized matrices", "[Matrx][multiplication]") {
    Matrix<double, 2, 2> prod = {{ 5, 14},
                                 {14, 50}};
    REQUIRE(m1 * m2 == prod);
}

TEST_CASE("Matrix multiplication should multiply square matrices", "[Matrix][multiplication]") {
    Matrix<double, 4, 4> m1 = {{1, 2, 3, 4},
                               {2, 3, 4, 5},
                               {3, 4, 5, 6},
                               {4, 5, 6, 7}};

    Matrix<double, 4, 4> m2 = {{0, 1,  2,  4},
                               {1, 2,  4,  8},
                               {2, 4,  8, 16},
                               {4, 8, 16, 32}};

    Matrix<double, 4, 4> m3 = {{24, 49,  98, 196},
                               {31, 64, 128, 256},
                               {38, 79, 158, 316},
                               {45, 94, 188, 376}};

    REQUIRE(m1 * m2 == m3);
}

TEST_CASE("Matrix multiplied with a vector should give a vector", "[Matrix][vector_multiplication]") {
    Matrix<double, 3, 4> m = {{1, 2, 3, 4},
                              {2, 4, 4, 2},
                              {8, 6, 4, 1}};

    Vector<double, 4> v = {1, 2, 3, 1};
    Vector<double, 3> prod = {18, 24, 33};
    REQUIRE(m * v == prod);
}

TEST_CASE("Matrix multiplied on the left by a vector should give a vector", "[Matrix][vector_multipication]") {
    Vector<double, 3> v = {1, 2, 3};
    Vector<double, 2> prod = {8, 26};
    Vector<double, 2> vm2 = operator*(v,m2);
    REQUIRE(vm2 == prod);
}

TEST_CASE("Identity matrix should not change the value of a matrix, whether on left or right", "[Matrix][identity][multiplication]") {
    Matrix<double, 4, 4> m = {{0, 1,  2,  4},
                              {1, 2,  4,  8},
                              {2, 4,  8, 16},
                              {4, 8, 16, 32}};
    REQUIRE(matrix_constants::I<double, 4> * m == m);
    REQUIRE(m * matrix_constants::I<double, 4> == m);
}

TEST_CASE("Identity matrix should not change the value of vectors, whether on left or right", "[Matrix][identity][vector_multiplication]") {
    Vector<double, 4> v = {4, 3, 2, 1};
    REQUIRE(matrix_constants::I<double, 4> * v == v);
    REQUIRE(v * matrix_constants::I<double, 4> == v);
}

TEST_CASE("Matrix of size 1x1 should be able to calculate its determinant", "[Matrix][determinant]") {
    Matrix<double, 1, 1> m = {{5}};
    REQUIRE(m.determinant() == 5);
}

TEST_CASE("Matrix of size 2x2 should be able to calculate its determinant", "[Matrix][determinant]") {
    Matrix<double, 2, 2> m = {{ 1, 5},
                              {-3, 2}};
    REQUIRE(m.determinant() == 17);
}

TEST_CASE("Matric of size 2x2 should be able to get submatrix", "[Matrix][submatrix]") {
    Matrix<double, 2, 2> m = {{0, 1},
                              {2, 3}};
    REQUIRE(m.submatrix(0,0) == Matrix<double, 1, 1>{{3}});
}

TEST_CASE("Matrix of size 3x3 should be able to get submatrix", "[Matrix][submatrix]") {
    Matrix<double, 3, 3> m = {{ 1, 5,  0},
                              {-3, 2,  7},
                              { 0, 6, -3}};

    Matrix<double, 2, 2> sm = {{-3, 2},
                               {0, 6}};

    REQUIRE(m.submatrix(0, 2) == sm);
}

TEST_CASE("Matrix of size 4x4 should be able to get submatrix", "[Matrix][submatrix]") {
    Matrix<double, 4, 4> m = {{-6, 1,  1, 6},
                              {-8, 5,  8, 6},
                              {-1, 0,  8, 2},
                              {-7, 1, -1, 1}};

    Matrix<double, 3, 3> sm = {{-6,  1, 6},
                               {-8,  8, 6},
                               {-7, -1, 1}};

    REQUIRE(m.submatrix(2, 1) == sm);
}

TEST_CASE("Matrix of non-square size should be able to get submatrix", "[Matrix][submatrix]") {
    Matrix<double, 4, 3> m = {{0, 1,  2},
                             {3,  4,  5},
                             {6,  7,  8},
                             {9, 10, 11}};

    Matrix<double, 3, 2> sm = {{0, 1},
                               {3, 4},
                               {6, 7}};

    REQUIRE(m.submatrix(3, 2) == sm);
}

TEST_CASE("Matrix of size 3x3 should be able to calculate its minors", "[Matrix][minor]") {
    Matrix<double, 3, 3> m = {{3,  5,  0},
                              {2, -1, -7},
                              {6, -1,  5}};

    REQUIRE(m.minor(0, 0) == -12);
    REQUIRE(m.minor(0, 1) ==  52);
    REQUIRE(m.minor(0, 2) ==   4);
    REQUIRE(m.minor(1, 0) ==  25);
    REQUIRE(m.minor(1, 1) ==  15);
    REQUIRE(m.minor(1, 2) == -33);
    REQUIRE(m.minor(2, 0) == -35);
    REQUIRE(m.minor(2, 1) == -21);
    REQUIRE(m.minor(2, 2) == -13);
}

TEST_CASE("Matrix of size 3x3 should be able to calculate its cofactors", "[Matrix][cofactors]") {
    Matrix<double, 3, 3> m = {{3,  5,  0},
                              {2, -1, -7},
                              {6, -1,  5}};

    REQUIRE(m.cofactor(0, 0) == -12);
    REQUIRE(m.cofactor(0, 1) == -52);
    REQUIRE(m.cofactor(0, 2) ==   4);
    REQUIRE(m.cofactor(1, 0) == -25);
    REQUIRE(m.cofactor(1, 1) ==  15);
    REQUIRE(m.cofactor(1, 2) ==  33);
    REQUIRE(m.cofactor(2, 0) == -35);
    REQUIRE(m.cofactor(2, 1) ==  21);
    REQUIRE(m.cofactor(2, 2) == -13);
}