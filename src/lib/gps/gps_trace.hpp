
#ifndef GPS_TRACE_HPP
#define GPS_TRACE_HPP

#define _USE_MATH_DEFINES
#include <cmath>

#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>

#include "lib/data/vec.hpp"
#include "lib/internal/trace.hpp"

#include "../../external/rapidxml-1.13/rapidxml.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

namespace gps {
/**
 * @brief Class handling
 */
class gps_trace {
    public: // visible by net objects and the main program
        struct trkpt
        {
            double x;
            double y;
            time_t timestamp;

            //todo: add timestamp management
            //todo: trkpt can also have elevation from <ele> child
        };

        
        /**
         * @brief Main constructor.
         * 
         * @param src_gpx_file The src of the gpx file to load
         * @param origin The origin of the gps track expressed in meters from poin (0, 0) where all track points will be mapped from.
         * @param start_time Time offset from beginning of simulation expressed in seconds to map track points timestamps from.
         */
        gps_trace(const std::string& src_gpx_file, const vec<2> origin, const time_t start_time);


        /**
         * @brief Load and read a gpx file
         * 
         * @param src The src of the gpx file to load
         */
        bool load_gpx_file(const std::string& src);


        /**
         * @brief print a trkpt lat and lon in the console
         */
        void print_trkpt(trkpt t);


        /**
         * @brief Haversine formula to convert lat/lon to meters
         */
        vec<2> coord_to_meters(double lat, double lon, double ref_lat, double ref_lon);

        /**
         * @brief convert time string from gpx file into time_t value
         */
        time_t parse_time_t(const std::string& iso_string);

        template <typename node_t>
        void follow_track(node_t& node, trace_t call_point) {
            node.velocity() = make_vec(3, 0.0);
        }

    private:
        std::vector<trkpt> track;
        vec<2> origin;
        time_t start_time;
};

}
}

#endif // GPS_TRACE_HPP