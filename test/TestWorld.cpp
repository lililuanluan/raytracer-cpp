/**
 * TestWorld.cpp
 *
 * By Sebastian Raaphorst, 2018.
 */

#include <catch.hpp>

#include <optional>

#include "affine_transform.h"
#include "hit.h"
#include "material.h"
#include "pointlight.h"
#include "ray.h"
#include "sphere.h"
#include "vector.h"
#include "world.h"

#include <iostream>

using namespace raytracer;

TEST_CASE("Creating a world") {
    World w;
    REQUIRE(w.getObjects().empty());
    REQUIRE_FALSE(w.getLightSource().has_value());
}

TEST_CASE("The default world") {
    constexpr PointLight light{make_point(-10, 10, -10), predefined_colours::white};

    constexpr Material m1{make_colour(0.8, 1.0, 0.6), Material::DEFAULT_AMBIENT, 0.7, 0.2, Material::DEFAULT_SHININESS};
    Sphere s1{};
    s1.setMaterial(m1);

    Sphere s2{scale(0.5, 0.5, 0.5)};

    auto w = World::getDefaultWorld();
    REQUIRE(w.getLightSource().has_value());
    REQUIRE(w.getLightSource().value() == light);
    REQUIRE(w.contains(s1));
    REQUIRE(w.contains(s2));
}

TEST_CASE("Intersect a world with a ray") {
    auto w = World::getDefaultWorld();
    constexpr Ray ray{make_point(0, 0, -5), make_vector(0, 0, 1)};
    auto xs = w.intersect(ray);
    REQUIRE(xs.size() == 4);
    REQUIRE(ALMOST_EQUALS(xs[0].getT(), 4));
    REQUIRE(ALMOST_EQUALS(xs[1].getT(), 4.5));
    REQUIRE(ALMOST_EQUALS(xs[2].getT(), 5.5));
    REQUIRE(ALMOST_EQUALS(xs[3].getT(), 6));
    REQUIRE(true);
}

TEST_CASE("Shading an intersection") {
    auto w = World::getDefaultWorld();
    Ray ray{make_point(0, 0, -5), make_vector(0, 0, 1)};
    REQUIRE_FALSE(w.getObjects().empty());
    auto s = w.getObjects().front();
    Intersection i{4, s};
    auto hit = Intersection::prepare_hit(i, ray);
    auto cOpt = w.shade_hit(hit);
    REQUIRE(cOpt.has_value());
    REQUIRE(cOpt.value() == make_colour(0.38066, 0.47583, 0.2855));
}

TEST_CASE("Shading an intersection from the inside") {
    auto w = World::getDefaultWorld();
    w.setLightSource(PointLight{make_point(0, 0.25, 0), predefined_colours::white});
    Ray ray{make_point(0, 0, 0), make_vector(0, 0, 1)};
    REQUIRE(w.getObjects().size() >= 2);
    auto s = w.getObjects()[1];
    Intersection i{0.5, s};
    auto hit = Intersection::prepare_hit(i, ray);
    auto cOpt = w.shade_hit(hit);
    REQUIRE(cOpt.has_value());
    REQUIRE(cOpt.value() == make_colour(0.90498, 0.90498, 0.90498));
}

TEST_CASE("The colour when a ray misses") {
    auto w = World::getDefaultWorld();
    Ray ray{make_point(0, 0, -5), make_vector(0, 1, 0)};
    auto c = w.colour_at(ray);
    REQUIRE(c == predefined_colours::black);
}

TEST_CASE("The colour when a ray hits") {
    auto w = World::getDefaultWorld();
    Ray ray{make_point(0, 0, -5), make_vector(0, 0, 1)};
    auto c = w.colour_at(ray);
    REQUIRE(c == make_colour(0.38066, 0.47583, 0.2855));
}

// This test case is incorrect: pg 101 from the PDF.
//TEST_CASE("The colour with an intersection behind the ray") {
//    auto w = World::getDefaultWorld();
//    REQUIRE(w.getObjects().size() >= 2);
//
//    auto outer = w.getObjects()[0];
//    auto mouter = outer->getMaterial();
//    mouter.setAmbient(1);
//    outer->setMaterial(mouter);
//
//    auto inner = w.getObjects()[1];
//    auto minner = inner->getMaterial();
//    minner.setAmbient(1);
//    inner->setMaterial(minner);
//
//    REQUIRE(w.getObjects()[0]->getMaterial().getAmbient() == 1);
//    REQUIRE(w.getObjects()[1]->getMaterial().getAmbient() == 1);
//    Ray ray{make_point(0, 0, -0.75), make_vector(0, 0, 1)};
//    REQUIRE(w.colour_at(ray) == inner->getMaterial().getColour());
//}