// Copyright © 2025 Lorenzo Framarin and Giorgio Audrito. All Rights Reserved.

/**
 * @file gps_trace.hpp
 * @brief Implementation and helper functions for the `gps_trace` class handling a collection of GPS traces.
 */

#ifndef FCPP_DATA_GPS_TRACE_HPP
#define FCPP_DATA_GPS_TRACE_HPP

#define _USE_MATH_DEFINES
#include <cmath>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ostream>
#include <unordered_map>
#include <ctime>
#include <cmath>
#include <algorithm>

#include "lib/data/vec.hpp"

#include "../../external/rapidxml-1.13/rapidxml.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief A GPS track point.
struct track_point {
    //! @brief x coordinate.
    real_t x;
    //! @brief y coordinate.
    real_t y;
    //! @brief z coordinate.
    real_t z;
    //! @brief time coordinate.
    times_t timestamp;
};

//! @brief GPS track and index for navigation.
struct track_data {
    //! @brief Time-ordered sequence of track points.
    std::vector<track_point> track;
    //! @brief Index of the current track point.
    int index;
};

/**
 * @brief Class handling a collection of GPS traces.
 */
class gps_trace {
  public: // visible by net objects and the main program
    //! @brief Default constructor.
    gps_trace() = default;

    /**
     * @brief Main constructor.
     * @param src_gpx_file path of the gpx file to load.
     * @param ref_lat reference latitude to be mapped in x = 0.
     * @param ref_lon reference longitude to be mapped in y = 0.
     * @param ref_ele reference elevation to be mapped in z = 0.
     * @param ref_time time offset for track timestamps.
     */
    gps_trace(const char* src_gpx_file, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time);

    /**
     * @brief Constructor with direct XML string.
     * @param xml zero-terminated XML string.
     * @param ref_lat reference latitude to be mapped in x = 0.
     * @param ref_lon reference longitude to be mapped in y = 0.
     * @param ref_ele reference elevation to be mapped in z = 0.
     * @param ref_time time offset for track timestamps.
     */
    gps_trace(std::string &xml, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time);

    /**
     * @brief Next track point to follow based on the given timestamp.
     * @param td track_data struct with the GPS trace to search.
     * @param time current time.
     */
    track_point& next_point(track_data& td, fcpp::times_t time);

    //! @brief GPS trace associated with the given device id.
    track_data& find_track(device_t uid);

    //! @brief Number of saved GPS tracks in the object.
    size_t size() const {
        return m_tracks.size();
    }

  private:
    //! @brief The stored GPS tracks.
    std::unordered_map<device_t, track_data> m_tracks;

    /**
     * @brief Initialization method, called by the constructors to parse an XML string.
     * @param xml zero-terminated XML string.
     * @param ref_lat reference latitude to be mapped in x = 0.
     * @param ref_lon reference longitude to be mapped in y = 0.
     * @param ref_ele reference elevation to be mapped in z = 0.
     * @param ref_time time offset for track timestamps.
     */
    void init(std::string &xml, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time);

    /**
     * @brief Conversion of geographic coordinates to projected coordinates using equirectangular projection.
     * @param lat latitude value to convert.
     * @param lon longitude value to convert.
     * @param ref_lat reference latitude mapped to x = 0.
     * @param ref_lon reference longitude mapped to y = 0.
     */
    static vec<2> coord_to_meters(real_t lat, real_t lon, real_t ref_lat, real_t ref_lon);

    /**
     * @brief Converts a time string from gpx file into a `time_t` value.
     * @param string timestamp given in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ).
     */
    static time_t parse_time_t(const std::string &string);
};


//! @brief Prints a `gps_traces` object.
template <typename O>
inline O& operator<<(O& os, gps_trace const& trace) {
    os << "gps_trace(" << trace.size() << " tracks)";
    return os;
}


} // namespace fcpp


#endif // FCPP_DATA_GPS_TRACE_HPP
