# File: python/gui.py
import sys
from PyQt6.QtWidgets import (
    QApplication,
    QMainWindow,
    QPushButton,
    QLabel,
    QVBoxLayout,
    QWidget,
)

# Uvezi naš C++ modul naziva 'montecarlo'
import montecarlo


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Monte Carlo Simulator")

        # Gumb za pokretanje simulacije
        self.button = QPushButton("Pokreni simulaciju")
        self.button.clicked.connect(self.run_simulation)

        # Oznaka za prikaz rezultata
        self.label = QLabel("Rezultat će se pojaviti ovdje")

        # Raspored u vertikalni layout
        layout = QVBoxLayout()
        layout.addWidget(self.button)
        layout.addWidget(self.label)

        # Glavni widget
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def run_simulation(self):
        # Pozovi C++ funkciju Monte Carlo simulacije
        result = montecarlo.run_simulation(10000, 100)
        # Prikaži rezultat u oznaci (zaokružen na 2 decimale)
        self.label.setText(f"Prosječna vrijednost: {result:.2f}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
