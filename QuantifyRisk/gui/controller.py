# File: python/gui.py
import sys
import time
from datetime import datetime
import os
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
import matplotlib.pyplot as plt
from PyQt6.QtWidgets import (
    QApplication,
    QMainWindow,
    QPushButton,
    QLabel,
    QVBoxLayout,
    QWidget,
)

import PortfolioSimulator

DATASETS_DIR = "datasets"


class Controller(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Crypto Monte Carlo Simulator")
        self.setGeometry(200, 200, 800, 600)

        layout = QVBoxLayout(self)

        # Coin selection
        self.coin_list = QListWidget()
        self.coin_list.setSelectionMode(QListWidget.SelectionMode.MultiSelection)
        layout.addWidget(QLabel("Select Cryptocurrencies from datasets/:"))
        layout.addWidget(self.coin_list)

        # Inputs
        input_layout = QHBoxLayout()
        self.sim_spin = QSpinBox()
        self.sim_spin.setRange(10, 10000)
        self.sim_spin.setValue(100)

        self.steps_spin = QSpinBox()
        self.steps_spin.setRange(10, 1000)
        self.steps_spin.setValue(60)

        input_layout.addWidget(QLabel("Simulations:"))
        input_layout.addWidget(self.sim_spin)
        input_layout.addWidget(QLabel("Steps:"))
        input_layout.addWidget(self.steps_spin)
        layout.addLayout(input_layout)

        # Buttons
        self.run_button = QPushButton("Run Monte Carlo")
        self.run_button.clicked.connect(self.run_simulation)
        layout.addWidget(self.run_button)

        # Chart area
        self.canvas = FigureCanvas(plt.Figure())
        layout.addWidget(self.canvas)

        self.load_available_coins()

    def load_available_coins(self):
        self.coin_list.clear()
        for file in os.listdir(DATASETS_DIR):
            if file.endswith(".csv"):
                self.coin_list.addItem(file)

    def run_simulation(self):
        selected_items = self.coin_list.selectedItems()
        if not selected_items:
            QMessageBox.warning(
                self, "No Selection", "Please select at least one CSV file."
            )
            return

        parser = main.CSV_Parser()
        portfolio = main.Portfolio()

        try:
            for item in selected_items:
                filename = os.path.join(DATASETS_DIR, item.text())
                coin = parser.fastReadCryptoCSV(filename)
                portfolio.add_asset(
                    coin, 1.0
                )  # Add each coin with weight 1.0 (as AssetAmount)

            # Start date as timestamp (e.g., Jan 1 2022)
            start = int(time.mktime(datetime(2022, 1, 1).timetuple()))

            result = portfolio.monte_carlo(
                self.sim_spin.value(), self.steps_spin.value(), start
            )
        except Exception as e:
            QMessageBox.critical(self, "Simulation Error", str(e))
            return

        self.plot_simulation(result)

    def plot_simulation(self, data):
        self.canvas.figure.clear()
        ax = self.canvas.figure.add_subplot(111)

        for sim in data:
            # sim = [ [coin0, coin1, ...], [coin0, coin1, ...], ... ]
            # We'll just average across coins at each step
            avg_series = [sum(day) / len(day) for day in sim]
            ax.plot(avg_series, alpha=0.5)

        ax.set_title("Monte Carlo Simulation (Averaged Portfolio Value)")
        ax.set_xlabel("Step")
        ax.set_ylabel("Value")
        self.canvas.draw()
