#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

struct Point {
  double x, y;
};

/**
 * Generates points along a cochlear-inspired logarithmic spiral where
 * consecutive points are separated by an approximately constant arc length 's'.
 *
 * The spiral is defined as r(theta) = r_max * exp(-k * theta), spiraling
 * inward. The parameter 'k' controls the tightness of the spiral and is
 * computed based on the desired number of turns (e.g., human cochlea has ~2.5
 * turns).
 *
 * Points are generated incrementally using the approximation delta_theta = s /
 * r, which holds well for loosely wound spirals and matches the description
 * where the angle between points increases as radius decreases.
 *
 * @param r_max Initial (maximum) radius.
 * @param r_min Minimum radius to stop at.
 * @param s Constant arc length between points.
 * @param num_turns Number of spiral turns (e.g., 2.5 for cochlear mimic).
 * @return Vector of points along the spiral.
 */
std::vector<Point> generate_cochlear_scan(double r_max, double r_min, double s,
                                          double num_turns) {
  std::vector<Point> points;
  if (r_max <= r_min || s <= 0 || num_turns <= 0) {
    std::cerr << "SOmeThinG is WrOnG." << std::endl;
    return points; // Invalid parameters
  }

  // Compute k based on number of turns: theta_max = num_turns * 2 * PI
  double theta_max = num_turns * 2.0 * M_PI;
  // double k = std::log(r_max / r_min) / theta_max;
  double a = (r_max - r_min) / theta_max;

  double theta = 0.0;
  double r = r_max;

  std::ofstream ofs;
  ofs.open("cochlear.csv");

  std::cout << "R(" << r << "), Rmin(" << r_min << "), Rmax(" << r_max
            << "), a(" << a << ")\n";
  while (r > r_min) {
    double x = r * std::cos(theta);
    double y = r * std::sin(theta);
    points.push_back({x, y});

    ofs << theta << "," << r << "\n";
    // ofs << theta << "," << r << "," << x << "," << y << "\n";
    // std::cout << theta << "," << r << "," << x << "," << y << "\n";

    // Approximate delta_theta for constant arc length s (ignoring radial
    // component for simplicity)
    double delta_theta = s / r;

    // Update theta and recompute r
    theta += delta_theta;
    // r = r_max * std::exp(-k * theta);
    r = r_max - a * theta;

    if (r < r_min) {
      r = r_min;
    }
  }

  return points;
}

// Example usage (can be removed or commented out)
int main() {
  double r_max = 10.0;
  double r_min = 0.1;
  double s = 2.0;
  double num_turns = 3.0;

  auto points = generate_cochlear_scan(r_max, r_min, s, num_turns);
  return 0;
}
