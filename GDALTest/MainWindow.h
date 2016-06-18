#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "Canvas.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void onOpenDEM();
	void onOpenSHP();
	void onSaveSHP();

private:
	Ui::MainWindowClass ui;
	Canvas canvas;
};

#endif // MAINWINDOW_H
