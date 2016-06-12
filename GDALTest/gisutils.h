#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <ogrsf_frmts.h>
/*
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
*/

namespace gisutils {

	bool within(const std::vector<glm::vec2>& polygon, const glm::vec2& pt);

}