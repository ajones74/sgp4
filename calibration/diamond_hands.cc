#include <cstdint>

// The idea is this:
// 1) Move the positioner to the platonic look-angle at time=t0
// 2) Move the positioner to t0+15 seconds.
// 3) Lock in azimuth and commence a quick elevation scan, plus/minus 2 degrees.
//    a) Capture the recorded RSSI values in a matrix, where RSSI rises, and
//    then falls --
//       treat as a derivative: positive slope, followed by negative slope
//    b) at the end of 30 seconds ( +15 to -15 of t0 ), find the ideal
//    elevation:
// 4) move to the postioner to time=t1  (current time + 10 seconds ), locking in
// the elevation 5) After the elevation is locked, commence a quick azimuth
// scan, plus/minus 2 degrees.
//   a) Capture the recorded RSSI values as a vector and find the peak value --
//   it should be close
//      to the peak value discovered in step (3a) -- once a peak value is found.
//      STOP SCANNING!
// 6) At this point, the accuracy should be around 0.5 degrees of accuracy.
// 7) Move to the current-time + 10 seconds, and perform a circular scan (or
// "diamond scan"), quickly, with a radius of 0.1 to 0.5 degrees
//    a) The diamond-scan captures max and min elevation, max and min azimuth.
// 8) Move to the current-time + 10 seconds, and repeat the
// circular/diamond-scan, again.
//    a) Capture max and min elevation, max and min azimuth.
// 9) Repeat step 8, 8a again. At this point, there should be a collection of 12
// points ( 3 diamonds, each diamond is 4 points. ) 10) There's now enough info
// to extrapolate the best elevation, the best azimuth.
//
//
