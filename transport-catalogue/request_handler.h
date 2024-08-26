#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"
#include "json_builder.h"
#include "transport_router.h"

using namespace transport_catalogue;
using namespace map_renderer;
using namespace transport_catalogue::detail::json;
using namespace transport_catalogue::detail::json::builder;
using namespace transport_catalogue::detail::router;

namespace request_handler {

    class RequestHandler {
    public:
        RequestHandler() = default;

        std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue& catalogue_) const;
        std::vector<std::string_view> GetSortBusesNames(TransportCatalogue& catalogue_) const;

        BusQueryInfo BusQuery(TransportCatalogue& catalogue, std::string_view str);
        StopQueryInfo StopQuery(TransportCatalogue& catalogue, std::string_view stop_name);

        void ExecuteRenderMap(MapRenderer& map_catalogue, TransportCatalogue& catalogue_) const;
        
        std::optional<RouteInfo> GetRouteInfo(std::string_view start,
                                              std::string_view end,
                                              TransportCatalogue& catalogue,
                                              TransportRouter& routing) const;

    };

} //end namespace request_handler