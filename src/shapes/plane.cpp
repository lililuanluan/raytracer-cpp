/**
 * plane.cpp
 *
 * By Sebastian Raaphorst, 2018.
 */

#include <memory>
#include <vector>

#include "intersection.h"
#include "plane.h"
#include "ray.h"
#include "vec.h"

using namespace raytracer::impl;

namespace raytracer::shapes {
    Plane::Plane(dummy d) noexcept: Shape{d} {}

    std::shared_ptr<Plane> Plane::createPlane() noexcept {
        std::shared_ptr<Plane> plane = std::make_shared<Plane>(dummy{});
        registerInstance(plane);
        return plane;
    }

    const std::vector<Intersection> Plane::localIntersection(const Ray &ray) const noexcept {
        if (absd(ray.getDirection()[tuple_constants::y]) < 1e-4)
            return {};

        const auto t = -ray.getOrigin()[tuple_constants::y] / ray.getDirection()[tuple_constants::y];
        return {Intersection{t, shared_from_this()}};
    }

    const Tuple Plane::localNormalAt(const Tuple &point) const noexcept {
        return predefined_tuples::y1;
    }
}