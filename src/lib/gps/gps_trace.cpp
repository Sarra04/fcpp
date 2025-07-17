#include "gps_trace.hpp"

#define EARTH_RADIUS 6371000.0

namespace fcpp {

namespace gps {

gps_trace::gps_trace(const std::string& src_gpx_file, const double ref_lat, const double ref_lon, const time_t ref_time, const device_t owner_uid)
{
    this->owner_uid = owner_uid;
    if (!load_gpx_file(src_gpx_file, ref_lat, ref_lon, ref_time)) {
        throw std::runtime_error("Failed to load GPX file: " + src_gpx_file);
    }
};

bool gps_trace::load_gpx_file(const std::string& src, const double ref_lat, const double ref_lon, const time_t ref_time) {
    std::ifstream file(src);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open GPX file: " + src);
    }

    // Save file to string stream for processing
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    time_t start_time = 0.0;

    try {
        rapidxml::xml_document<> doc;
        doc.parse<0>(&content[0]);

        rapidxml::xml_node<> *gpx_node = doc.first_node("gpx");
        if (!gpx_node) { return false; }

        rapidxml::xml_node<> *trk_node = gpx_node->first_node("trk");
        if (!trk_node) { return false; }

        for (rapidxml::xml_node<>* trkseg_node = trk_node->first_node("trkseg"); 
        trkseg_node; 
        trkseg_node = trkseg_node->next_sibling("trkseg")) {
                
            for (rapidxml::xml_node<>* trkpt_node = trkseg_node->first_node("trkpt"); 
            trkpt_node; 
            trkpt_node = trkpt_node->next_sibling("trkpt")) {

                rapidxml::xml_attribute<>* lat = trkpt_node->first_attribute("lat");
                rapidxml::xml_attribute<>* lon = trkpt_node->first_attribute("lon");
                rapidxml::xml_node<>* time_node = trkpt_node->first_node("time");

                if (lat && lon && time_node) {
                    double gps_lat = std::stod(lat->value());
                    double gps_lon = std::stod(lon->value());

                    vec<2> pos;

                    if (track.empty()) { //first node entered
                        start_time = parse_time_t(time_node->value());
                    }

                    pos = coord_to_meters(gps_lat, gps_lon, ref_lat, ref_lon);
                    
                    trkpt point;
                    point.x = pos[0];
                    point.y = pos[1];
                    point.timestamp = parse_time_t(time_node->value()) - start_time + ref_time;

                    track.push_back(point);
                    print_trkpt(point);
                }
            }
        }

    } catch (const rapidxml::parse_error& e) {
        throw std::runtime_error("Error while parsing GPX file");
    } catch (const std::exception& e) {
        throw std::runtime_error("Exeption while reading GPX file");
    }

    return !track.empty(); // Return true if at least one point was loaded
}

void gps_trace::print_trkpt(trkpt t) {
    std::cout << std::setprecision(3);
    std::cout << "trkpt: {x: " << t.x << " , y:" << t.y << " , timestamp: " << t.timestamp << "}" << std::endl;
}

vec<2> gps_trace::coord_to_meters(double lat, double lon, double ref_lat, double ref_lon) {
    double ref_lat_rad = ref_lat * M_PI / 180.0;
    double d_lat = (lat- ref_lat) * M_PI / 180.0;
    double d_lon = (lon - ref_lon) * M_PI / 180.0;
    
    double x = EARTH_RADIUS * std::cos(ref_lat_rad) * d_lon;
    double y = EARTH_RADIUS * d_lat;
    
    return make_vec(x, y);
}

time_t gps_trace::parse_time_t(const std::string& string) {
    struct tm tm = {};
    int year, month, day, hour, minute, second;
    
    sscanf(string.c_str(), "%d-%d-%dT%d:%d:%d", 
           &year, &month, &day, &hour, &minute, &second);
    
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    
    return mktime(&tm);
}

gps_trace::trkpt gps_trace::next_point(fcpp::times_t time) {
    int i = 0;
    
    while(time > track[i].timestamp && i < track.size()) {
        i++;
    }

    if(i < track.size()) {
        return track[i];
    } else {
        return track[track.size() - 1];
    }
}

}
}