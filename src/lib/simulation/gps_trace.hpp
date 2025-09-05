
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
#include <algorithm>

#include "lib/data/vec.hpp"
#include "lib/component/timer.hpp"
#include "lib/internal/trace.hpp"

#include "../../external/rapidxml-1.13/rapidxml.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {
/**
 * @brief Class handling
 */
class gps_trace {
    public: // visible by net objects and the main program
        struct track_point
        {
            double x;
            double y;
            time_t timestamp;

            //todo: trkpt can also have elevation from <ele> child
        };

        
        /**
         * @brief Main constructor.
         * 
         * @param src_gpx_file The src of the gpx file to load
         * @param ref_lat Reference latitude to be mapped in x:0
         * @param ref_lon Reference longitude to be mapped in y:0
         * @param ref_time Time offset for track timestamps.
         * @param uid Uid of the node that will follow the track.
         */
        gps_trace(const std::string& src_gpx_file, const double ref_lat, const double ref_lon, const time_t ref_time, const device_t uid);


        /**
         * @brief Load a gpx file
         * 
         * @param src The src of the gpx file to load
         * @param ref_lat Reference latitude to be mapped in x:0
         * @param ref_lon Reference longitude to be mapped in y:0
         * @param ref_time Time offset for track timestamps.
         */
        bool load_gpx_file(const std::string& src, const double ref_lat, const double ref_lon, const time_t ref_time);


        /**
         * @brief print a track_point lat and lon in the console
         * @param t the track_point to be printed
         */
        void print_track_point(track_point t);


        /**
         * @brief conversion of a geographic coordinates to projected coordinates using equirectangular projection
         * @param lat latitude value to convert
         * @param lon longitude value to convert
         * @param ref_lat reference latitude mapped at in x:0
         * @param ref_lon reference longitude mapped at in y:0
         */
        vec<2> coord_to_meters(double lat, double lon, double ref_lat, double ref_lon);

        /**
         * @brief convert time string from gpx file into time_t value
         * @param string timestamp given in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
         */
        time_t parse_time_t(const std::string& string);

        /**
         * @brief get the next track point to follow based on the given timestamp
         * @param time current time
         */
        trkpt next_point(fcpp::times_t time, device_t node_uid);

        template <typename node_t>
        void follow_track(node_t& node, trace_t call_point) {
            track_point target = next_point(node.current_time(), node.uid);


            vec<2> direction = make_vec(target.x, target.y) - node.position();

            //calculate magnitude (distance)
            double distance = std::sqrt(std::pow(direction[0], 2) + std::pow(direction[1], 2));

            if(distance == 0) {
                node.velocity() = make_vec(0.0, 0.0);
                return;
            }

            //normalize direction vector
            direction /= distance; 

            double time_left = target.timestamp - node.current_time();
            double next_time_step = node.next_time() - node.current_time();

            double velocity;
            if(time_left > next_time_step) {
                velocity = distance / time_left;
            } else {
                velocity = distance / next_time_step;
            }

            node.velocity() = direction * velocity;
        }
    private:
        std::map<device_t, std::vector<track_point>> tracks;
};
}

#endif // GPS_TRACE_HPP