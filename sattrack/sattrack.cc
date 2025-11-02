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

#include "DateTime.h"
#include "TimeSpan.h"
#include <CoordGeodetic.h>
#include <CoordTopocentric.h>
#include <Observer.h>
#include <SGP4.h>

#include <iostream>

int main() {
  // lat/lon/altitude of PIE airport.
  libsgp4::Observer obs(27.9086, -82.6865, 3.0);

  libsgp4::Tle tle = libsgp4::Tle(
      "O3B MPOWER F9",
      "1 64866U 25153A   25281.47605596 -.00000005  00000+0  00000+0 0  9990",
      "2 64866   3.2383 359.8473 0761196 355.0351   4.2328  5.89130413  5392");
  libsgp4::SGP4 sgp4(tle);

  std::cout << tle << std::endl;

  libsgp4::DateTime now = libsgp4::DateTime::Now();
  libsgp4::DateTime epoch = tle.Epoch();
  libsgp4::TimeSpan tsince = now - epoch;

  double tsince_d = tsince.TotalMinutes();

  std::cout << "Minutes since TLE Epoch:(" << tsince_d << ")" << std::endl;

  std::cout << "epoch(" << epoch << ")" << std::endl;
  std::cout << "now(" << now << ")" << std::endl;

  for (int i = 0; i < 10; ++i) {
    libsgp4::DateTime dt = tle.Epoch().AddMinutes((int)tsince_d + (i * 10));
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

    std::cout << dt << " " << topo << " " << geo << std::endl;
  };

  return 0;
}
