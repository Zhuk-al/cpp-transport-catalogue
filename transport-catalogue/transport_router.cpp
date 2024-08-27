#include "transport_router.h"
 
namespace transport_catalogue {
namespace detail {
namespace router {
 
TransportRouter::TransportRouter(const RoutingSettings& routing_settings, TransportCatalogue& transport_catalogue)
    : routing_settings_(routing_settings) {
    BuildRouter(transport_catalogue);
}
 
void TransportRouter::BuildRouter(TransportCatalogue& transport_catalogue) {
    SetGraph(transport_catalogue);
    router_ptr_ = std::make_unique<Router<double>>(*graph_ptr_);
    router_ptr_->Build();
}
 
std::optional<RouteInfo> TransportRouter::GetRouteInfo(Stop* stop_from, Stop* stop_to) const {

    if (!stop_from || !stop_to) {
        return std::nullopt;
    }

    VertexId from_vertex = stop_to_router_.at(stop_from).bus_wait_start;
    VertexId to_vertex = stop_to_router_.at(stop_to).bus_wait_end;

    auto route_info = router_ptr_->BuildRoute(from_vertex, to_vertex);

    if (!route_info) {
        return std::nullopt;
    }

    RouteInfo result;
    result.total_time = route_info->weight;

    for (const auto edge_id : route_info->edges) {
        const auto& edge = edge_id_to_edge_.at(edge_id);
        result.edges.emplace_back(edge);
    }

    return result;
}
    
std::deque<Stop*> TransportRouter::GetStopsPtr(TransportCatalogue& transport_catalogue) {
    std::deque<Stop*> stops_ptr;  
    
    for (auto [_, stop_ptr] : transport_catalogue.GetStopnameToStop()) {
        stops_ptr.push_back(stop_ptr);
    }
 
    return stops_ptr;
}
    
std::deque<Bus*> TransportRouter::GetBusPtr(TransportCatalogue& transport_catalogue) {
    std::deque<Bus*> buses_ptr;  
    
    for (auto [_, bus_ptr] : transport_catalogue.GetBusnameToBus()) {
        buses_ptr.push_back(bus_ptr);
    }
 
    return buses_ptr;
}

void TransportRouter::SetGraph(TransportCatalogue& transport_catalogue) {
    const auto stops_ptr_size = GetStopsPtr(transport_catalogue).size();

    graph_ptr_ = std::make_unique<DirectedWeightedGraph<double>>(2 * stops_ptr_size);

    SetStops(GetStopsPtr(transport_catalogue));
    AddEdgeToStop();
    AddEdgeToBus(transport_catalogue);
}
    
void TransportRouter::SetStops(const std::deque<Stop*>& stops) {
    size_t i = 0;
    
    for (const auto stop : stops) {
        VertexId first = i++;
        VertexId second = i++;
        
        stop_to_router_[stop] = RouterByStop{first, second};
    }
}
 
void TransportRouter::AddEdgeToStop() {
    
    for (const auto &[stop, num] : stop_to_router_) {
        EdgeId id = graph_ptr_->AddEdge(Edge<double>{num.bus_wait_start, 
                                                     num.bus_wait_end, 
                                                     routing_settings_.bus_wait_time});
        
        edge_id_to_edge_[id] = StopEdge{stop->name, routing_settings_.bus_wait_time};
    }
}
 
void TransportRouter::AddEdgeToBus(TransportCatalogue& transport_catalogue) {
 
    for (auto bus : GetBusPtr(transport_catalogue)) {
        ParseBusToEdges(bus->stops.begin(),
                        bus->stops.end(), 
                        transport_catalogue,
                        bus);

        if (!bus->is_roundtrip) {
            ParseBusToEdges(bus->stops.rbegin(),
                            bus->stops.rend(), 
                            transport_catalogue,
                            bus);
        }
    }
}
 
Edge<double> TransportRouter::MakeEdgeToBus(Stop* start, Stop* end, const double distance) const {
    Edge<double> result;
    
    result.from = stop_to_router_.at(start).bus_wait_end;
    result.to = stop_to_router_.at(end).bus_wait_start;
    result.weight = distance * 1.0 / (routing_settings_.bus_velocity * 1000 / 60);
    
    return result;
}
 
} //end namespace router
} //end namespace detail
} //end namespace transport_catalogue