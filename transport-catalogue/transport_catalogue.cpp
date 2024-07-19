#include "transport_catalogue.h"
 
namespace transport_catalogue {  
    
void TransportCatalogue::AddStop(Stop&& stop) {
    stops_.push_back(std::move(stop));
    Stop* stop_pointer = &stops_.back();
    stopname_to_stop_[stop_pointer->name] = stop_pointer;
}
 
void TransportCatalogue::AddBus(Bus&& bus) {
    Bus* bus_pointer;
    
    buses_.push_back(std::move(bus)); 
    bus_pointer = &buses_.back();
    busname_to_bus_.insert({ bus_pointer->name, bus_pointer });
 
    for (Stop* stop : bus_pointer->stops) {
         stop->buses.push_back(bus_pointer);
    }
    
    bus_pointer->route_length = GetDistanceBus(bus_pointer);
}
 
void TransportCatalogue::SetDistance(const std::vector<Distance>& distances) {
    for (auto distance : distances){
        auto dist_pair = std::make_pair(distance.stop_from, distance.stop_to);
        distance_to_stop_.insert(DistanceMap::value_type(dist_pair, distance.distance));
    }
}
 
Bus* TransportCatalogue::GetBus(std::string_view bus) {
    auto it = busname_to_bus_.find(bus);
    return it != busname_to_bus_.end() ? it->second : nullptr;
}
 
Stop* TransportCatalogue::GetStop(std::string_view stop) {
    auto it = stopname_to_stop_.find(stop);
    return it != stopname_to_stop_.end() ? it->second : nullptr;
}
    
BusMap TransportCatalogue::GetBusnameToBus() const {
    return busname_to_bus_;
}
    
StopMap TransportCatalogue::GetStopnameToStop() const {
    return stopname_to_stop_;
}
 
std::unordered_set<const Stop*> TransportCatalogue::GetUniqStops(Bus* bus) {
    std::unordered_set<const Stop*> stops;   
    stops.insert(bus->stops.begin(), bus->stops.end());    
    return stops;
}
 
double TransportCatalogue::GetDistance(Bus* bus) {
    double distance = 0.0;
    const auto& stops = bus->stops;
    for (size_t i = 1; i < stops.size(); ++i) {
        distance += geo::ComputeDistance({ stops[i - 1]->latitude, stops[i - 1]->longitude }, 
                                         { stops[i]->latitude, stops[i]->longitude } );
    }
    return distance;
}
 
std::unordered_set<const Bus*> TransportCatalogue::StopGetUniqBuses(Stop* stop){
    std::unordered_set<const Bus*> unique_stops;        
    unique_stops.insert(stop->buses.begin(), stop->buses.end());   
    return unique_stops;
}
 
size_t TransportCatalogue::GetDistanceStop(const Stop* stop_start, const Stop* stop_finish) {
    auto it = distance_to_stop_.find(std::make_pair(stop_start, stop_finish));
    if (it == distance_to_stop_.end()) {
        it = distance_to_stop_.find(std::make_pair(stop_finish, stop_start));
    }
    return it->second;
}
 
size_t TransportCatalogue::GetDistanceBus(Bus* bus) {
    size_t distance = 0;
    const auto& stops = bus->stops;
    for (size_t i = 1; i < stops.size(); ++i) {
        distance += GetDistanceStop(bus->stops[i - 1], bus->stops[i]);
    }
    return distance;
}
    
} //end namespace transport_catalogue