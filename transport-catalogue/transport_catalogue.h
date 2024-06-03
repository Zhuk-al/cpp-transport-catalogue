#pragma once
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "geo.h"

namespace transport_catalogue {
    using namespace geo;

    class TransportCatalogue {

    public:
        struct Bus;

        struct Stop {
            std::string stop_name;
            Coordinates coordinates;
        };

        struct Bus {
            std::string bus_name;
            std::vector<Stop*> stops;
        };

        struct Distance {
            const Stop* A;
            const Stop* B;
            int distance;
        };

        struct BusStat {
            int bus_all_stops;
            int bus_unique_stops;
            size_t bus_distance;
            double route_curvature;
        };

        struct BusComparator {
            bool operator()(const Bus* lhs, const Bus* rhs) const {
                return lhs->bus_name < rhs->bus_name;
            }
        };

        class DistanceHasher {
        public:
            std::size_t operator()(const std::pair<const Stop*, const Stop*> stops) const noexcept {
                auto hash_1 = static_cast<const void*>(stops.first);
                auto hash_2 = static_cast<const void*>(stops.second);
                return hasher_(hash_1) * 24 + hasher_(hash_2);
            }
        private:
            std::hash<const void*> hasher_;
        };

        void AddStop(const std::string& stop, const Coordinates& coordinates);
        void AddBus(const std::string& bus, const std::vector<std::string_view>& stops);
        void SetDistance(std::vector<Distance> distances);

        const Stop* GetStop(std::string_view stop) const;
        const Bus* GetBus(std::string_view bus) const;

        BusStat GetBusInfo(const Bus* bus) const;

        std::unordered_map<const Stop*, std::unordered_set<const Bus*>> GetBusesFromStop() const;

    private:
        std::deque<Stop> stops_; // контейнер всех остановок
        std::deque<Bus> buses_; // контейнер всех автобусов
        
        // словарь: название остановки - данные остановки
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_;

        // словарь: название автобуса - данные автобуса
        std::unordered_map<std::string_view, Bus*> busname_to_bus_;

        // словарь автобусов через остановку
        std::unordered_map<const Stop*, std::unordered_set<const Bus*>> bus_by_stop_;

        // словарь расстояний между остановками
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher> distance_to_stop_;

        double GetDistance(const Bus* bus) const; // географическая длина маршрута
        int GetUniqueStops(const Bus* bus) const; // количество уникальных остановок маршрута 
        size_t GetDistanceStop(const Stop* stop_start, const Stop* stop_finish) const;
        size_t GetDistanceBus(const Bus* bus) const; // фактическая длина маршрута
    };

}