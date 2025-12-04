#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

// Eigen is header-only – just drop Eigen into your include path
// (or use a package manager: vcpkg install eigen3, conan, etc.)
#include <Eigen/Dense>
#include <Eigen/QR>

// ---------------------------------------------------------------------------
// Simple structure to hold one calibration point (one GEO satellite)
// ---------------------------------------------------------------------------
struct CalPoint {
  double platonic_az_deg; // theoretical azimuth  (degrees)
  double platonic_el_deg; // theoretical elevation (degrees)
  double peak_az_deg;     // encoder value at max SNR
  double peak_el_deg;     // encoder value at max SNR

  // Residuals that the model will try to match
  double dAz() const { return peak_az_deg - platonic_az_deg; }
  double dEl() const { return peak_el_deg - platonic_el_deg; }
};

// ---------------------------------------------------------------------------
// 7-parameter pointing correction model (more than enough for a 1 m dish)
// ---------------------------------------------------------------------------
// Corrected_Az = Az_cmd
//                + AZ0   (azimuth zero offset)
//                + AC    × cos(Az_cmd)          (E-W axis tilt)
//                + AN    × sin(Az_cmd)          (N-S axis tilt)
//                + COLL  × tan(El_cmd)          (collimation /
//                non-orthogonality)
//
// Corrected_El = El_cmd
//                + EL0   (elevation zero offset)
//                + GRAV  × cos(El_cmd)          (gravity sag – small on small
//                dishes)
//                + EL_LIN × sin(El_cmd)         (optional linear term)
// ---------------------------------------------------------------------------
struct PointingModel {
  double AZ0 = 0.0;    // Azimuth zero offset        (deg)
  double AN = 0.0;     // N-S axis tilt              (deg)
  double AC = 0.0;     // E-W axis tilt              (deg)
  double COLL = 0.0;   // Collimation error          (deg)
  double EL0 = 0.0;    // Elevation zero offset      (deg)
  double GRAV = 0.0;   // Gravity term (cos El)      (deg)
  double EL_LIN = 0.0; // Optional elevation linearity (deg)

  // Apply the correction to a raw command
  void applyCorrection(double az_cmd_deg, double el_cmd_deg,
                       double &az_corrected, double &el_corrected) const {
    double az_rad = az_cmd_deg * M_PI / 180.0;
    double el_rad = el_cmd_deg * M_PI / 180.0;

    az_corrected = az_cmd_deg + AZ0 + AN * std::sin(az_rad) +
                   AC * std::cos(az_rad) + COLL * std::tan(el_rad);

    el_corrected =
        el_cmd_deg + EL0 + GRAV * std::cos(el_rad) + EL_LIN * std::sin(el_rad);
  }

  void print() const {
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "AZ0    (az offset)    = " << AZ0 << " °\n";
    std::cout << "AN     (N-S tilt)     = " << AN << " °\n";
        std::cout << "AC     (E-W tilt

)     = " << AC    << " °\n";
        std::cout << "COLL   (collimation)  = " << COLL  << " °\n";
        std::cout << "EL0    (el offset)    = " << EL0 << " °\n";
        std::cout << "GRAV   (gravity)      = " << GRAV << " °\n";
        std::cout << "EL_LIN (linearity)    = " << EL_LIN << " °\n";
  }
};

// ---------------------------------------------------------------------------
// Least-squares solver – works with 2 or more points
// ---------------------------------------------------------------------------
PointingModel solvePointingModel(const std::vector<CalPoint> &points) {
  if (points.size() < 2) {
    throw std::runtime_error("Need at least 2 calibration points");
  }

  Eigen::MatrixXd A(points.size() * 2, 7); // 2 equations per point
  Eigen::VectorXd b(points.size() * 2);

  for (size_t i = 0; i < points.size(); ++i) {
    const auto &p = points[i];
    double az_rad = p.platonic_az_deg * M_PI / 180.0;
    double el_rad = p.platonic_el_deg * M_PI / 180.0;

    // ----- Azimuth row -----
    A.row(i * 2) << 1.0,  // AZ0
        std::sin(az_rad), // AN
        std::cos(az_rad), // AC
        std::tan(el_rad), // COLL
        0.0, 0.0, 0.0;    // EL terms = 0 for Az equation

    // ----- Elevation row -----
    A.row(i * 2 + 1) << 0.0, 0.0, 0.0, 0.0, // AZ terms = 0 for El equation
        1.0,                                // EL0
        std::cos(el_rad),                   // GRAV
        std::sin(el_rad);                   // EL_LIN

    b(i * 2) = p.dAz();
    b(i * 2 + 1) = p.dEl();
  }

  // Solve normal equations: (A^T A) x = A^T b
  Eigen::VectorXd x = A.householderQr().solve(b);

  PointingModel model;
  model.AZ0 = x(0);
  model.AN = x(1);
  model.AC = x(2);
  model.COLL = x(3);
  model.EL0 = x(4);
  model.GRAV = x(5);
  model.EL_LIN = x(6);

  // Optional: print RMS residual
  double rms = std::sqrt((b - A * x).squaredNorm() / b.size());
  std::cout << "RMS pointing residual after fit: " << std::fixed
            << std::setprecision(4) << rms << " °\n\n";

  return model;
}

// ---------------------------------------------------------------------------
// Example with the two Inmarsat satellites
// ---------------------------------------------------------------------------
int main() {
  std::vector<CalPoint> cal;

  // Example data – replace with your real peaked values!
  cal.push_back({.platonic_az_deg = 235.15,
                 .platonic_el_deg = 42.12,
                 .peak_az_deg = 236.78,
                 .peak_el_deg = 41.69});

  cal.push_back({.platonic_az_deg = 197.31,
                 .platonic_el_deg = 38.41,
                 .peak_az_deg = 198.51,
                 .peak_el_deg = 38.93});

  // Add more if you peak a third or fourth GEO...

  PointingModel model = solvePointingModel(cal);
  model.print();

  // Test the correction on one of the points
  double az_corr, el_corr;
  model.applyCorrection(cal[0].platonic_az_deg, cal[0].platonic_el_deg, az_corr,
                        el_corr);
  std::cout << "\nTest on first GEO (should match peak values):\n";
  std::cout << "Corrected Az = " << az_corr << " °   (peak was "
            << cal[0].peak_az_deg << " °)\n";
  std::cout << "Corrected El = " << el_corr << " °   (peak was "
            << cal[0].peak_el_deg << " °)\n";

  return 0;
}
