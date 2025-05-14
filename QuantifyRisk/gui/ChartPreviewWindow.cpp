#include "ChartPreviewWindow.h"
#include <QtCharts/QChartView>

ChartPreviewWindow::ChartPreviewWindow(QChart *chart, QWidget *parent)
	: QMainWindow(parent) {
	auto *chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);
	chartView->setRubberBand(QChartView::RectangleRubberBand);
	chartView->setDragMode(QGraphicsView::ScrollHandDrag);
	chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	chartView->setInteractive(true);
	setCentralWidget(chartView);
	setWindowTitle("Simulation Preview");
	resize(1200, 800);
}
