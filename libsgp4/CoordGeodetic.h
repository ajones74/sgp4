/*
 * Copyright 2013 Daniel Warner <contact@danrw.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "Util.h"

#include <iomanip>
#include <sstream>
#include <string>

namespace libsgp4 {

/**
 * @brief Stores a geodetic location (latitude, longitude, altitude).
 *
 * Internally the values are stored in radians and kilometres.
 */
struct CoordGeodetic {
public:
  /**
   * Default constructor
   */
  CoordGeodetic() = default;

  /**
   * Constructor
   * @param[in] lat the latitude (degrees by default)
   * @param[in] lon the longitude (degrees by default)
   * @param[in] alt the altitude in kilometers
   * @param[in] is_radians whether the latitude/longitude is in radians
   */
  CoordGeodetic(double lat, double lon, double alt, bool is_radians = false) {
    if (is_radians) {
      m_latitude = lat;
      m_longitude = lon;
    } else {
      m_latitude = Util::DegreesToRadians(lat);
      m_longitude = Util::DegreesToRadians(lon);
    }
    m_altitude = alt;
  }

  /**
   * Copy constructor
   * @param[in] geo object to copy from
   */
  CoordGeodetic(const CoordGeodetic &geo) {
    m_latitude = geo.m_latitude;
    m_longitude = geo.m_longitude;
    m_altitude = geo.m_altitude;
  }

  /**
   * Assignment operator
   * @param[in] geo object to copy from
   */
  CoordGeodetic &operator=(const CoordGeodetic &geo) {
    if (this != &geo) {
      m_latitude = geo.m_latitude;
      m_longitude = geo.m_longitude;
      m_altitude = geo.m_altitude;
    }
    return *this;
  }

  /**
   * Dump this object to a string
   * @returns string
   */
  std::string ToString() const {
    std::stringstream ss;
    ss << std::right << std::fixed << std::setprecision(3);
    ss << "Lat: " << std::setw(8) << Util::RadiansToDegrees(m_latitude);
    ss << ", Lon: " << std::setw(8) << Util::RadiansToDegrees(m_longitude);
    ss << ", Alt: " << std::setw(10) << m_altitude;
    return ss.str();
  }

  double latitude() const { return Util::RadiansToDegrees(m_latitude); }
  double longitude() const { return Util::RadiansToDegrees(m_longitude); }
  double altitude() const { return m_altitude; }

  /** latitude in radians (-PI >= latitude < PI) */
  double m_latitude{};
  /** latitude in radians (-PI/2 >= latitude <= PI/2) */
  double m_longitude{};
  /** altitude in kilometers */
  double m_altitude{};
};

/**
 * Dump a Coordgeodetic to a stream
 * @param[in,out] strm stream to output to
 * @param[in] g the CoordGeodetic to print
 */
inline std::ostream &operator<<(std::ostream &strm, const CoordGeodetic &g) {
  return strm << g.ToString();
}

} // namespace libsgp4
