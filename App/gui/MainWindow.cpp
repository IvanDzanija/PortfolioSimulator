#include "ChartPreviewWindow.h"
#include "DateTime_formatting.h"
#include "DistributionChartWindow.h"
#include "MainWindow.h"
#include <QDir>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
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
    resize(1200, 800);

    auto *mainLayout = new QHBoxLayout(centralWidget);

    // Left panel for portfolio management
    auto *leftPanel = new QVBoxLayout();

    coinList = new QListWidget(this);
    leftPanel->addWidget(new QLabel("Available Coins:"));
    leftPanel->addWidget(coinList);

    amountInput = new QDoubleSpinBox(this);
    amountInput->setRange(0.01, 10000);
    amountInput->setValue(1.0);
    leftPanel->addWidget(new QLabel("Amount to Add:"));
    leftPanel->addWidget(amountInput);

    QPushButton *addButton = new QPushButton("Add to Portfolio", this);
    connect(addButton, &QPushButton::clicked, this,
            &MainWindow::addCoinToPortfolio);
    leftPanel->addWidget(addButton);

    leftPanel->addWidget(new QLabel("Current Portfolio:"));
    portfolioView = new QListWidget(this);
    leftPanel->addWidget(portfolioView);

    QPushButton *removeButton = new QPushButton("Remove Selected Asset", this);
    QDoubleSpinBox *removeAmountInput = new QDoubleSpinBox(this);
    removeAmountInput->setRange(0.01, 100000);
    removeAmountInput->setValue(1.0);
    removeAmountInput->setSingleStep(0.1);
    leftPanel->addWidget(new QLabel("Amount to Remove:"));
    leftPanel->addWidget(removeAmountInput);
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
    leftPanel->addWidget(removeButton);

    // Right panel for simulations and PCA
    auto *rightPanel = new QVBoxLayout();

    // Monte Carlo Section
    auto *monteCarloGroup = new QGroupBox("Monte Carlo Simulation");
    auto *monteCarloLayout = new QVBoxLayout(monteCarloGroup);

    startDateInput = new QLineEdit(this);
    startDateInput->setPlaceholderText("YYYY-MM-DD HH:MM:SS");
    monteCarloLayout->addWidget(new QLabel("Start Time:"));
    monteCarloLayout->addWidget(startDateInput);

    simSpin = new QSpinBox(this);
    simSpin->setRange(1, 100000);
    simSpin->setValue(100);
    stepSpin = new QSpinBox(this);
    stepSpin->setRange(1, 100000);
    stepSpin->setValue(60);

    monteCarloLayout->addWidget(new QLabel("Simulations:"));
    monteCarloLayout->addWidget(simSpin);
    monteCarloLayout->addWidget(new QLabel("Steps:"));
    monteCarloLayout->addWidget(stepSpin);

    QPushButton *simulateButton = new QPushButton("Run Monte Carlo", this);
    connect(simulateButton, &QPushButton::clicked, this,
            &MainWindow::runMonteCarloSimulation);
    monteCarloLayout->addWidget(simulateButton);

    distributionButton = new QPushButton("Show Final Day Distribution", this);
    distributionButton->setEnabled(false);
    connect(distributionButton, &QPushButton::clicked, this,
            &MainWindow::showFinalDayDistribution);
    monteCarloLayout->addWidget(distributionButton);

    rightPanel->addWidget(monteCarloGroup);

    // PCA Analysis Section
    auto *pcaGroup = new QGroupBox("PCA Analysis");
    auto *pcaLayout = new QVBoxLayout(pcaGroup);

    // Number of principal components input
    componentsSpin = new QSpinBox(this);
    componentsSpin->setRange(1, 20);
    componentsSpin->setValue(3);
    pcaLayout->addWidget(new QLabel("Number of Principal Components:"));
    pcaLayout->addWidget(componentsSpin);

    // PCA start date (separate from Monte Carlo)
    pcaStartDateInput = new QLineEdit(this);
    pcaStartDateInput->setPlaceholderText("YYYY-MM-DD HH:MM:SS");
    pcaLayout->addWidget(new QLabel("PCA Analysis Start Time:"));
    pcaLayout->addWidget(pcaStartDateInput);

    QPushButton *pcaButton = new QPushButton("Run PCA Analysis", this);
    connect(pcaButton, &QPushButton::clicked, this,
            &MainWindow::runPCAAnalysis);
    pcaLayout->addWidget(pcaButton);

    // Text area for PCA results
    pcaResultsText = new QTextEdit(this);
    pcaResultsText->setMaximumHeight(150);
    pcaResultsText->setReadOnly(true);
    pcaLayout->addWidget(new QLabel("Variability Explained:"));
    pcaLayout->addWidget(pcaResultsText);

    rightPanel->addWidget(pcaGroup);

    // Add panels to main layout
    auto *leftWidget = new QWidget();
    leftWidget->setLayout(leftPanel);
    leftWidget->setMaximumWidth(350);

    auto *rightWidget = new QWidget();
    rightWidget->setLayout(rightPanel);

    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);

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
        coin.print();
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
    bool time_set = false;
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
            time_set = true;
        } catch (const std::exception &e) {
            QMessageBox::critical(
                this, "Invalid Date",
                "Please enter a valid date in the format YYYY-MM-DD HH:MM:SS.");
            return;
        }
    }
    try {
        int sim_count = simSpin->value();
        int step_count = stepSpin->value();
        std::vector<Doubles_Matrix> result =
            portfolio.monte_carlo(sim_count, step_count, start);
        if (result.empty()) {
            QMessageBox::warning(this, "Simulation Error",
                                 "No data available for the given date. Check "
                                 "console log for more info.");
            return;
        }
        lastSimulationResults = result;
        distributionButton->setEnabled(true);

        // This can me multithreaded, one thread for fetching the simulation,
        // one for fetching cut-off data
        if (time_set) {
            std::cout << "Monte Carlo simulation started at: "
                      << timestamp_to_string(start) << std::endl;
            Doubles_Matrix rem = portfolio.future_log_returns(start);
            if (portfolio.get_asset_count() == 1) {
                // this is only for testing
                Doubles_Matrix last_row(1, std::vector<double>(step_count));
                for (size_t i = 0; i < rem.size(); ++i) {
                    for (size_t j = 0; j < step_count; ++j) {
                        last_row.at(i).at(j) = rem.at(i).at(j);
                    }
                }
                result.push_back(last_row);
            }
        } else {
            std::cout << "Monte Carlo simulation started at: now" << std::endl;
        }
        plotSimulation(result);
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Simulation Error", e.what());
    }
}

