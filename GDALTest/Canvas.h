#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "ui_Canvas.h"
#include <vector>
#include "GShapefile.h"

class Canvas : public QWidget
{
	Q_OBJECT

public:
	Canvas(QWidget *parent = 0);
	~Canvas();

protected:
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);

public:
	void loadDEMfile(const std::string& filename);
	void loadShapfile(const std::string& filename);
	void saveShapfile(const std::string& filename);
	void constrainCameraCenter();
	void updateShapeImage();
	void selectShape(const glm::vec2& pt);

private:
	Ui::Canvas ui;
	std::vector<gs::Shape> shapes;
	QImage shapeImage;
	QImage elevationImage;
	glm::vec3 minBound;
	glm::vec3 maxBound;

	glm::vec2 cameraCenter;
	glm::vec2 prevMousePoint;
};

#endif // CANVAS_H
