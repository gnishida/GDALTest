#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionOpenDEM, SIGNAL(triggered()), this, SLOT(onOpenDEM()));
	connect(ui.actionOpenSHP, SIGNAL(triggered()), this, SLOT(onOpenSHP()));
	connect(ui.actionSaveSHP, SIGNAL(triggered()), this, SLOT(onSaveSHP()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	this->setCentralWidget(&canvas);
}

MainWindow::~MainWindow() {
}

void MainWindow::onOpenDEM() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open DEM file..."), "", tr("DEM Files (*.tif)"));
	if (filename.isEmpty()) return;

	canvas.loadDEMfile(filename.toUtf8().constData());
	canvas.update();
}

void MainWindow::onOpenSHP() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open shape file..."), "", tr("Shape Files (*.shp)"));
	if (filename.isEmpty()) return;

	canvas.loadShapfile(filename.toUtf8().constData());
	canvas.update();
}

void MainWindow::onSaveSHP() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save shape file..."), "", tr("Shape Files (*.shp)"));
	if (filename.isEmpty()) return;

	canvas.saveShapfile(filename.toUtf8().constData());
	canvas.update();
}
