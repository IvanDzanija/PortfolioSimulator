#include "ChartPreviewWindow.h"
#include "DateTime_formatting.h"
#include "MainWindow.h"
#include <QDir>
#include <QLabel>
#include <QLineEdit>
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

	QPushButton *removeButton = new QPushButton("Remove Selected Asset", this);
	QDoubleSpinBox *removeAmountInput = new QDoubleSpinBox(this);
	removeAmountInput->setRange(0.01, 100000);
	removeAmountInput->setValue(1.0);
	removeAmountInput->setSingleStep(0.1);
	layout->addWidget(new QLabel("Amount to Remove:"));
	layout->addWidget(removeAmountInput);
	connect(
		removeButton, &QPushButton::clicked, this, [this, removeAmountInput]() {
			auto *item = portfolioView->currentItem();
			if (!item)
				return;

			QString displayText = item->text();
			QString keyToRemove;

			// Try to find the matching key from portfolioMap
			for (auto it = portfolioMap.begin(); it != portfolioMap.end();
				 ++it) {
				if (displayText.startsWith(it.key())) {
					keyToRemove = it.key();
					break;
				}
			}
			if (!keyToRemove.isEmpty()) {
				double amountToRemove = removeAmountInput->value();
				double currentAmount = portfolioMap[keyToRemove];

				if (amountToRemove > currentAmount) {
					QMessageBox::warning(
						this, "Invalid Amount",
						"You can't remove more than you added.");
					return;
				}

				Cryptocurrency coin =
					portfolio.get_asset(keyToRemove.toStdString());
				portfolio.remove_asset(coin, amountToRemove);
				if (currentAmount - amountToRemove <= 0.00001) {
					portfolioMap.remove(keyToRemove);
					delete portfolioView->takeItem(portfolioView->currentRow());
				} else {
					portfolioMap[keyToRemove] -= amountToRemove;
					item->setText(
						QString("%1 (%2)")
							.arg(QString::fromStdString(coin.get_name()))
							.arg(portfolioMap[keyToRemove]));
				}
			}
		});
	layout->addWidget(removeButton);

	startDateInput = new QLineEdit(this);
	startDateInput->setPlaceholderText("YYYY-MM-DD HH:MM:SS");
	layout->addWidget(new QLabel("Start Time:"));
	layout->addWidget(startDateInput);

	simSpin = new QSpinBox(this);
	simSpin->setRange(10, 10000);
	simSpin->setValue(100);
	stepSpin = new QSpinBox(this);
	stepSpin->setRange(10, 10000);
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
		std::cout << "Coin already in portfolio, updating amount." << std::endl;
		portfolioMap[filename] += amount;
		portfolio.add_asset(portfolio.get_asset(filename.toStdString()),
							amount);
		for (int i = 0; i < portfolioView->count(); ++i) {
			QListWidgetItem *currentItem = portfolioView->item(i);
			if (currentItem->text().startsWith(filename)) {
				currentItem->setText(
					QString("%1 (%2)")
						.arg(QString::fromStdString(
							portfolio.get_asset(filename.toStdString())
								.get_name()))
						.arg(portfolioMap[filename]));
				break;
			}
		}
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
	QString datetimeStr = startDateInput->text();

	timestamp start;
	if (datetimeStr.isEmpty()) {
		start = std::chrono::floor<std::chrono::seconds>(
			std::chrono::system_clock::now());
	} else if (datetimeStr.length() != 19) {
		QMessageBox::critical(
			this, "Invalid Date",
			"Please enter a valid date in the format YYYY-MM-DD HH:MM:SS.");
		return;
	} else {
		try {
			start = string_to_timestamp(datetimeStr.toStdString());
		} catch (const std::exception &e) {
			QMessageBox::critical(
				this, "Invalid Date",
				"Please enter a valid date in the format YYYY-MM-DD HH:MM:SS.");
			return;
		}
	}
	try {
		std::vector<Doubles_Matrix> result =
			portfolio.monte_carlo(simSpin->value(), stepSpin->value(), start);
		if (result.empty()) {
			QMessageBox::warning(this, "Simulation Error",
								 "No data available for the given date. Check "
								 "console log for more info.");
			return;
		}
		plotSimulation(result);
	} catch (const std::exception &e) {
		QMessageBox::critical(this, "Simulation Error", e.what());
	}
}

void MainWindow::plotSimulation(const std::vector<Doubles_Matrix> &data) {

	QChart *chart = new QChart();
	for (const auto &sim : data) {
		QLineSeries *series = new QLineSeries();
		for (size_t step = 0; step < sim.at(0).size(); ++step) {
			double sum = 0;
			for (size_t i = 0; i < sim.size(); ++i) {
				sum += sim.at(i).at(step);
			}
			series->append(step, sum);
		}
		chart->addSeries(series);
	}

	chart->createDefaultAxes();
	chart->zoomReset();
	chart->setTitle("Monte Carlo Simulation");
	auto *preview = new ChartPreviewWindow(chart, this);
	preview->show();
}