void MainWindow::runPCAAnalysis() {
    if (portfolioMap.isEmpty()) {
        QMessageBox::warning(this, "Empty Portfolio",
                             "Add coins to your portfolio first.");
        return;
    }

    QString datetimeStr = pcaStartDateInput->text();
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
        std::vector<std::pair<double, std::vector<double>>> components;
        std::vector<std::string> labels = portfolio.get_asset_names();

        double total_variance;
        double variance_explained;
        int components_count;
        if (componentsSpin->value() == 0) {
            components_count = -1; // Default to all components
        } else {
            components_count = componentsSpin->value();
        }
        int info = portfolio.PCA(start, components_count, components,
                                 total_variance, variance_explained);
        std::cout << "PCA analysis completed with " << components.size()
                  << " components." << std::endl;

        if (info != 0) {
            QMessageBox::critical(
                this, "PCA Analysis Error",
                "PCA analysis failed. Check console log for more info.");
            return;
        } else {
            QMessageBox::information(
                this, "PCA Analysis",
                QString("PCA analysis completed successfully.\n"
                        "Total Variance: %1\n"
                        "Variance Explained: %2")
                    .arg(total_variance)
                    .arg(variance_explained));
        }

        // Display variability explained
        displayPCAResults(components, labels, total_variance);

        // Plot eigenvectors
        std::vector<std::vector<double>> eigenvectors;
        for (const auto &component : components) {
            eigenvectors.push_back(component.second);
        }
        plotEigenvectors(eigenvectors, labels);

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "PCA Analysis Error", e.what());
    }
}

