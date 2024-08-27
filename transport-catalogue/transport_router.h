#pragma once
 
#include "transport_catalogue.h"
#include "router.h"
#include "domain.h"
 
#include <deque>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <variant>
 
namespace transport_catalogue {
namespace detail {
namespace router {
 
using namespace domain;
using namespace graph;  
 
class TransportRouter {
public:
    TransportRouter(const RoutingSettings& routing_settings, TransportCatalogue& transport_catalogue);

    std::optional<RouteInfo> GetRouteInfo(Stop* stop_from, Stop* stop_to) const;
    
private:
    void BuildRouter(TransportCatalogue& transport_catalogue);

    std::unordered_map<Stop*, RouterByStop> stop_to_router_;
    std::unordered_map<EdgeId, std::variant<StopEdge, BusEdge>> edge_id_to_edge_;
    
    std::unique_ptr<DirectedWeightedGraph<double>> graph_ptr_;
    std::unique_ptr<Router<double>> router_ptr_;
    
    RoutingSettings routing_settings_;

    std::deque<Stop*> GetStopsPtr(TransportCatalogue& transport_catalogue);
    std::deque<Bus*> GetBusPtr(TransportCatalogue& transport_catalogue);

    void AddEdgeToStop();
    void AddEdgeToBus(TransportCatalogue& transport_catalogue);

    void SetStops(const std::deque<Stop*>& stops);
    void SetGraph(TransportCatalogue& transport_catalogue);

    Edge<double> MakeEdgeToBus(Stop* start, Stop* end, const double distance) const;

    template <typename Iterator>
    void ParseBusToEdges(Iterator first,
                         Iterator last,
                         const TransportCatalogue& transport_catalogue,
                         const Bus* bus);
};
 
template <typename Iterator>
void TransportRouter::ParseBusToEdges(Iterator first, 
                                      Iterator last,
                                      const TransportCatalogue& transport_catalogue, 
                                      const Bus* bus) {
    
    for (auto it = first; it != last; ++it) {
        const auto from = *it;
        {
            size_t distance = 0;
            size_t span_count = 0;

            for (auto it2 = std::next(it); it2 != last; ++it2) {
                const auto before_to = *prev(it2);
                const auto to = *it2;
                distance += transport_catalogue.GetDistanceStop(before_to, to);
                ++span_count;

                EdgeId id = graph_ptr_->AddEdge(MakeEdgeToBus(from, to, distance));

                edge_id_to_edge_[id] = BusEdge{ bus->name, span_count, graph_ptr_->GetEdge(id).weight };
            }
        }
    }
}
 
} //end namespace router
} //end namespace detail
} //end namespace transport_catalogue