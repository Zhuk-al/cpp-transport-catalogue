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
    
typedef  std::unordered_map<std::string_view, Stop*> StopMap;
typedef  std::unordered_map<std::string_view, Bus*> BusMap;
typedef  std::unordered_map<std::pair<const Stop*, const  Stop*>, int, DistanceHasher> DistanceMap;
 
class TransportCatalogue {
public:      
    void AddBus(Bus&& bus);
    void AddStop(Stop&& stop);
    void SetDistance(const std::vector<Distance>& distances);
    
    Bus* GetBus(std::string_view bus_name);
    Stop* GetStop(std::string_view stop_name);
    
    BusMap GetBusnameToBus() const;
    StopMap GetStopnameToStop() const;
    
    std::unordered_set<const Bus*> StopGetUniqBuses(Stop* stop);
    std::unordered_set<const Stop*> GetUniqStops(Bus* bus);

    size_t GetDistanceStop(const Stop* start, const Stop* finish);
    size_t GetDistanceBus(Bus* bus);
    double GetDistance(Bus* bus);
    
private:    
    std::deque<Stop> stops_;
    StopMap stopname_to_stop_;
    
    std::deque<Bus> buses_;
    BusMap busname_to_bus_;
    
    DistanceMap distance_to_stop_;
    
};
    
} //end namespace transport_catalogue