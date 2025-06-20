#ifndef DISTRIBUTIONCHARTWINDOW_H
#define DISTRIBUTIONCHARTWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <vector>

class DistributionChartWindow : public QDialog {
    Q_OBJECT

  public:
    explicit DistributionChartWindow(const std::vector<double> &finalValues,
                                     QWidget *parent = nullptr);
    ~DistributionChartWindow();

  private:
    void createHistogram(const std::vector<double> &finalValues);
    void calculateStatistics(const std::vector<double> &finalValues);

    QChartView *chartView;
    QChart *chart;

    // Statistics labels
    QLabel *meanLabel;
    QLabel *medianLabel;
    QLabel *stdDevLabel;
    QLabel *minLabel;
    QLabel *maxLabel;
    QLabel *countLabel;
};

#endif // DISTRIBUTIONCHARTWINDOW_H
