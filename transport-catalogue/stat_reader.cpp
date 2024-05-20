#include "stat_reader.h"

#include <ostream>

namespace transport_catalogue {

    void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view stop_name = request.substr(request.find_first_of(' ') + 1);
        output << transport_catalogue.GetInfo(stop_name, "Stop") << std::endl;
    }

    void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view bus_name = request.substr(request.find_first_of(' ') + 1);
        output << transport_catalogue.GetInfo(bus_name, "Bus") << std::endl;
    }

    void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view request_type = request.substr(0, request.find_first_of(' '));
        if (request_type == "Bus") {
            PrintBusInfo(transport_catalogue, request, output);
        }
        else {
            PrintStopInfo(transport_catalogue, request, output);
        }
    }

}