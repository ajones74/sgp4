#include <cmath>
#include <iostream>
#include <string>

#include "DateTime.h"
#include "Eci.h"
#include "SGP4.h"

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
  // Example observer (from GPS: e.g., Clearwater, FLorida)
  double lat_rad = 27.9086 * PI / 180.0;
  double lon_rad = -82.6865 * PI / 180.0;
  double alt_m = 3.0;

  std::string line1 =
      "1 62363U 24244B   25304.56767013 -.00000012  00000+0  00000+0 0  9997";
  std::string line2 =
      "2 62363   0.0820 342.5037 0006936 239.2979 138.1528  5.00116245  9439";

  libsgp4::Tle tle(line1, line2);
  // Initialize and propagate (adapt to library API)
  libsgp4::SGP4 propagator(tle);

  libsgp4::DateTime tle_epoch = tle.Epoch();
  libsgp4::DateTime now = libsgp4::DateTime::Now(true);

  int ts = now.Minute() - tle_epoch.Minute();

  libsgp4::Eci eci = propagator.FindPosition(ts);

  const libsgp4::Vector vector = eci.Position();

  // Get ECEF position (km to m)
  double Xs = vector.x * 1000.0;
  double Ys = vector.y * 1000.0;
  double Zs = vector.z * 1000.0;

  // Compute elevation
  double el = compute_elevation(Xs, Ys, Zs, lat_rad, lon_rad, alt_m);

  std::cout << "Elevation for satellite: " << el << " deg" << std::endl;

  return 0;
}
