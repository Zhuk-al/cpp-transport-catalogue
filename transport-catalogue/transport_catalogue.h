#pragma once

#include <deque>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
 
#include "domain.h"
 
using namespace domain;
 
namespace transport_catalogue {   
 
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
 
class TransportCatalogue {
public:      
    void AddBus(const Bus& bus);
    void AddStop(const Stop& stop);

    void SetDistance(const Stop* stop_from, const Stop* stop_to, int distances);
    
    Bus* GetBus(std::string_view bus_name);
    Stop* GetStop(std::string_view stop_name);
    
    std::unordered_map<std::string_view, Bus*> GetBusnameToBus() const;
    std::unordered_map<std::string_view, Stop*> GetStopnameToStop() const;
    
    std::unordered_set<const Bus*> StopGetUniqBuses(const Stop* stop);
    std::unordered_set<const Stop*> GetUniqStops(const Bus* bus);

    size_t GetDistanceStop(const Stop* start, const Stop* finish) const;
    size_t GetDistanceBus(const Bus* bus);
    double GetDistance(const Bus* bus);
    
private:    
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    
    std::unordered_map<std::pair<const Stop*, const  Stop*>, int, DistanceHasher> distance_to_stop_;
    
};
    
} //end namespace transport_catalogue