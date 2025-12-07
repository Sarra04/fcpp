// Copyright © 2025 Lorenzo Framarin and Giorgio Audrito. All Rights Reserved.

#include "gps_trace.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


static constexpr real_t EARTH_RADIUS = 6371000.0;

static track_data s_empty_track{{}, -1};


gps_trace::gps_trace(const char* src_gpx_file, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time) {
    std::ifstream file(src_gpx_file);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open GPX file: ") + src_gpx_file);
    }

    std::string xml((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    this->init(xml, ref_lat, ref_lon, ref_ele, ref_time);
}

gps_trace::gps_trace(std::string &xml, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time) {
    this->init(xml, ref_lat, ref_lon, ref_ele, ref_time);
}

void gps_trace::init(std::string &xml, const real_t ref_lat, const real_t ref_lon, const real_t ref_ele, const times_t ref_time) {
    times_t start_time = 0.0;
    device_t track_id = 0;
    vec<2> pos;

    try {
        rapidxml::xml_document<> doc;
        doc.parse<0>(&xml[0]);
        rapidxml::xml_node<> *gpx_node = doc.first_node("gpx");
        if (!gpx_node) throw std::runtime_error("Failed to find gpx node");

        for (rapidxml::xml_node<> *trk_node = gpx_node->first_node("trk"); trk_node; trk_node = trk_node->next_sibling("trk")) {
            
            std::vector<track_point> track = {};

            for (rapidxml::xml_node<> *trkseg_node = trk_node->first_node("trkseg"); trkseg_node; trkseg_node = trkseg_node->next_sibling("trkseg")) {

                for (rapidxml::xml_node<> *trkpt_node = trkseg_node->first_node("trkpt"); trkpt_node; trkpt_node = trkpt_node->next_sibling("trkpt")) {

                    rapidxml::xml_attribute<> *lat = trkpt_node->first_attribute("lat");
                    rapidxml::xml_attribute<> *lon = trkpt_node->first_attribute("lon");
                    rapidxml::xml_node<> *ele_node = trkpt_node->first_node("ele");
                    rapidxml::xml_node<> *time_node = trkpt_node->first_node("time");

                    if (lat && lon && time_node) {
                        real_t gps_lat = std::stod(lat->value());
                        real_t gps_lon = std::stod(lon->value());

                        if (track.empty()) start_time = static_cast<times_t>(parse_time_t(time_node->value())); // first node entered

                        pos = coord_to_meters(gps_lat, gps_lon, ref_lat, ref_lon);

                        track_point point;
                        point.x = pos[0];
                        point.y = pos[1];
                        point.timestamp = static_cast<times_t>(parse_time_t(time_node->value())) - start_time + ref_time;
                        ele_node ? point.z = std::stod(ele_node->value()) - ref_ele : point.z = 0.0;
                        
                        track.push_back(point);
                    }
                }
            }

            m_tracks[track_id] = {track, 0};
            track_id++;
        }
    }
    catch (const rapidxml::parse_error &e) {
        throw std::runtime_error("Error while parsing GPX file");
    }
    catch (const std::exception &e) {
        throw std::runtime_error("Exeption while reading GPX file");
    }
};

vec<2> gps_trace::coord_to_meters(real_t lat, real_t lon, real_t ref_lat, real_t ref_lon) {
    real_t ref_lat_rad = ref_lat * M_PI / 180;
    real_t d_lat = (lat - ref_lat) * M_PI / 180;
    real_t d_lon = (lon - ref_lon) * M_PI / 180;

    real_t x = EARTH_RADIUS * std::cos(ref_lat_rad) * d_lon;
    real_t y = EARTH_RADIUS * d_lat;

    return make_vec(x, y);
}

time_t gps_trace::parse_time_t(const std::string &string) {
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

track_point& gps_trace::next_point(track_data& td, fcpp::times_t time) {
    track_point& tp = td.track[td.index];

    while(tp.timestamp < time) {
        td.index += 1;
        if(td.index >= td.track.size()) return tp;
        tp = td.track[td.index];
    }

    return tp;
}

track_data& gps_trace::find_track(device_t uid) {
    auto it = m_tracks.find(uid);
    if (it != m_tracks.end()) return it->second;
    return s_empty_track;
}


} // namespace fcpp
