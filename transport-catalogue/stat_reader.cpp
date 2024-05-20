#include "stat_reader.h"

#include <iomanip>
#include <ostream>
#include <set>

namespace transport_catalogue {

    void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view stop_name = request.substr(request.find_first_of(' ') + 1);
        const TransportCatalogue::Stop* stop = transport_catalogue.GetStop(stop_name);
        if (stop) {
            if (stop->buses.empty()) {
                output << request << ": no buses" << std::endl;
            }
            else {
                std::set<const TransportCatalogue::Bus*, TransportCatalogue::BusComparator> sorted_buses(stop->buses.begin(), stop->buses.end());
                output << request << ": buses ";
                for (const TransportCatalogue::Bus* bus : sorted_buses) {
                    output << bus->bus_name << " ";
                }
                output << std::endl;
            }
        }
        else {
            output << request << ": not found" << std::endl;
        }
    }

    void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        auto* bus = transport_catalogue.GetBus(request.substr(request.find_first_of(' ') + 1));
        if (bus) {
            auto bus_stat = transport_catalogue.GetBusInfo(bus);
            output << request << ": " << bus->stops.size() << " stops on route, "
                << bus_stat.bus_unique_stops << " unique stops, " << std::setprecision(6)
                << bus_stat.bus_distance << " route length" << std::endl;
        }
        else {
            output << request << ": not found" << std::endl;
        }
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