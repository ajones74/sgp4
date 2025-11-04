#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <CoordGeodetic.h>
#include <CoordTopocentric.h>
#include <DateTime.h>
#include <Observer.h>
#include <SGP4.h>
#include <TimeSpan.h>

static size_t fetch_tle_data(const std::string &tle_filename,
                             std::vector<std::string> &tle_data) {

  std::ifstream infile(tle_filename);
  size_t found_craft_count{0};

  if (false == infile.is_open()) {
    std::cout << "Failed to open TLE File." << std::endl;
    return 0;
  }

  std::string line;
  while (std::getline(infile, line)) {
    tle_data.push_back(line);
    found_craft_count++;
  }
  // divide-by-3 as each craft (better have!) three lines of
  // details in the TLE file.
  found_craft_count = found_craft_count / 3;

  infile.close();
  return found_craft_count;
}

static bool fetch_tle_triplet(int index,
                              std::array<std::string, 3> &tle_triplet,
                              const std::vector<std::string> &tle_data) {

  tle_triplet[0] = tle_data.at(index * 3);
  tle_triplet[1] = tle_data.at((index * 3) + 1);
  tle_triplet[2] = tle_data.at((index * 3) + 2);

  return true;
}

int main() {
  // lat/lon/altitude of PIE airport.
  libsgp4::Observer obs(27.9086, -82.6865, 3.0);

  std::vector<std::string> tle_data{};
  size_t craft_count = fetch_tle_data("mPOWER.tle", tle_data);
  if (craft_count == 0) {
    return -EXIT_FAILURE;
  }
  std::cout << "Found (" << craft_count << ") craft in the TLE file..."
            << std::endl;

  std::array<std::string, 3> tle_triplet;
  int index{0};
  while (craft_count-- && fetch_tle_triplet(index++, tle_triplet, tle_data)) {
    std::string craft_name{tle_triplet[0]};

    libsgp4::Tle tle =
        libsgp4::Tle(tle_triplet[0], tle_triplet[1], tle_triplet[2]);

    libsgp4::SGP4 sgp4(tle);
    // std::cout << tle << std::endl;

    // The current time, in UTC reference
    libsgp4::DateTime now = libsgp4::DateTime::Now();
    libsgp4::DateTime epoch = tle.Epoch();
    libsgp4::TimeSpan tsince = now - epoch;

    double tsince_d = tsince.TotalSeconds();
    libsgp4::DateTime dt = tle.Epoch().AddSeconds((int)tsince_d);

    /*
     * calculate satellite position
     */
    libsgp4::Eci eci = sgp4.FindPosition(dt);
    /*
     * get look angle for observer to satellite
     */
    libsgp4::CoordTopocentric topo = obs.GetLookAngle(eci);

    if (topo.elevation() > 10.00) {
      std::cout << craft_name << " is ABOVE HORIZON: AZ(" << topo.azimuth()
                << "), EL(" << topo.elevation() << ")" << std::endl;
    }
  }
  return 0;
}
