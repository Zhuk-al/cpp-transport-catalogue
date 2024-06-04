#include <algorithm>

#include "transport_catalogue.h"

namespace transport_catalogue {
    using namespace geo;

    void TransportCatalogue::AddStop(const std::string& stop, const Coordinates& coordinates) {
        Stop add_stop{ stop, coordinates };
        stops_.push_back(std::move(add_stop));
        Stop* stop_pointer = &stops_.back();
        bus_by_stop_[stop_pointer] = {};
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
            bus_by_stop_[stop].insert(bus_pointer);;
        }
        busname_to_bus_.insert({ bus_pointer->bus_name, bus_pointer });
    }

    void TransportCatalogue::SetDistance(const Stop* stop_from, const Stop* stop_to, int distances) {
        auto stop_pair = std::make_pair(stop_from, stop_to);
        distance_to_stop_.insert({ stop_pair, distances });
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
        int bus_all_stops = bus->stops.size();
        auto bus_unique_stops = GetUniqueStops(bus);
        auto bus_distance = GetDistanceBus(bus);
        auto route_curvature = static_cast<double>(GetDistanceBus(bus) / GetDistance(bus));
        return { bus_all_stops, bus_unique_stops, bus_distance, route_curvature };
    }

    double TransportCatalogue::GetDistance(const Bus* bus) const {
        double distance = 0.0;
        const auto& stops = bus->stops;
        for (size_t i = 1; i < stops.size(); ++i) {
            distance += ComputeDistance(stops[i - 1]->coordinates, stops[i]->coordinates);
        }
        return distance;
    }

    int TransportCatalogue::GetUniqueStops(const Bus* bus) const {
        std::unordered_set<const Stop*> stops;
        stops.insert(bus->stops.begin(), bus->stops.end());
        return stops.size();
    }

    std::unordered_map<const TransportCatalogue::Stop*, std::unordered_set<const TransportCatalogue::Bus*>> TransportCatalogue::GetBusesFromStop() const {
        return bus_by_stop_;
    }

    size_t TransportCatalogue::GetDistanceStop(const Stop* stop_start, const Stop* stop_finish) const {
        auto it = distance_to_stop_.find(std::make_pair(stop_start, stop_finish));
        if (it == distance_to_stop_.end()) {
            it = distance_to_stop_.find(std::make_pair(stop_finish, stop_start));
        }
        return it->second;
    }

    size_t TransportCatalogue::GetDistanceBus(const Bus* bus) const {
        size_t distance = 0;
        const auto& stops = bus->stops;
        for (size_t i = 1; i < stops.size(); ++i) {
            distance += GetDistanceStop(bus->stops[i - 1], bus->stops[i]);
        }
        return distance;
    }

}