
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
     * @param uid Uid of the node that will follow the track.
     */
    gps_trace(const std::string &src_gpx_file, const double ref_lat, const double ref_lon, const times_t ref_time);

    /**
     *! @brief conversion of a geographic coordinates to projected coordinates using equirectangular projection
     * @param lat latitude value to convert
     * @param lon longitude value to convert
     * @param ref_lat reference latitude mapped at in x:0
     * @param ref_lon reference longitude mapped at in y:0
     */
    vec<2> coord_to_meters(double lat, double lon, double ref_lat, double ref_lon); // private static

    /**
     *! @brief convert time string from gpx file into time_t value
     * @param string timestamp given in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
     */
    time_t parse_time_t(const std::string &string); // private static

    /**
     *! @brief get the next track point to follow based on the given timestamp
     * @param time current time
     */
    track_point next_point(track_data& td, fcpp::times_t time);


    /**
     *! @brief aggregate function to follow the GPS track assigned to the calling node
     */
    template <typename node_t>
    bool follow_track(node_t &node, trace_t call_point) {
        auto t = tracks.find(node.uid);

        if (t == tracks.end()) { return false; } //track not found for current node

        track_data& td = t->second;

        if (td.index >= td.track.size() - 1) { return false; } //end of track

        track_point target = next_point(td, node.current_time());

        vec<2> direction = make_vec(target.x, target.y) - node.position();

        double distance = std::sqrt(std::pow(direction[0], 2) + std::pow(direction[1], 2));

        // normalize direction vector
        direction /= distance;

        double time_left = target.timestamp - node.current_time();
        double next_time_step = node.next_time() - node.current_time();

        double velocity;
        if (time_left > next_time_step)
        {
            velocity = distance / time_left;
        }
        else
        {
            velocity = distance / next_time_step;
        }

        node.velocity() = direction * velocity;

        return true;
    }

    //! @brief returns the number of saved GPS tracks in the object
    size_t size() const {
        return tracks.size();
    }

private:
    std::unordered_map<device_t, track_data> tracks;
};


template <typename O>
inline O& operator<<(O& os, gps_trace const& trace) {
    os << "gps_trace(tracks=" << trace.size() << ")";
    return os;
}


} // namespace fcpp

#endif // GPS_TRACE_HPP