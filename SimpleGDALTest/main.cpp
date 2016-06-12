#include <iostream>
#include "ogrsf_frmts.h"

using namespace std;

int main() {
	GDALAllRegister();

	GDALDataset* poDS;
	//poDS = (GDALDataset*)GDALOpenEx("..\\3D\\Building.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
	poDS = (GDALDataset*)GDALOpenEx("..\\3D\\Parcels.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
	//poDS = (GDALDataset*)GDALOpenEx("..\\3D\\zone1.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (poDS == NULL) {
		printf("Open failed.\n");
		return 1;
	}

	int nLayers = poDS->GetLayerCount();

	cout << poDS->GetGCPProjection() << endl;
	cout << poDS->GetRasterXSize() << endl;
	cout << poDS->GetRasterYSize() << endl;
	cout << poDS->GetRefCount() << endl;
	cout << poDS->GetProjectionRef() << endl;

	for (int i = 0; i < nLayers; ++i) {
		OGRLayer* poLayer = poDS->GetLayer(i);

		// .prjファイルの内容を取得
		OGRSpatialReference* spatialRef = poLayer->GetSpatialRef();
		spatialRef->dumpReadable();


		cout << poLayer->GetGeometryColumn() << endl;
		cout << poLayer->GetFeatureCount() << endl;

		OGRPoint pt;
		pt.setX(0);
		pt.setY(0);
		OGRLinearRing ring;
		ring.addPoint(-10, -10);
		ring.addPoint(-10, 10);
		ring.addPoint(10, 10);
		ring.addPoint(10, -10);
		ring.addPoint(-10, -10);
		OGRPolygon poly;
		poly.addRing(&ring);
		if (pt.Within(&poly)) {
			cout << "Inside." << endl;
		}
		else {
			cout << "Outside." << endl;
		}

		OGRFeature* poFeature;
		poLayer->ResetReading();
		int cnt = 0;
		while ((poFeature = poLayer->GetNextFeature()) != NULL) {
			for (int j = 0; j < poFeature->GetFieldCount(); ++j) {
				OGRFieldDefn* poFieldDefn = poFeature->GetFieldDefnRef(j);
				//cout << poFieldDefn->GetNameRef() << endl;
			}

			OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

			for (int j = 0; j < poFDefn->GetFieldCount(); ++j) {
				OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(j);
				if (poFieldDefn->GetType() == OFTInteger) {
					//cout << poFeature->GetFieldAsInteger(j) << endl;
				}
				else if (poFieldDefn->GetType() == OFTInteger64) {
					//cout << poFeature->GetFieldAsInteger64(j) << endl;
				}
				else if (poFieldDefn->GetType() == OFTReal) {
					//cout << poFeature->GetFieldAsDouble(j) << endl;
				}
				else if (poFieldDefn->GetType() == OFTString) {
					//cout << poFeature->GetFieldAsString(j) << endl;
				}
				else {
					//cout << poFeature->GetFieldAsString(j) << endl;
				}
			}

			OGRGeometry* poGeometry = poFeature->GetGeometryRef();
			if (poGeometry != NULL) {
				if (wkbFlatten(poGeometry->getGeometryType()) == wkbPoint) {
					OGRPoint* poPoint = (OGRPoint*)poGeometry;
					cout << poPoint->getX() << "," << poPoint->getY() << endl;
				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
					OGRPolygon* poPolygon = (OGRPolygon*)poGeometry;
					OGRLinearRing* ring = poPolygon->getExteriorRing();
					for (int j = 0; j < ring->getNumPoints(); ++j) {
						OGRPoint point;
						ring->getPoint(j, &point);
						//cout << point.getX() << "," << point.getY() << endl;
					}

					int nInteriorRings = poPolygon->getNumInteriorRings();
					if (nInteriorRings > 0) {
						cout << "Interior ring: " << nInteriorRings << endl;
					}
				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon) {
					OGRMultiPolygon* poMultiPolygon = (OGRMultiPolygon*)poGeometry;
					cout << "Multi polygon." << endl;
				}
				else {
					cout << "Unknown geometry type." << endl;
				}
			}
			OGRFeature::DestroyFeature(poFeature);

			cnt++;
		}

		cout << "#cnt: " << cnt << endl;
	}

	GDALClose(poDS);



	return 0;
}