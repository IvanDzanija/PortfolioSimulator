#ifndef CRYPTOCURRENCY_H
#define CRYPTOCURRENCY_H
#include "DateTime_formatting.h"
#include <cmath>
#include <vector>

struct Candle {

  public:
    // OHLC
    double open, high, low, close;
    double volume, marketcap;
    timestamp time;

    double log_return;

    Candle() {}

    Candle(double o, double h, double l, double c, double v, double m,
           ::timestamp t)
        : open(o), high(h), low(l), close(c), volume(v), marketcap(m), time(t) {
        log_return = std::log(close / open);
    }

    void print() const {
        std::cout << "Candle at " << time << ": "
                  << "Open: " << open << ", High: " << high << ", Low: " << low
                  << ", Close: " << close << ", Volume: " << volume
                  << ", Market Cap: " << marketcap
                  << ", Log Return: " << log_return << std::endl;
    }
};

class Cryptocurrency {
  public:
    std::string name, tick;
    std::vector<Candle> hist_data;
    bool metrics_calculated = false;

    // Drift = sum(return_i) / n;
    // Volatility = sqrt((1/n - 1) * sum((return_i - Drift)^2)
    double drift, volatility;

    Cryptocurrency() : name(""), tick(""), hist_data({}) {}

    Cryptocurrency(const std::string &name, const std::string &tick,
                   const std::vector<Candle> &hist_data)
        : name(name), tick(tick), hist_data(hist_data) {}

    ~Cryptocurrency() {
        // std::string and std::vector manage their own resources.
    }

    Cryptocurrency(const Cryptocurrency &other)
        : name(other.name), tick(other.tick), hist_data(other.hist_data) {}

    Cryptocurrency &operator=(const Cryptocurrency &other) {
        if (this != &other) {
            name = other.name, tick = other.tick;
            hist_data = other.hist_data;
        }
        return *this;
    }

    Cryptocurrency(Cryptocurrency &&other) noexcept
        : name(std::move(other.name)), tick(std::move(other.tick)),
          hist_data(std::move(other.hist_data)) {}

    Cryptocurrency &operator=(Cryptocurrency &&other) noexcept {
        if (this != &other) {
            name = std::move(other.name), tick = std::move(other.tick);
            hist_data = std::move(other.hist_data);
        }
        return *this;
    }

    struct Hash {
        size_t operator()(const Cryptocurrency &crypto) const {
            return std::hash<std::string>()(crypto.name);
        }
    };

    bool operator==(const Cryptocurrency &other) const {
        return name == other.name;
    }

    void print() const {
        std::cout << "Cryptocurrency: " << name << " (" << tick << ")\n"
                  << "Historical Data Size: " << hist_data.size() << "\n"
                  << "Starting Timestamp: " << hist_data.front().time << "\n"
                  << "Ending Timestamp: " << hist_data.back().time << "\n"
                  << std::endl;
    }

    std::string get_name() const { return name; }
    std::string get_tick() const { return tick; }
    void set_name(const std::string &name) { this->name = name; }
    void set_tick(const std::string &tick) { this->tick = tick; }

    void calculate_metrics();
    void reevaluate_metrics();
    void individual_monte_carlo(int sim_count, int forecast_days);
};
#endif // CRYPTOCURRENCY_H
