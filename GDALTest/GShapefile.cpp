#include "GShapefile.h"

namespace gs {

	std::ostream& operator<<(std::ostream& os, const Variant& v) {
		if (v.type == OFTInteger) {
			os << v._intValue;
		}
		else if (v.type == OFTReal) {
			os << v._doubleValue;
		}
		else if (v.type == OFTString) {
			os << v._stringValue;
		}
		return os;
	}

	Shape::Shape() {
	}


	Shape::~Shape() {
	}

	bool Shape::load(const std::string& filename) {
		GDALAllRegister();

		GDALDataset* poDS;
		poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
		if (poDS == NULL) return false;

		// 初期化
		shapeObjects.clear();
		minBound.x = std::numeric_limits<float>::max();
		minBound.y = std::numeric_limits<float>::max();
		minBound.z = std::numeric_limits<float>::max();
		maxBound.x = -std::numeric_limits<float>::max();
		maxBound.y = -std::numeric_limits<float>::max();
		maxBound.z = -std::numeric_limits<float>::max();

		int nLayers = poDS->GetLayerCount();
		int i = 0;
		for (int n = 0; n < nLayers; ++n) {
			OGRLayer* poLayer = poDS->GetLayer(n);
			shapeObjects.resize(shapeObjects.size() + poLayer->GetFeatureCount());
			
			OGRFeature* poFeature;
			poLayer->ResetReading();
			while ((poFeature = poLayer->GetNextFeature()) != NULL) {
				// 属性の名前を読み込む
				std::vector<std::string> fieldNames;
				for (int j = 0; j < poFeature->GetFieldCount(); ++j) {
					OGRFieldDefn* poFieldDefn = poFeature->GetFieldDefnRef(j);
					fieldNames.push_back(poFieldDefn->GetNameRef());
				}

				// 属性の値を読み込む
				OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
				for (int j = 0; j < poFDefn->GetFieldCount(); ++j) {
					OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(j);
					if (poFieldDefn->GetType() == OFTInteger) {
						shapeObjects[i].attributes[fieldNames[j]] = Variant(poFeature->GetFieldAsInteger(j));
					}
					else if (poFieldDefn->GetType() == OFTInteger64) {
						shapeObjects[i].attributes[fieldNames[j]] = Variant(poFeature->GetFieldAsInteger(j));
					}
					else if (poFieldDefn->GetType() == OFTReal) {
						shapeObjects[i].attributes[fieldNames[j]] = Variant(poFeature->GetFieldAsDouble(j));
					}
					else if (poFieldDefn->GetType() == OFTString) {
						shapeObjects[i].attributes[fieldNames[j]] = Variant(poFeature->GetFieldAsString(j));
					}
					else {
						shapeObjects[i].attributes[fieldNames[j]] = Variant(poFeature->GetFieldAsString(j));
					}
				}

				// このshapeのベクトルデータを読み込む
				OGRGeometry* poGeometry = poFeature->GetGeometryRef();
				if (poGeometry != NULL) {
					if (wkbFlatten(poGeometry->getGeometryType()) == wkbPoint) {
						shapeObjects[i].parts.resize(1);
						shapeObjects[i].parts[0].points.resize(1);

						OGRPoint* poPoint = (OGRPoint*)poGeometry;
						shapeObjects[i].parts[0].points[0].x = poPoint->getX();
						shapeObjects[i].parts[0].points[0].y = poPoint->getY();

						updateBounds(poPoint);
					}
					else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
						OGRPolygon* poPolygon = (OGRPolygon*)poGeometry;
						readPolygon(poPolygon, shapeObjects[i]);
					}
					else if (wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon) {
						OGRMultiPolygon* poMultiPolygon = (OGRMultiPolygon*)poGeometry;
						readMultiPolygon(poMultiPolygon, shapeObjects[i]);
					}
					else {
						// not supported
					}
				}

				// shapeObjectsのインデックスをインクリメント
				i++;

				// OGRが取得したメモリを開放
				OGRFeature::DestroyFeature(poFeature);
			}
		}

		GDALClose(poDS);

