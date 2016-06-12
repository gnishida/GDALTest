#include "gisutils.h"

namespace gisutils {

	bool within(const std::vector<glm::vec2>& polygon, const glm::vec2& pt) {
		OGRPoint point(pt.x, pt.y);
		OGRLinearRing ring;
		for (int i = 0; i < polygon.size(); ++i) {
			ring.addPoint(polygon[i].x, polygon[i].y);
		}
		OGRPolygon poly;
		poly.addRing(&ring);
		return point.Within(&poly);
	}

}