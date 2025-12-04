#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

// ------------------------------------------------------------------
// Practical Cochlear (Constant Arc-Length Spiral) Scan for GEO Peaking
// ------------------------------------------------------------------
struct Point2D {
  double az_offset_deg; // azimuth offset from nominal (degrees)
  double el_offset_deg; // elevation offset from nominal (degrees)
};

class CochlearScanner {
public:
  struct Config {
    double start_radius_deg =
        4.0; // initial search radius (±4 degrees is safe for Ka-band)
    double min_radius_deg =
        0.12; // stop when inside ~1/5 of typical 0.7 degrees beam
    double arc_step_deg = 0.15; // constant arc length between points
    double dwell_time_sec =
        0.18; // time to wait at each point for RSSI to settle
  };

  explicit CochlearScanner(const Config &cfg = {}) : config(cfg) {}

  // Generate full spiral point list (az/el offsets from nominal pointing)
  std::vector<Point2D> generateScan() const {
    std::vector<Point2D> path;
    path.reserve(1200); // typical for 4° → 0.12° spiral

    double radius = config.start_radius_deg;
    double theta = 0.0; // radians

    while (radius > config.min_radius_deg) {
      double az = radius * std::cos(theta);
      double el = radius * std::sin(theta);

      path.push_back({az, el});

      // ---- Constant arc-length magic ----
      double delta_theta =
          config.arc_step_deg * (M_PI / 180.0) / radius; // rad per step
      theta += delta_theta;

      // Shrink radius by fixed arc length (converted to degrees)
      radius -= config.arc_step_deg;

      // Safety: prevent negative radius (should never happen)
      if (radius < config.min_radius_deg)
        radius = config.min_radius_deg;
    }

    // Add final center point (theoretical peak)
    path.push_back({0.0, 0.0});
    return path;
  }

  // Optional: stream points one by one with simulated RSSI feedback
  void executeWithRssiFeedback(
      const std::function<double(double az_offset, double el_offset)>
          &rssiCallback,
      double &best_az_offset, double &best_el_offset, double &best_rssi) const {
    double radius = config.start_radius_deg;
    double theta = 0.0;

    best_rssi = -999.0;

    while (radius > config.min_radius_deg) {
      double az = radius * std::cos(theta);
      double el = radius * std::sin(theta);

      double current_rssi = rssiCallback(az, el);

      if (current_rssi > best_rssi) {
        best_rssi = current_rssi;
        best_az_offset = az;
        best_el_offset = el;
      }

      // Update spiral
      double delta_theta = config.arc_step_deg * (M_PI / 180.0) / radius;
      theta += delta_theta;
      radius -= config.arc_step_deg;
    }

    // Final center check
    double final_rssi = rssiCallback(0.0, 0.0);
    if (final_rssi > best_rssi) {
      best_rssi = final_rssi;
      best_az_offset = 0.0;
      best_el_offset = 0.0;
    }
  }

private:
  Config config;
};

// ------------------------------------------------------------------
// Example usage with simulated Gaussian beam (for testing)
// ------------------------------------------------------------------
int main() {
  CochlearScanner scanner;

  std::cout << "Generating practical cochlear scan path...\n";
  auto path = scanner.generateScan();

  std::cout << "Total points: " << path.size() << "\n\n";
  std::cout << std::fixed << std::setprecision(4);
  for (size_t i = 0; i < path.size(); i += 25) { // print every 25th point
    std::cout << "Point " << std::setw(4) << i
              << " -> Az offset: " << std::setw(8) << path[i].az_offset_deg
              << "°   El offset: " << std::setw(8) << path[i].el_offset_deg
              << "°\n";
  }

  // Bonus: simulate peaking on a real beam (0.7° HPBW Gaussian)
  auto simulatedRssi = [](double az_off_deg, double el_off_deg) -> double {
    double distance = std::hypot(az_off_deg, el_off_deg);
    double beamwidth_deg = 0.7;
    return -12.0 * (distance / (beamwidth_deg / 1.665)) *
           (distance / (beamwidth_deg / 1.665)); // approx dB
  };

  double best_az = 0, best_el = 0, best_rssi = -999;
  scanner.executeWithRssiFeedback(simulatedRssi, best_az, best_el, best_rssi);

  std::cout << "\nPeak found at: Az offset = " << best_az
            << "°   El offset = " << best_el
            << "°   with simulated RSSI = " << best_rssi << " dB\n";

  return 0;
}
