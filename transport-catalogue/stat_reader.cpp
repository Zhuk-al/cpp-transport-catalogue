#include "stat_reader.h"

#include <iomanip>
#include <ostream>
#include <set>

namespace transport_catalogue {

    void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view stop_name = request.substr(request.find_first_of(' ') + 1);
        const TransportCatalogue::Stop* stop = transport_catalogue.GetStop(stop_name);
        std::unordered_map<const TransportCatalogue::Stop*, std::unordered_set<const TransportCatalogue::Bus*>> buses_from_stop = transport_catalogue.GetBusesFromStop();
        auto it = buses_from_stop.find(stop);
        if (it != buses_from_stop.end()) {
            const std::unordered_set<const TransportCatalogue::Bus*>& buses = it->second;
            if (buses.empty()) {
                output << request << ": no buses" << std::endl;
            }
            else {
                std::set<const TransportCatalogue::Bus*, TransportCatalogue::BusComparator> sorted_buses(buses.begin(), buses.end());
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
            output << request << ": " << bus_stat.bus_all_stops << " stops on route, "
                << bus_stat.bus_unique_stops << " unique stops, " << bus_stat.bus_distance << " route length, "
                << std::setprecision(6) << bus_stat.route_curvature << " curvature" << std::endl;
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