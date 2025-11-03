#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <CoordGeodetic.h>
#include <CoordTopocentric.h>
#include <DateTime.h>
#include <Observer.h>
#include <SGP4.h>
#include <TimeSpan.h>

static bool fetch_tle_data(const std::string &tle_filename,
                           std::array<std::string, 3> &tle_triplet) {

  if

    return true;
}

int main() {
  // lat/lon/altitude of PIE airport.
  libsgp4::Observer obs(27.9086, -82.6865, 3.0);

  std::array<std::string, 3> tle_triplet{};

  while (fetch_tle_data("mPOWER.tle", tle_triplet)) {
    std::string craft_name{};

    libsgp4::Tle tle =
        libsgp4::Tle(tle_triplet[0], tle_triplet[1], tle_triplet[2]);

    libsgp4::SGP4 sgp4(tle);

    // std::cout << tle << std::endl;

    // The current time, in UTC reference
    libsgp4::DateTime now = libsgp4::DateTime::Now();
    libsgp4::DateTime epoch = tle.Epoch();
    libsgp4::TimeSpan tsince = now - epoch;

    double tsince_d = tsince.TotalMinutes();
    libsgp4::DateTime dt = tle.Epoch().AddMinutes((int)tsince_d);
    // std::cout << "Minutes since TLE Epoch:(" << tsince_d << ")" << std::endl;
    // std::cout << "epoch(" << epoch << ")" << std::endl;
    // std::cout << "now(" << now << ")" << std::endl;

    /*
     * calculate satellite position
     */
    libsgp4::Eci eci = sgp4.FindPosition(dt);
    /*
     * get look angle for observer to satellite
     */
    libsgp4::CoordTopocentric topo = obs.GetLookAngle(eci);
    /*
     * convert satellite position to geodetic coordinates
     */
    libsgp4::CoordGeodetic geo = eci.ToGeodetic();

    // std::cout << topo << " " << geo << std::endl;
    if (topo.elevation() <= 10.00) {
      std::cout << "BELOW HORIZON.";
    } else {
      std::cout << "ABOVE HORIZON.";
    }
  }
  return 0;
}
