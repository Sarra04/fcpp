
#ifndef GPS_TRACE_HPP
#define GPS_TRACE_HPP

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
#include "lib/component/timer.hpp"
#include "lib/internal/trace.hpp"

#include "../../external/rapidxml-1.13/rapidxml.hpp"

/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp
{

//! @brief struct for storing a GPS track point
struct track_point
{
    double x;
    double y;
    double z;
    times_t timestamp;
};

//! @brief GPS track and index for navigation
struct track_data {
    std::vector<track_point> track;
    int index;
};

/**
 * @brief Class handling
 */
class gps_trace {

public: // visible by net objects and the main program
    gps_trace() = default;

    /**
     *! @brief Main constructor.
     * @param src_gpx_file The src of the gpx file to load
     * @param ref_lat Reference latitude to be mapped in x:0
     * @param ref_lon Reference longitude to be mapped in y:0
     * @param ref_time Time offset for track timestamps.
     */
    gps_trace(const char* src_gpx_file, const double ref_lat, const double ref_lon, const double ref_ele, const times_t ref_time);

    /**
     *! @brief Constructor with direct XML string
     * @param xml zero-terminated XML string
     * @param ref_lat Reference latitude to be mapped in x:0
     * @param ref_lon Reference longitude to be mapped in y:0
     * @param ref_time Time offset for track timestamps.
     */
    gps_trace(std::string &xml, const double ref_lat, const double ref_lon, const double ref_ele, const times_t ref_time);

    /**
     *! @brief get the next track point to follow based on the given timestamp
     * @param td track_data struct with the GPS trace to search
     * @param time current time
     */
    track_point& next_point(track_data& td, fcpp::times_t time);

    /**
     *! @brief returns the GPS trace associated with the device id given
     * @param uid device identifier
     */
    track_data& find_track(device_t uid);

    //! @brief returns the number of saved GPS tracks in the object
    size_t size() const {
        return m_tracks.size();
    }

private:
    std::unordered_map<device_t, track_data> m_tracks;

    /**
     *! @brief Initialization method, called by the constructors to parse an XML string
     * @param xml zero-terminated XML string
     * @param ref_lat Reference latitude to be mapped in x:0
     * @param ref_lon Reference longitude to be mapped in y:0
     * @param ref_time Time offset for track timestamps.
     */
    void init(std::string &xml, const double ref_lat, const double ref_lon, const double ref_ele, const times_t ref_time);

    /**
     *! @brief conversion of a geographic coordinates to projected coordinates using equirectangular projection
     * @param lat latitude value to convert
     * @param lon longitude value to convert
     * @param ref_lat reference latitude mapped at in x:0
     * @param ref_lon reference longitude mapped at in y:0
     */
    static vec<2> coord_to_meters(double lat, double lon, double ref_lat, double ref_lon);

    /**
     *! @brief convert time string from gpx file into time_t value
     * @param string timestamp given in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
     */
    static time_t parse_time_t(const std::string &string);
};


template <typename O>
inline O& operator<<(O& os, gps_trace const& trace) {
    os << "gps_trace(tracks=" << trace.size() << ")";
    return os;
}


} // namespace fcpp

#endif // GPS_TRACE_HPP