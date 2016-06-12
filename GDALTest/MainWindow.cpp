#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionOpenSHP, SIGNAL(triggered()), this, SLOT(onOpenSHP()));
	connect(ui.actionOpenDEM, SIGNAL(triggered()), this, SLOT(onOpenDEM()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	this->setCentralWidget(&canvas);
}

MainWindow::~MainWindow() {
}

void MainWindow::onOpenSHP() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open shape file..."), "", tr("Shape Files (*.shp)"));
	if (filename.isEmpty()) return;

	canvas.loadShapfile(filename.toUtf8().constData());
	canvas.update();
}

void MainWindow::onOpenDEM() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open DEM file..."), "", tr("DEM Files (*.tif)"));
	if (filename.isEmpty()) return;

	canvas.loadDEMfile(filename.toUtf8().constData());
	canvas.update();
}