#include <cmath>
#include <iostream>
#include <string>

#include "sgp4.h"

const double PI = 3.141592653589793;
const double A = 6378137.0;         // WGS84 semi-major axis (m)
const double E2 = 6.69437999014e-3; // Eccentricity squared

// Function to convert geodetic to ECEF
void geodetic_to_ecef(double lat, double lon, double h, double &X, double &Y,
                      double &Z) {
  double sin_lat = sin(lat);
  double cos_lat = cos(lat);
  double sin_lon = sin(lon);
  double cos_lon = cos(lon);

  double N = A / sqrt(1 - E2 * sin_lat * sin_lat);
  X = (N + h) * cos_lat * cos_lon;
  Y = (N + h) * cos_lat * sin_lon;
  Z = (N * (1 - E2) + h) * sin_lat;
}

// Function to convert satellite ECEF to AZ/EL (degrees); returns EL only for
// this adaptation
double compute_elevation(double Xs, double Ys, double Zs, double lat,
                         double lon, double h) {
  double Xo, Yo, Zo;
  geodetic_to_ecef(lat, lon, h, Xo, Yo, Zo);

  double dX = Xs - Xo;
  double dY = Ys - Yo;
  double dZ = Zs - Zo;

  double sin_lat = sin(lat);
  double cos_lat = cos(lat);
  double sin_lon = sin(lon);
  double cos_lon = cos(lon);

  double E = -sin_lon * dX + cos_lon * dY;
  double N = -sin_lat * cos_lon * dX - sin_lat * sin_lon * dY + cos_lat * dZ;
  double U = cos_lat * cos_lon * dX + cos_lat * sin_lon * dY + sin_lat * dZ;

  double r = sqrt(E * E + N * N + U * U);

  double el = asin(U / r) * 180.0 / PI;
  return el;
}

int main() {
  // Example observer (from GPS: e.g., New York)
  double lat_rad = 40.7128 * PI / 180.0;
  double lon_rad = -74.0060 * PI / 180.0;
  double alt_m = 0.0;

  // Example tsince (minutes since TLE epoch, from GPS UTC - epoch)
  double tsince_min = 0.0; // For current time = epoch; compute dynamically

  // Example TLE for one mPOWER satellite (update with real/current)
  std::string line1 =
      "1 54755U 22174A   25298.12345678  .00000000  00000-0  10000-3 0  9999";
  std::string line2 =
      "2 54755  0.0450  0.0000 0001000  0.0000  0.0000  5.00123456 12345";

  // Initialize and propagate (adapt to library API)
  Sgp4 propagator(line1, line2);
  propagator.FindPosition(tsince_min);

  // Get ECEF position (km to m)
  double Xs = propagator.GetPosition()[0] * 1000.0;
  double Ys = propagator.GetPosition()[1] * 1000.0;
  double Zs = propagator.GetPosition()[2] * 1000.0;

  // Compute elevation
  double el = compute_elevation(Xs, Ys, Zs, lat_rad, lon_rad, alt_m);

  std::cout << "Elevation for satellite: " << el << " deg" << std::endl;

  return 0;
}
