# PortfolioSimulator

**Cryptocurrency Portfolio Simulation Toolkit** C++/Qt gui application for
simulating cryptocurrency portfolios based on historical data.

## Features

- Import historical data from CSV files.
- Select portfolio assets and set initial ammounts.
- Choose simulation period. (start and end dates)
- Simulate portfolio performance over the selected period.
- View portfolio value over time.
- View simulation distribution.
- View simulation statistics.
- Generate eigen portfolio.
- Preview the portfolio in a chart.

## Models

- Monte Carlo simulation.
- Geometric Brownian Motion (GBM) simulation.
- PCA (Principal Component Analysis) decomposition of the portfolio.
- Eigen portfolio generation based on PCA.
- Correlation matrix generation.

## Future Plans

- Add more simulation models (e.g. GARCH, ARIMA).
- Add more statistical analysis tools.
- Fetch live data from exchanges.
- Nicer UI/UX.
- Optimize performance for large datasets.
- Optimize threading and parallel processing where possible.
- Add more testing.

## Ultimate Goal

- Write a standard library for portfolio simulation that can be used in other
  projects.

## Requirements

- C++20
- Qt 6.x
- CMake 3.16+

## Build Instructions

1. Clone the repository:
    ```bash
    git clone https:://github.com/IvanDzanija/PortfolioSimulator.git
    ```
2. Navigate to the project directory:
    ```bash
    cd PortfolioSimulator/App
    ```
3. Run the given build script:
    ```bash
    ./build_script.sh
    ```
4. Navigate to the build directory:
    ```bash
    cd build
    ```
5. Run the application:
    ```bash
    ./PortfolioSimulator
    ```

## Contributing

New repository for future development will be created soon.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file
for details.

## Contact

For any questions or suggestions, feel free to contact me through email
available on my [GitHub profile](https://github.com/IvanDzanija).
