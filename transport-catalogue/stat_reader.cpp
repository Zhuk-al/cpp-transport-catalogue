#include "stat_reader.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <set>

namespace transport_catalogue {

    void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
        std::string_view request_type = request.substr(0, request.find_first_of(' '));
        if (request_type == "Bus") {
            auto* bus = transport_catalogue.GetBus(request.substr(request.find_first_of(' ') + 1));
            if (bus) {
                output << request << ": " << bus->stops.size() << " stops on route, "
                    << transport_catalogue.GetUniqueStops(bus) << " unique stops, " << std::setprecision(6) << transport_catalogue.GetDistance(bus) << " route length" << std::endl;
            }
            else {
                output << request << ": not found" << std::endl;
            }
        }
        else {
            std::string_view stop_name = request.substr(request.find_first_of(' ') + 1);
            const TransportCatalogue::Stop* stop = transport_catalogue.GetStop(stop_name);
            if (stop) {
                std::unordered_set<TransportCatalogue::Bus*> buses = transport_catalogue.GetBusesFromStop(stop);
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
    }

}