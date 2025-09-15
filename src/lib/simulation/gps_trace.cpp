#include "gps_trace.hpp"

#define EARTH_RADIUS 6371000.0

namespace fcpp
{

    gps_trace::gps_trace(const std::string &src_gpx_file, const double ref_lat, const double ref_lon, const time_t ref_time)
    {
        std::ifstream file(src_gpx_file);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open GPX file: " + src_gpx_file);
        }

        // Save file to string stream for processing
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();

        time_t start_time = 0.0;

        try
        {
            rapidxml::xml_document<> doc;
            doc.parse<0>(&content[0]);

            rapidxml::xml_node<> *gpx_node = doc.first_node("gpx");
            if (!gpx_node)
            {
                throw std::runtime_error("Failed to find gpx node in file: " + src_gpx_file);
            }

            
            device_t track_id = 0;
            for (rapidxml::xml_node<> *trk_node = gpx_node->first_node("trk");
                 trk_node;
                 trk_node = trk_node->next_sibling("trk"))
            {
                auto track = tracks[track_id];

                for (rapidxml::xml_node<> *trkseg_node = trk_node->first_node("trkseg");
                     trkseg_node;
                     trkseg_node = trkseg_node->next_sibling("trkseg"))
                {

                    for (rapidxml::xml_node<> *trkpt_node = trkseg_node->first_node("trkpt");
                         trkpt_node;
                         trkpt_node = trkpt_node->next_sibling("trkpt"))
                    {

                        rapidxml::xml_attribute<> *lat = trkpt_node->first_attribute("lat");
                        rapidxml::xml_attribute<> *lon = trkpt_node->first_attribute("lon");
                        rapidxml::xml_node<> *time_node = trkpt_node->first_node("time");

                        if (lat && lon && time_node)
                        {
                            double gps_lat = std::stod(lat->value());
                            double gps_lon = std::stod(lon->value());

                            vec<2> pos;

                            if (track.empty())
                            { // first node entered
                                start_time = parse_time_t(time_node->value());
                            }

                            pos = coord_to_meters(gps_lat, gps_lon, ref_lat, ref_lon);

                            track_point point;
                            point.x = pos[0];
                            point.y = pos[1];
                            point.timestamp = parse_time_t(time_node->value()) - start_time + ref_time;

                            track.push_back(point);

                            // print_track_point(point);
                        }
                    }
                }

                std::reverse(track.begin(), track.end());
                track_id++;
            }
        }
        catch (const rapidxml::parse_error &e)
        {
            throw std::runtime_error("Error while parsing GPX file");
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Exeption while reading GPX file");
        }
    };

    void gps_trace::print_track_point(track_point t)
    {
        std::cout << std::setprecision(3);
        std::cout << "track_point: {x: " << t.x << " , y:" << t.y << " , timestamp: " << t.timestamp << "}" << std::endl;
    }

    vec<2> gps_trace::coord_to_meters(double lat, double lon, double ref_lat, double ref_lon)
    {
        double ref_lat_rad = ref_lat * M_PI / 180.0;
        double d_lat = (lat - ref_lat) * M_PI / 180.0;
        double d_lon = (lon - ref_lon) * M_PI / 180.0;

        double x = EARTH_RADIUS * std::cos(ref_lat_rad) * d_lon;
        double y = EARTH_RADIUS * d_lat;

        return make_vec(x, y);
    }

    time_t gps_trace::parse_time_t(const std::string &string)
    {
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

    gps_trace::track_point gps_trace::next_point(std::vector<track_point> track, fcpp::times_t time)
    {

        if(tracks.empty()) {
            throw std::runtime_error("Given GPS track is empty.");
        }

        while (track.size() > 1)
        {
            auto t = track.back();

            if (t.timestamp < time)
            {
                track.pop_back();
            }
            else
            {
                return t;
            }
        }

        // In case there's only one track_point remaining keep returning it
        return track[0];
    }

}
