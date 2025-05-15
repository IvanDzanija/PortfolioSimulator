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

	void
	plotSimulation(const std::vector<std::vector<std::vector<double>>> &data);
};

#endif // MAINWINDOW_H