		return true;
	}

	void Shape::updateBounds(OGRPoint* poPoint) {
		// bounding boxを更新
		if (poPoint->getX() < minBound.x) minBound.x = poPoint->getX();
		if (poPoint->getX() > maxBound.x) maxBound.x = poPoint->getX();
		if (poPoint->getY() < minBound.y) minBound.y = poPoint->getY();
		if (poPoint->getY() > maxBound.y) maxBound.y = poPoint->getY();
		if (poPoint->getZ() < minBound.z) minBound.z = poPoint->getZ();
		if (poPoint->getZ() > maxBound.z) maxBound.z = poPoint->getZ();
	}

	void Shape::readMultiPolygon(OGRMultiPolygon* poMultiPolygon, ShapeObject& shapeObject) {
		int numGeometries = poMultiPolygon->getNumGeometries();
		int partsIndex = 0;
		for (int i = 0; i < numGeometries; ++i) {
			OGRGeometry* geo = poMultiPolygon->getGeometryRef(i);
			if (wkbFlatten(geo->getGeometryType()) == wkbPolygon) {
				OGRPolygon* poPolygon = (OGRPolygon*)geo;

				int nInteriorRings = poPolygon->getNumInteriorRings();
				shapeObject.parts.resize(shapeObject.parts.size() + nInteriorRings + 1);

				OGRLinearRing* ring = poPolygon->getExteriorRing();
				readRing(ring, shapeObject.parts[partsIndex++]);

				for (int j = 0; j < nInteriorRings; ++j) {
					OGRLinearRing* ring = poPolygon->getInteriorRing(j);
					readRing(ring, shapeObject.parts[partsIndex++]);
				}
			}
		}
	}

	void Shape::readPolygon(OGRPolygon* poPolygon, ShapeObject& shapeObject) {
		int nInteriorRings = poPolygon->getNumInteriorRings();
		shapeObject.parts.resize(nInteriorRings + 1);

		OGRLinearRing* ring = poPolygon->getExteriorRing();
		readRing(ring, shapeObject.parts[0]);

		for (int j = 0; j < nInteriorRings; ++j) {
			OGRLinearRing* ring = poPolygon->getInteriorRing(j);
			readRing(ring, shapeObject.parts[j + 1]);
		}
	}

	void Shape::readRing(OGRLinearRing* ring, ShapePart& shapePart) {
		shapePart.points.resize(ring->getNumPoints());
		for (int j = 0; j < ring->getNumPoints(); ++j) {
			OGRPoint point;
			ring->getPoint(j, &point);

			shapePart.points[j].x = point.getX();
			shapePart.points[j].y = point.getY();
			updateBounds(&point);
		}
	}

	bool DEM::load(const std::string& filename) {
		GDALAllRegister();

		GDALDataset* poDS;
		poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_RASTER, NULL, NULL, NULL);
		if (poDS == NULL) return false;

		double adfGeoTransform[6];
		if (poDS->GetGeoTransform(adfGeoTransform) == CE_None) {
			origin.x = adfGeoTransform[0];
			origin.y = adfGeoTransform[3];

			pixelSize.x = adfGeoTransform[1];
			pixelSize.y = abs(adfGeoTransform[5]);
		}

		width = poDS->GetRasterXSize() * pixelSize.x;
		height = poDS->GetRasterYSize() * pixelSize.y;
		data.resize(width * height);
		min_val = std::numeric_limits<float>::max();
		max_val = -std::numeric_limits<float>::max();

		// bandが存在しない場合は、エラー
		if (poDS->GetRasterCount() == 0) return false;

		// 最初のbandのみを読み込む。複数bandは未対応
		GDALRasterBand* poBand = poDS->GetRasterBand(1);

		int nBlockXSize, nBlockYSize;
		poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
		//printf("Block=%dx%d Type=%s, ColorInterp=%s\n", nBlockXSize, nBlockYSize, GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

		// 最低、最高の値を取得
		int bGotMin, bGotMax;
		double adfMinMax[2];
		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);
		if (!(bGotMin && bGotMax)) GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		//printf("Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1]);
		min_val = adfMinMax[0];
		max_val = adfMinMax[1];
			
		//int nXSize = poBand->GetXSize();
		//int nYSize = poBand->GetYSize();

		int nXBlocks = (poBand->GetXSize() + nBlockXSize - 1) / nBlockXSize;
		int nYBlocks = (poBand->GetYSize() + nBlockYSize - 1) / nBlockYSize;
		float *pData = (float *)CPLMalloc(sizeof(float*) * nBlockXSize * nBlockYSize);
		for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
			for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {
				int nXValid, nYValid;

				poBand->ReadBlock(iXBlock, iYBlock, pData);

				// Compute the portion of the block that is valid
				// for partial edge blocks.
				if ((iXBlock + 1) * nBlockXSize > poBand->GetXSize())
					nXValid = poBand->GetXSize() - iXBlock * nBlockXSize;
				else
					nXValid = nBlockXSize;
				if ((iYBlock + 1) * nBlockYSize > poBand->GetYSize())
					nYValid = poBand->GetYSize() - iYBlock * nBlockYSize;
				else
					nYValid = nBlockYSize;

				for (int iY = 0; iY < nYValid; iY++) {
					for (int iX = 0; iX < nXValid; iX++) {
						float val;
						if (pData[iY * nBlockXSize + iX] > max_val) {
							val = max_val;
						}
						else if (pData[iY * nBlockXSize + iX] < min_val) {
							val = min_val;
						}
						else {
							val = pData[iY * nBlockXSize + iX];
						}

						for (int y = 0; y < pixelSize.y; ++y) {
							for (int x = 0; x < pixelSize.x; ++x) {
								data[((iYBlock * nBlockYSize + iY) * pixelSize.y + y) * width + (iXBlock * nBlockXSize + iX) * pixelSize.x + x] = val;
							}
						}
						
					}
				}
			}
		}

		GDALClose(poDS);

		return true;
	}
}