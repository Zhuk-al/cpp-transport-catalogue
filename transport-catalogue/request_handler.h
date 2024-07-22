#pragma once
 
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"
 
using namespace transport_catalogue;
using namespace map_renderer;
using namespace transport_catalogue::detail::json;
 
namespace request_handler {
 
class RequestHandler {
public:
    
    std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue& catalogue_) const;
    std::vector<std::string_view> GetSortBusesNames(TransportCatalogue& catalogue_) const;
    
    RequestHandler() = default;
    
    BusQueryInfo BusQuery(TransportCatalogue& catalogue, std::string_view str);
    StopQueryInfo StopQuery(TransportCatalogue& catalogue, std::string_view stop_name);
    
    void ExecuteRenderMap(MapRenderer& map_catalogue, TransportCatalogue& catalogue_) const;
      
};
    
} //end namespace request_handler