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
            std::vector<Bus*> buses; // контейнер маршрутов через остановку
        };

        struct Bus {
            std::string bus_name;
            std::vector<Stop*> stops; // контейнер остановок маршрута
        };

        struct BusComparator {
            bool operator()(const Bus* lhs, const Bus* rhs) const {
                return lhs->bus_name < rhs->bus_name;
            }
        };

        void AddStop(const std::string& stop, const Coordinates& coordinates); // добавляем остановку
        void AddBus(const std::string& bus, const std::vector<std::string_view>& stops); // добавляем маршрут

        const Stop* GetStop(std::string_view stop) const; // получаем остановку по названию
        const Bus* GetBus(std::string_view bus) const; // получаем маршрут по названию
        double GetDistance(const Bus* bus) const; // считаем расстояние маршрута
        int GetUniqueStops(const Bus* bus) const; // считаем количество уникальных остановок маршрута    
        std::unordered_set<Bus*> GetBusesFromStop(const Stop* stop) const; // получаем автобусы проходящие через заданную остановку

    private:
        std::deque<Stop> stops_; // контейнер всех остановок
        std::deque<Bus> buses_; // контейнер всех автобусов
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_; // словарь: название остановки - данные остановки
        std::unordered_map<std::string_view, Bus*> busname_to_bus_; // словарь: название автобуса - данные автобуса
    };

}