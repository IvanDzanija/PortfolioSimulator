#include "ChartPreviewWindow.h"
#include "MainWindow.h"
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <ctime>
#include <fstream>

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
	timestamp;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), centralWidget(new QWidget(this)) {
	setWindowTitle("Portfolio Simulator");
	resize(1000, 600);

	auto *layout = new QVBoxLayout(centralWidget);

	coinList = new QListWidget(this);
	layout->addWidget(new QLabel("Available Coins:"));
	layout->addWidget(coinList);

	amountInput = new QDoubleSpinBox(this);
	amountInput->setRange(0.01, 10000);
	amountInput->setValue(1.0);
	layout->addWidget(new QLabel("Amount to Add:"));
	layout->addWidget(amountInput);

	QPushButton *addButton = new QPushButton("Add to Portfolio", this);
	connect(addButton, &QPushButton::clicked, this,
			&MainWindow::addCoinToPortfolio);
	layout->addWidget(addButton);

	layout->addWidget(new QLabel("Current Portfolio:"));
	portfolioView = new QListWidget(this);
	layout->addWidget(portfolioView);

	simSpin = new QSpinBox(this);
	simSpin->setRange(10, 10000);
	simSpin->setValue(100);
	stepSpin = new QSpinBox(this);
	stepSpin->setRange(10, 1000);
	stepSpin->setValue(60);

	layout->addWidget(new QLabel("Simulations:"));
	layout->addWidget(simSpin);
	layout->addWidget(new QLabel("Steps:"));
	layout->addWidget(stepSpin);

	QPushButton *simulateButton = new QPushButton("Run Monte Carlo", this);
	connect(simulateButton, &QPushButton::clicked, this,
			&MainWindow::runMonteCarloSimulation);
	layout->addWidget(simulateButton);

	setCentralWidget(centralWidget);
	loadAvailableCoins();
}

MainWindow::~MainWindow() = default;

void MainWindow::loadAvailableCoins() {
	coinList->clear();
	QString datasetPath =
		QCoreApplication::applicationDirPath() + "/../datasets";
	QDir datasetDir(datasetPath);
	QStringList files =
		datasetDir.entryList(QStringList() << "*.csv", QDir::Files);
	for (const QString &file : files) {
		const QString &coinName = file.mid(5, file.length() - 9);
		coinList->addItem(coinName);
	}
}

void MainWindow::addCoinToPortfolio() {
	auto *item = coinList->currentItem();
	if (!item)
		return;

	QString filename = item->text();
	double amount = amountInput->value();

	if (portfolioMap.contains(filename)) {
		QMessageBox::warning(this, "Duplicate",
							 "This coin is already in the portfolio.");
		return;
	}

	try {
		Cryptocurrency coin = parser.fastReadCryptoCSV(
			"../datasets/coin_" + filename.toStdString() + ".csv");
		portfolio.add_asset(coin, amount);
		portfolioMap.insert(filename, amount);
		portfolioView->addItem(QString("%1 (%2)")
								   .arg(QString::fromStdString(coin.get_name()))
								   .arg(amount));
	} catch (const std::exception &e) {
		QMessageBox::critical(this, "Load Error", e.what());
	}
}

void MainWindow::runMonteCarloSimulation() {
	if (portfolioMap.isEmpty()) {
		QMessageBox::warning(this, "Empty Portfolio",
							 "Add coins to your portfolio first.");
		return;
	}

	timestamp start = std::chrono::floor<std::chrono::seconds>(
		std::chrono::system_clock::now());

	try {
		auto result =
			portfolio.monte_carlo(simSpin->value(), stepSpin->value(), start);
		plotSimulation(result);
	} catch (const std::exception &e) {
		QMessageBox::critical(this, "Simulation Error", e.what());
	}
}

void MainWindow::plotSimulation(const std::vector<Doubles_Matrix> &data) {

	QChart *chart = new QChart();
	for (const auto &sim : data) {
		QLineSeries *series = new QLineSeries();
		for (size_t step = 0; step < sim.size(); ++step) {
			double sum = 0;
			for (double val : sim[step])
				sum += val;
			std::cout << "Step: " << step << ", Sum: " << sum << std::endl;
			series->append(step, sum / sim[step].size());
		}
		chart->addSeries(series);
	}

	chart->createDefaultAxes();
	chart->zoomReset();
	chart->setTitle("Monte Carlo Simulation");
	auto *preview = new ChartPreviewWindow(chart, this);
	preview->show();
}
