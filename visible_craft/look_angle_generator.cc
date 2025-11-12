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

struct look_angle_data_t {
  uint64_t m_current_tick;
  double m_az;
  double m_el;
  double m_range;
  double m_range_rate;
};

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
  // divide by three as three lines of text define each craft in a TLE file...
  // (Three-Line-Element? I know, it's referred to as a TWO Line Element
  // file...)
  found_craft_count = found_craft_count / 3;

  infile.close();
  return found_craft_count;
}

static void
generate_track_data(const std::vector<std::string> &discovered_craft,
                    const libsgp4::CoordGeodetic &observer_GPS, int index) {

  libsgp4::Observer obs(observer_GPS);
  libsgp4::Tle tle = libsgp4::Tle(discovered_craft.at(index * 3),
                                  discovered_craft.at((index * 3) + 1),
                                  discovered_craft.at((index * 3) + 2));

  libsgp4::SGP4 sgp4(tle);

  // The current time, in UTC reference time
  libsgp4::DateTime now = libsgp4::DateTime::Now();
  libsgp4::DateTime epoch = tle.Epoch();
  libsgp4::TimeSpan tsince = now - epoch;

  double tsince_d = tsince.TotalSeconds();
  libsgp4::DateTime dt = tle.Epoch().AddSeconds((int)tsince_d);

  std::cout << "CRAFT: (" << tle.Name() << ")." << std::endl;
  double elevation{0.0};

  std::vector<look_angle_data_t> look_angle_data;
  do {
    libsgp4::Eci eci = sgp4.FindPosition(dt);
    libsgp4::CoordTopocentric topo = obs.GetLookAngle(eci);
    dt = dt.AddSeconds(1);

    // "Ticks" are in microsceconds of time...
    uint64_t current_tick = dt.Ticks();
    look_angle_data_t t{current_tick, topo.azimuth(), topo.elevation(),
                        topo.range(), topo.range_rate()};

    look_angle_data.push_back(std::move(t));

    elevation = topo.elevation();
  } while (elevation > 10.0);

  std::string ofilename{tle.Name()};

  std::replace(ofilename.begin(), ofilename.end(), ' ', '_');

  std::ofstream ofs(ofilename);
  if (!ofs) {
    std::cout << "CANNOT OPEN OUTPUT FILE.";
    return;
  }

  bool first_pass{true};
  int64_t microsecond_adjustment{0};
  for (const auto &it : look_angle_data) {
    int64_t corrected_time = it.m_current_tick - libsgp4::UnixEpoch;
    if (first_pass) {
      microsecond_adjustment = corrected_time % 1000000;
      first_pass = false;
    }
    corrected_time -= microsecond_adjustment;
    corrected_time /= 1000000;
    ofs << corrected_time << "," << it.m_az << "," << it.m_el << ","
        << it.m_range << "," << it.m_range_rate << "\n";
  }

  std::cout << "DONE!" << std::endl << std::endl;
}

int main() {

  // lat/lon/altitude of PIE airport.
  libsgp4::CoordGeodetic observer_GPS(27.9086, -82.6865, 3.0);
  libsgp4::Observer obs(observer_GPS);

  std::vector<std::string> tle_data{};
  size_t craft_count = fetch_tle_data("mPOWER.tle", tle_data);
  if (craft_count == 0) {
    return -EXIT_FAILURE;
  }
  std::cout << "Found (" << craft_count << ") craft in the TLE file..."
            << std::endl;

  std::vector<std::string> discovered_craft;
  int index{0};
  int discovered_craft_count{0};
  while (craft_count--) {
    std::string craft_name{tle_data.at(index * 3)};

    libsgp4::Tle tle =
        libsgp4::Tle(tle_data.at(index * 3), tle_data.at((index * 3) + 1),
                     tle_data.at((index * 3) + 2));

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

      discovered_craft.push_back(tle_data.at((index * 3)));
      discovered_craft.push_back(tle_data.at((index * 3) + 1));
      discovered_craft.push_back(tle_data.at((index * 3) + 2));
      discovered_craft_count++;
    }
    index++;
  }

  if (discovered_craft_count == 0) {
    std::cout << "NO craft found visible above the horizon...";
    return 0;
  }
  std::cout << "Discovered craft count:(" << discovered_craft_count << ")"
            << std::endl;

  for (int d = 0; d < discovered_craft_count; ++d) {
    generate_track_data(discovered_craft, observer_GPS, d);
  }

  return 0;
}