void MainWindow::displayPCAResults(
    const std::vector<std::pair<double, std::vector<double>>> &components,
    const std::vector<std::string> &labels, double total_variability) {
    // Clear previous results
    pcaResultsText->clear();

    QString resultsText;
    resultsText += "PCA Analysis Results:\n";
    resultsText += "====================\n\n";

    // Assuming pcaResults has a method to get variability explained
    // Adjust this based on your actual PCA result structure

    double cumulative_variance = 0.0;
    for (size_t i = 0; i < components.size(); ++i) {
        cumulative_variance += components[i].first;
        double percentage = (components[i].first / total_variability) * 100.0;
        resultsText += QString("PC%1: %2% of variance\n")
                           .arg(i + 1)
                           .arg(QString::number(percentage, 'f', 2));
    }

    resultsText +=
        QString("\nCumulative variance explained: %1%\n")
            .arg(QString::number(cumulative_variance / total_variability * 100,
                                 'f', 2));

    resultsText +=
        QString("Number of components: %1\n").arg(componentsSpin->value());
    resultsText += QString("Portfolio assets: %1\n").arg(portfolioMap.size());

    pcaResultsText->setPlainText(resultsText);
}

void MainWindow::plotEigenvectors(
    const std::vector<std::vector<double>> &eigenvectors,
    const std::vector<std::string> &labels) {
    // Create chart for eigenvectors visualization
    QChart *chart = new QChart();
    chart->setTitle("Principal Component Eigenvectors");

    // Assuming pcaResults has methods to get eigenvectors
    int numComponents = std::min(componentsSpin->value(),
                                 static_cast<int>(eigenvectors.size()));

    // Create a bar chart showing the weights of each asset in each principal
    // component
    QBarSeries *series = new QBarSeries();

    // Get asset names for categories
    QStringList assetNames;
    for (auto it = portfolioMap.begin(); it != portfolioMap.end(); ++it) {
        assetNames << it.key();
    }

    // Create bar sets for each principal component
    for (int pc = 0; pc < numComponents; ++pc) {
        QBarSet *barSet = new QBarSet(QString("PC%1").arg(pc + 1));

        // Add eigenvector values for each asset
        for (size_t asset = 0; asset < eigenvectors[pc].size(); ++asset) {
            *barSet << eigenvectors[pc][asset];
        }

        series->append(barSet);
    }

    chart->addSeries(series);

    // Create category axis for assets
    QBarCategoryAxis *categoryAxis = new QBarCategoryAxis();
    categoryAxis->append(assetNames);
    chart->setAxisX(categoryAxis, series);

    // Create value axis
    QValueAxis *valueAxis = new QValueAxis();
    valueAxis->setTitleText("Eigenvector Weight");
    chart->setAxisY(valueAxis, series);

    // Show the chart in a preview window
    auto *preview = new ChartPreviewWindow(chart, this);
    preview->show();
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

void MainWindow::showFinalDayDistribution() {
    if (lastSimulationResults.empty()) {
        QMessageBox::warning(this, "No Simulation Data",
                             "Please run a Monte Carlo simulation first.");
        return;
    }

    // Extract final day values from all simulations
    std::vector<double> finalDayValues;

    // Get the last step (final day) from each simulation
    for (const auto &simulation : lastSimulationResults) {
        if (!simulation.empty() && !simulation[0].empty()) {
            // Sum all assets for the final day of this simulation
            double portfolioValue = 0.0;
            size_t finalStep = simulation[0].size() - 1; // Last step index

            for (size_t asset = 0; asset < simulation.size(); ++asset) {
                if (finalStep < simulation[asset].size()) {
                    portfolioValue += simulation[asset][finalStep];
                }
            }
            finalDayValues.push_back(portfolioValue);
        }
    }

    if (finalDayValues.empty()) {
        QMessageBox::warning(this, "No Data",
                             "No final day data found in simulation results.");
        return;
    }

    // Create and show the distribution window
    auto *distributionWindow =
        new DistributionChartWindow(finalDayValues, this);
    distributionWindow->show();
}
