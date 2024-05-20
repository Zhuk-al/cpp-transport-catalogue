#include <algorithm>

#include "transport_catalogue.h"

namespace transport_catalogue {
    using namespace geo;

    void TransportCatalogue::AddStop(const std::string& stop, const Coordinates& coordinates) {
        Stop add_stop{ stop, coordinates, {} };
        stops_.push_back(std::move(add_stop));
        Stop* stop_pointer = &stops_.back();
        stopname_to_stop_[stop_pointer->stop_name] = stop_pointer;
    }

    void TransportCatalogue::AddBus(const std::string& bus, const std::vector<std::string_view>& stops) {
        Bus add_bus{ bus, {} };
        for (auto& stop : stops) {
            auto it = stopname_to_stop_.find(stop);
            if (it != stopname_to_stop_.end()) {
                add_bus.stops.push_back(it->second);
            }
        }
        buses_.push_back(std::move(add_bus));
        Bus* bus_pointer = &buses_.back();
        for (auto* stop : bus_pointer->stops) {
            stop->buses.push_back(bus_pointer);
        }
        busname_to_bus_.insert({ bus_pointer->bus_name, bus_pointer });
    }

    const TransportCatalogue::Stop* TransportCatalogue::GetStop(std::string_view stop) const {
        auto it = stopname_to_stop_.find(stop);
        return it != stopname_to_stop_.end() ? it->second : nullptr;
    }

    const TransportCatalogue::Bus* TransportCatalogue::GetBus(std::string_view name) const {
        auto it = busname_to_bus_.find(name);
        return it != busname_to_bus_.end() ? it->second : nullptr;
    }

    TransportCatalogue::BusStat TransportCatalogue::GetBusInfo(const Bus* bus) const {
        auto bus_distance = GetDistance(bus);
        auto bus_unique_stops = GetUniqueStops(bus);
        return { bus_distance, bus_unique_stops };
    }

    double TransportCatalogue::GetDistance(const Bus* bus) const {
        double distance = 0.0;
        const auto& stops = bus->stops;
        for (size_t i = 0; i < stops.size() - 1; ++i) {
            distance += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
        }
        return distance;
    }

    int TransportCatalogue::GetUniqueStops(const Bus* bus) const {
        std::unordered_set<const Stop*> stops;
        stops.insert(bus->stops.begin(), bus->stops.end());
        return stops.size();
    }

}