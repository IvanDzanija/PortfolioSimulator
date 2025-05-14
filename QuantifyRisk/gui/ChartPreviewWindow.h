#ifndef CHARTPREVIEWWINDOW_H
#define CHARTPREVIEWWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

class ChartPreviewWindow : public QMainWindow {
	Q_OBJECT
  public:
	explicit ChartPreviewWindow(QChart *chart, QWidget *parent = nullptr);
};

#endif // CHARTPREVIEWWINDOW_H
