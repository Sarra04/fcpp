#include "gtest/gtest.h"
#include "lib/data/gps_trace.hpp"
#include <string>


using namespace fcpp;

TEST(GpsTraceTest, Parsing) {
    std::string gpx_data = std::string(R"(<?xml version="1.0"?>
<gpx>
  <trk>
    <trkseg>
      <trkpt lat="47.86675" lon="15.16357">
        <ele>833.15</ele>
        <time>2018-05-21T04:51:34Z</time>
      </trkpt>
      <trkpt lat="47.86712" lon="15.16338">
        <ele>833.27</ele>
        <time>2018-05-21T04:52:14Z</time>
      </trkpt>
      <trkpt lat="47.86772" lon="15.16324">
        <ele>834.12</ele>
        <time>2018-05-21T04:52:56Z</time>
      </trkpt>
    </trkseg>
  </trk>
  <trk>
    <trkseg>
      <trkpt lat="47.86800" lon="15.16400">
        <time>2018-05-21T05:51:34Z</time>
      </trkpt>
      <trkpt lat="47.86850" lon="15.16450">
        <time>2018-05-21T05:52:14Z</time>
      </trkpt>
    </trkseg>
  </trk>
</gpx>)");

    static gps_trace trace(gpx_data, 47.86675, 15.16357, 10);

    EXPECT_EQ(trace.size(), 2);

    track_data& td_a = trace.find_track(0);
    track_data& td_b = trace.find_track(1);

    ASSERT_FALSE(td_a.track.empty());
    ASSERT_FALSE(td_b.track.empty());
    EXPECT_EQ(td_a.track.size(), 3);
    EXPECT_EQ(td_b.track.size(), 2);
    EXPECT_EQ(td_a.index, 0);
    EXPECT_EQ(td_b.index, 0);

    EXPECT_EQ(td_a.track[0].x, 0);
    EXPECT_EQ(td_a.track[0].y, 0);
    
    EXPECT_GT(td_a.track[1].y, 0.0);
    EXPECT_LT(td_a.track[1].x, 0.0);
    EXPECT_GT(td_a.track[1].timestamp, td_a.track[0].timestamp);
}