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
 * @brief Stores a topocentric location (azimuth, elevation, range and range
 * rate).
 *
 * Azimuth and elevation are stored in radians. Range in kilometres. Range
 * rate in kilometres/second.
 */
struct CoordTopocentric {
public:
  /**
   * Default constructor
   */
  CoordTopocentric() = default;

  /**
   * Constructor
   * @param[in] az azimuth in radians
   * @param[in] el elevation in radians
   * @param[in] rnge range in kilometers
   * @param[in] rnge_rate range rate in kilometers per second
   */
  CoordTopocentric(double az, double el, double rnge, double rnge_rate)
      : m_azimuth(az), m_elevation(el), m_range(rnge), m_range_rate(rnge_rate) {
  }

  /**
   * Copy constructor
   * @param[in] topo object to copy from
   */
  CoordTopocentric(const CoordTopocentric &topo) {
    m_azimuth = topo.m_azimuth;
    m_elevation = topo.m_elevation;
    m_range = topo.m_range;
    m_range_rate = topo.m_range_rate;
  }

  /**
   * Assignment operator
   * @param[in] topo object to copy from
   */
  CoordTopocentric &operator=(const CoordTopocentric &topo) {
    if (this != &topo) {
      m_azimuth = topo.m_azimuth;
      m_elevation = topo.m_elevation;
      m_range = topo.m_range;
      m_range_rate = topo.m_range_rate;
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
    ss << "Az: " << std::setw(8) << Util::RadiansToDegrees(m_azimuth);
    ss << ", El: " << std::setw(8) << Util::RadiansToDegrees(m_elevation);
    ss << ", Rng: " << std::setw(10) << m_range;
    ss << ", Rng Rt: " << std::setw(7) << m_range_rate;
    return ss.str();
  }

  double azimuth() const { return Util::RadiansToDegrees(m_azimuth); }
  double elevation() const { return Util::RadiansToDegrees(m_elevation); }
  double range() const { return m_range; }
  double range_rate() const { return m_range_rate; }

  /** azimuth in radians */
  double m_azimuth{};
  /** elevations in radians */
  double m_elevation{};
  /** range in kilometers */
  double m_range{};
  /** range rate in kilometers per second */
  double m_range_rate{};
};

inline std::ostream &operator<<(std::ostream &strm, const CoordTopocentric &t) {
  return strm << t.ToString();
}

} // namespace libsgp4
