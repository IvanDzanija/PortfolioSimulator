#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CSV_parser.h"
#include "Portfolio.h"
#include <QChartView>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

QT_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void loadAvailableCoins();
    void addCoinToPortfolio();
    void runMonteCarloSimulation();
    void runPCAAnalysis();
    void showFinalDayDistribution();

  private:
    QWidget *centralWidget;
    QListWidget *coinList;
    QDoubleSpinBox *amountInput;
    QListWidget *portfolioView;
    QSpinBox *simSpin;
    QSpinBox *stepSpin;
    QChartView *chartView;
    QLineEdit *startDateInput;

    CSV_Parser parser;
    Portfolio portfolio;
    QMap<QString, double> portfolioMap; // file -> amount

    QSpinBox *componentsSpin;
    QLineEdit *pcaStartDateInput;
    QSpinBox *pcaDataPointsSpin;
    QTextEdit *pcaResultsText;
    std::vector<Doubles_Matrix> lastSimulationResults;
    QPushButton *distributionButton;

    void
    plotSimulation(const std::vector<std::vector<std::vector<double>>> &data);

    void displayPCAResults(
        const std::vector<std::pair<double, std::vector<double>>> &components,
        const std::vector<std::string> &labels, double total_variability);

    void
    plotPCA(std::vector<std::pair<double, std::vector<double>>> &pcaResults,
            const std::vector<std::string> &labels);

    void plotEigenvectors(const std::vector<std::vector<double>> &eigenvectors,
                          const std::vector<std::string> &labels);
};

#endif // MAINWINDOW_H
