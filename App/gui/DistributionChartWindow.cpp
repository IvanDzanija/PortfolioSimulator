#include "DistributionChartWindow.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <numeric>

DistributionChartWindow::DistributionChartWindow(
    const std::vector<double> &finalValues, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Portfolio Value Distribution - Final Day");
    setModal(false);
    resize(800, 600);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Statistics group
    QGroupBox *statsGroup = new QGroupBox("Statistics");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    // Create statistics labels
    meanLabel = new QLabel();
    medianLabel = new QLabel();
    stdDevLabel = new QLabel();
    minLabel = new QLabel();
    maxLabel = new QLabel();
    countLabel = new QLabel();

    // Add labels to layout
    QVBoxLayout *leftStats = new QVBoxLayout();
    leftStats->addWidget(meanLabel);
    leftStats->addWidget(medianLabel);
    leftStats->addWidget(stdDevLabel);

    QVBoxLayout *rightStats = new QVBoxLayout();
    rightStats->addWidget(minLabel);
    rightStats->addWidget(maxLabel);
    rightStats->addWidget(countLabel);

    statsLayout->addLayout(leftStats);
    statsLayout->addLayout(rightStats);

    mainLayout->addWidget(statsGroup);

    // Create histogram
    createHistogram(finalValues);

    // Add chart to layout
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Calculate and display statistics
    calculateStatistics(finalValues);
}

DistributionChartWindow::~DistributionChartWindow() { delete chartView; }

void DistributionChartWindow::createHistogram(
    const std::vector<double> &finalValues) {
    if (finalValues.empty()) {
        chart = new QChart();
        chart->setTitle("No data available");
        return;
    }

    // Find min and max values
    double minVal = *std::min_element(finalValues.begin(), finalValues.end());
    double maxVal = *std::max_element(finalValues.begin(), finalValues.end());

    // Create bins for histogram
    int numBins =
        std::min(50, static_cast<int>(std::sqrt(
                         finalValues.size()))); // Sturges' rule approximation
    if (numBins < 5)
        numBins = 5; // Minimum bins

    double binWidth = (maxVal - minVal) / numBins;
    if (binWidth == 0)
        binWidth = 1.0; // Handle case where all values are the same

    // Initialize bins
    std::vector<int> binCounts(numBins, 0);
    std::vector<double> binCenters(numBins);

    // Calculate bin centers
    for (int i = 0; i < numBins; ++i) {
        binCenters[i] = minVal + (i + 0.5) * binWidth;
    }

    // Count values in each bin
    for (double value : finalValues) {
        int binIndex = static_cast<int>((value - minVal) / binWidth);
        if (binIndex >= numBins)
            binIndex = numBins - 1; // Handle edge case
        if (binIndex < 0)
            binIndex = 0;
        binCounts[binIndex]++;
    }

    // Create bar series
    QBarSeries *series = new QBarSeries();
    QBarSet *barSet = new QBarSet("Frequency");

    // Create categories for x-axis
    QStringList categories;
    for (int i = 0; i < numBins; ++i) {
        *barSet << binCounts[i];
        categories << QString::number(binCenters[i], 'f', 2);
    }

    series->append(barSet);

    // Create chart
    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Distribution of Portfolio Values on Final Day");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set up axes
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Portfolio Value");
    chart->setAxisX(axisX, series);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Frequency");
    chart->setAxisY(axisY, series);

    // Make chart legend visible
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
}

void DistributionChartWindow::calculateStatistics(
    const std::vector<double> &finalValues) {
    if (finalValues.empty()) {
        meanLabel->setText("Mean: N/A");
        medianLabel->setText("Median: N/A");
        stdDevLabel->setText("Std Dev: N/A");
        minLabel->setText("Min: N/A");
        maxLabel->setText("Max: N/A");
        countLabel->setText("Count: 0");
        return;
    }

    // Calculate mean
    double mean = std::accumulate(finalValues.begin(), finalValues.end(), 0.0) /
                  finalValues.size();

    // Calculate median
    std::vector<double> sortedValues = finalValues;
    std::sort(sortedValues.begin(), sortedValues.end());
    double median;
    size_t size = sortedValues.size();
    if (size % 2 == 0) {
        median = (sortedValues[size / 2 - 1] + sortedValues[size / 2]) / 2.0;
    } else {
        median = sortedValues[size / 2];
    }

    // Calculate standard deviation
    double variance = 0.0;
    for (double value : finalValues) {
        variance += (value - mean) * (value - mean);
    }
    variance /= finalValues.size();
    double stdDev = std::sqrt(variance);

    // Min and max
    double minVal = *std::min_element(finalValues.begin(), finalValues.end());
    double maxVal = *std::max_element(finalValues.begin(), finalValues.end());

    // Update labels
    meanLabel->setText(QString("Mean: %1").arg(QString::number(mean, 'f', 4)));
    medianLabel->setText(
        QString("Median: %1").arg(QString::number(median, 'f', 4)));
    stdDevLabel->setText(
        QString("Std Dev: %1").arg(QString::number(stdDev, 'f', 4)));
    minLabel->setText(QString("Min: %1").arg(QString::number(minVal, 'f', 4)));
    maxLabel->setText(QString("Max: %1").arg(QString::number(maxVal, 'f', 4)));
    countLabel->setText(QString("Count: %1").arg(finalValues.size()));
}
