#include <iostream>
#include "gps_trace.hpp"


int main() {
    std::cout << "Starting test for gps_trace" << std::endl;
    fcpp::gps::gps_trace trace("test_data/test.gpx", fcpp::make_vec(0.0, 0.0), 0);
}