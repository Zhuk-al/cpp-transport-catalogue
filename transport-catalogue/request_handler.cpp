#include "request_handler.h"

namespace request_handler {

    void RequestHandler::ExecuteRenderMap(MapRenderer& map_catalogue, TransportCatalogue& catalogue) const {
        std::vector<std::pair<Bus*, int>> buses_palette;
        std::vector<Stop*> stops_sort;
        int palette_size = 0;
        int palette_index = 0;

        palette_size = map_catalogue.GetPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return;
        }

        auto buses = catalogue.GetBusnameToBus();
        if (buses.size() > 0) {
            for (std::string_view bus_name : GetSortBusesNames(catalogue)) {
                Bus* bus_info = catalogue.GetBus(bus_name);
                if (bus_info) {
                    if (bus_info->stops.size() > 0) {
                        buses_palette.push_back(std::make_pair(bus_info, palette_index));
                        palette_index++;
                        if (palette_index == palette_size) {
                            palette_index = 0;
                        }
                    }
                }
            }

            if (buses_palette.size() > 0) {
                map_catalogue.AddLine(buses_palette);
                map_catalogue.AddBusesName(buses_palette);
            }
        }

        auto stops = catalogue.GetStopnameToStop();
        if (stops.size() > 0) {
            std::vector<std::string_view> stops_name;
            for (auto& [stop_name, stop] : stops) {
                if (stop->buses.size() > 0) {
                    stops_name.push_back(stop_name);
                }
            }
            std::sort(stops_name.begin(), stops_name.end());
            for (std::string_view stop_name : stops_name) {
                Stop* stop = catalogue.GetStop(stop_name);
                if (stop) {
                    stops_sort.push_back(stop);
                }
            }

            if (stops_sort.size() > 0) {
                map_catalogue.AddStopsCircle(stops_sort);
                map_catalogue.AddStopsName(stops_sort);
            }
        }
    }

    std::vector<geo::Coordinates> RequestHandler::GetStopsCoordinates(TransportCatalogue& catalogue) const {
        std::vector<geo::Coordinates> stops_coordinates;
        auto buses = catalogue.GetBusnameToBus();

        for (auto& [busname, bus] : buses) {
            for (auto& stop : bus->stops) {
                geo::Coordinates coordinates;
                coordinates.lat = stop->latitude;
                coordinates.lng = stop->longitude;
                stops_coordinates.push_back(coordinates);
            }
        }
        return stops_coordinates;
    }

    std::vector<std::string_view> RequestHandler::GetSortBusesNames(TransportCatalogue& catalogue) const {
        std::vector<std::string_view> buses_names;

        auto buses = catalogue.GetBusnameToBus();
        if (buses.size() > 0) {
            for (auto& [busname, bus] : buses) {
                buses_names.push_back(busname);
            }
            std::sort(buses_names.begin(), buses_names.end());
            return buses_names;
        }
        else {
            return {};
        }
    }

    BusQueryInfo RequestHandler::BusQuery(TransportCatalogue& catalogue, std::string_view bus_name) {
        BusQueryInfo bus_info;
        Bus* bus = catalogue.GetBus(bus_name);

        if (bus != nullptr) {
            bus_info.name = bus->name;
            bus_info.not_found = false;
            bus_info.stops_on_route = bus->stops.size();
            bus_info.unique_stops = catalogue.GetUniqStops(bus).size();
            bus_info.route_length = bus->route_length;
            bus_info.curvature = double(catalogue.GetDistanceBus(bus)
                / catalogue.GetDistance(bus));
        }
        else {
            bus_info.name = bus_name;
            bus_info.not_found = true;
        }
        return bus_info;
    }

    StopQueryInfo RequestHandler::StopQuery(TransportCatalogue& catalogue, std::string_view stop_name) {
        std::unordered_set<const Bus*> unique_buses;
        StopQueryInfo stop_info;
        Stop* stop = catalogue.GetStop(stop_name);

        if (stop != NULL) {
            stop_info.name = stop->name;
            stop_info.not_found = false;
            unique_buses = catalogue.StopGetUniqBuses(stop);

            if (unique_buses.size() > 0) {
                for (const Bus* bus : unique_buses) {
                    stop_info.buses_name.push_back(bus->name);
                }
                std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
            }
        }
        else {
            stop_info.name = stop_name;
            stop_info.not_found = true;
        }
        return stop_info;
    }

    std::optional<RouteInfo> RequestHandler::GetRouteInfo(std::string_view start,
        std::string_view end,
        TransportCatalogue& catalogue,
        TransportRouter& routing) const {

        return routing.GetRouteInfo(routing.GetRouterByStop(catalogue.GetStop(start))->bus_wait_start,
            routing.GetRouterByStop(catalogue.GetStop(end))->bus_wait_start);
    }
     
} //end namespace request_handler