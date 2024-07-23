#include "json_reader.h"

#include <sstream>
 
namespace transport_catalogue {
namespace detail {
namespace json {
    
JSONReader::JSONReader(Document doc) 
    : document_(std::move(doc)) {
}

JSONReader::JSONReader(std::istream& input) 
    : document_(json::Load(input)) {
}    
    
Stop JSONReader::ParseNodeStop(Node& node) {
    Stop stop;
    Dict stop_node;
    
    if (node.IsMap()) {
        stop_node = node.AsMap();
        stop.name = stop_node.at("name").AsString();
        stop.latitude = stop_node.at("latitude").AsDouble();
        stop.longitude = stop_node.at("longitude").AsDouble();
    }
    
    return stop;
}
    
void JSONReader::ParseNodeDistances(Node& node, TransportCatalogue& catalogue) {
    Dict stop_node;
    Dict stop_road_map;
    std::string begin_name;
    std::string last_name;
    int distance;
    
    if (node.IsMap()) {
        stop_node = node.AsMap();
        begin_name = stop_node.at("name").AsString();
        try {
            stop_road_map = stop_node.at("road_distances").AsMap();
            for (auto [key, value] : stop_road_map) {
                last_name = key;
                distance = value.AsInt();
                catalogue.SetDistance(catalogue.GetStop(begin_name),
                                      catalogue.GetStop(last_name), distance);
            }
        }
        catch (...) {
            std::cout << "invalide road" << std::endl;
        }
    }
}
 
Bus JSONReader::ParseNodeBus(Node& node, TransportCatalogue& catalogue) {
    Bus bus;
    Dict bus_node;
    Array bus_stops;
    
    if (node.IsMap()) {
        bus_node = node.AsMap();
        bus.name = bus_node.at("name").AsString();
        bus.is_roundtrip = bus_node.at("is_roundtrip").AsBool();
        try {
            bus_stops = bus_node.at("stops").AsArray();
            for (Node stop : bus_stops) {
                bus.stops.push_back(catalogue.GetStop(stop.AsString()));
            }
            if (!bus.is_roundtrip) {
                size_t size = bus.stops.size() - 1;
                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i - 1]);
                }
            }          
        }
        catch(...) {
            std::cout << "base_requests: bus: stops is empty" << std::endl;
        }        
    }
    
    return bus;
}
    
void JSONReader::ParseNodeBase(const Node& root, TransportCatalogue& catalogue){
    Array base_requests;
    Dict req_map;
    Node req_node;  
    std::vector<Node> buses;
    std::vector<Node> stops;
    
    if (root.IsArray()) {
        base_requests = root.AsArray();       
        for (Node& node : base_requests) {            
            if (node.IsMap()) {
                req_map = node.AsMap();             
                try {
                    req_node = req_map.at("type");                   
                    if (req_node.IsString()) {                       
                        if (req_node.AsString() == "Bus") {
                            buses.push_back(req_map);
                        }
                        else if (req_node.AsString() == "Stop") {
                            stops.push_back(req_map);
                        }
                        else {
                            std::cout << "base_requests are invalid";
                        }
                    }                    
                }
                catch(...) {
                    std::cout << "base_requests does not have type value";
                }                
            }
        }   

        for (auto stop : stops) {
            catalogue.AddStop(ParseNodeStop(stop));
        }

        for (auto stop : stops) {
            ParseNodeDistances(stop, catalogue);
        }

        for (auto bus : buses) {
            catalogue.AddBus(ParseNodeBus(bus, catalogue));
        }
        
    }
    else {
        std::cout << "base_requests is not an array";
    }  
}
 
void JSONReader::ParseNodeStat(const Node& node, std::vector<StatRequest>& stat_request){
    Array stat_requests;
    Dict req_map;
    StatRequest req;
    
    if (node.IsArray()) {
        stat_requests = node.AsArray();        
        for (Node req_node : stat_requests) {            
            if (req_node.IsMap()) {
                req_map = req_node.AsMap();
                req.id = req_map.at("id").AsInt();
                req.type = req_map.at("type").AsString();
                if (req.type != "Map") {
                    req.name = req_map.at("name").AsString();
                }
                else {
                    req.name = "";
                }                
                stat_request.push_back(req);                
            }   
        }        
    }
    else {
        std::cout << "base_requests is not array";
    }
}

svg::Color SetColor(const Node& color_data) {
    if (color_data.IsString()) {
        return color_data.AsString();
    }
    else if (color_data.IsArray() && color_data.AsArray().size() == 4) {
        auto result_color = svg::Rgba(static_cast<uint8_t>(color_data.AsArray().at(0).AsInt()),
            static_cast<uint8_t>(color_data.AsArray().at(1).AsInt()),
            static_cast<uint8_t>(color_data.AsArray().at(2).AsInt()),
            color_data.AsArray().at(3).AsDouble());
        return result_color;
    }
    else if (color_data.IsArray() && color_data.AsArray().size() == 3) {
        auto result_color = svg::Rgb(static_cast<uint8_t>(color_data.AsArray().at(0).AsInt()),
            static_cast<uint8_t>(color_data.AsArray().at(1).AsInt()),
            static_cast<uint8_t>(color_data.AsArray().at(2).AsInt()));
        return result_color;
    }
    else {
        return svg::NoneColor;
    }
}
  
void JSONReader::ParseNodeRender(const Node& node, map_renderer::RenderSettings& rend_set){
    Dict rend_map;
    Array bus_lab_offset;
    Array stop_lab_offset;
 
    if (node.IsMap()) {
        rend_map = node.AsMap();
        try {
            rend_set.width_ = rend_map.at("width").AsDouble();
            rend_set.height_ = rend_map.at("height").AsDouble();
            rend_set.padding_ = rend_map.at("padding").AsDouble();
            rend_set.line_width_ = rend_map.at("line_width").AsDouble();
            rend_set.stop_radius_ = rend_map.at("stop_radius").AsDouble();

            rend_set.bus_label_font_size_ = rend_map.at("bus_label_font_size").AsInt();
            
            if (rend_map.at("bus_label_offset").IsArray()) {
                bus_lab_offset = rend_map.at("bus_label_offset").AsArray();
                rend_set.bus_label_offset_ = std::make_pair(bus_lab_offset[0].AsDouble(),
                                                            bus_lab_offset[1].AsDouble());
            }
            
            rend_set.stop_label_font_size_ = rend_map.at("stop_label_font_size").AsInt();
 
            if (rend_map.at("stop_label_offset").IsArray()) {
                stop_lab_offset = rend_map.at("stop_label_offset").AsArray();
                rend_set.stop_label_offset_ = std::make_pair(stop_lab_offset[0].AsDouble(),
                                                             stop_lab_offset[1].AsDouble());
            }
            
            rend_set.underlayer_color_ = SetColor(rend_map.at("underlayer_color"));

            rend_set.underlayer_width_ = rend_map.at("underlayer_width").AsDouble();

            if (rend_map.at("color_palette").IsArray()) {
                auto arr_palette = rend_map.at("color_palette").AsArray();
                for (const Node& color_palette : arr_palette) {
                    rend_set.color_palette_.emplace_back(SetColor(color_palette));
                }
            }            
        }
        catch(...) {
            std::cout << "unable to parsse init settings";
        }
        
    }
    else {
        std::cout << "render_settings is not map";
    }
}
    
void JSONReader::ParseNode(const Node& root, TransportCatalogue& catalogue, 
                           std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings){
    Dict root_dictionary;
    
    if (root.IsMap()) {
        root_dictionary = root.AsMap();        
        try {          
            ParseNodeBase(root_dictionary.at("base_requests"), catalogue);
        }
        catch(...) {
            std::cout << "base_requests is empty";
        }
        
        try {
            ParseNodeStat(root_dictionary.at("stat_requests"), stat_request);
        }
        catch(...) {
            std::cout << "stat_requests is empty";
        }
        
        try {
            ParseNodeRender(root_dictionary.at("render_settings"), render_settings);
        }
        catch(...) {
            std::cout << "render_settings is empty";
        }
        
    }
    else {
        std::cout << "root is not map";
    }
}
    
void JSONReader::Parse(TransportCatalogue& catalogue, 
                       std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings) {  
    ParseNode(document_.GetRoot(), catalogue, stat_request, render_settings);
}

Node JSONReader::ExecuteMakeNodeStop(int id_request, StopQueryInfo stop_info) {
    Dict result;
    Array buses;
    std::string str_not_found = "not found";

    if (stop_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));

    }
    else {
        result.emplace("request_id", Node(id_request));
        for (std::string bus_name : stop_info.buses_name) {
            buses.push_back(Node(bus_name));
        }
        result.emplace("buses", Node(buses));
    }
    return Node(result);
}

Node JSONReader::ExecuteMakeNodeBus(int id_request, BusQueryInfo bus_info) {
    Dict result;
    std::string str_not_found = "not found";

    if (bus_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));
    }
    else {
        result.emplace("request_id", Node(id_request));
        result.emplace("curvature", Node(bus_info.curvature));
        result.emplace("route_length", Node(bus_info.route_length));
        result.emplace("stop_count", Node(bus_info.stops_on_route));
        result.emplace("unique_stop_count", Node(bus_info.unique_stops));
    }
    return Node(result);
}

Node JSONReader::ExecuteMakeNodeMap(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings) {
    Dict result;
    std::ostringstream map_stream;
    std::string map_str;

    MapRenderer map_catalogue(render_settings);
    map_catalogue.InitSphereProjector(request_handler_.GetStopsCoordinates(catalogue));
    request_handler_.ExecuteRenderMap(map_catalogue, catalogue);
    map_catalogue.GetStreamMap(map_stream);
    map_str = map_stream.str();

    result.emplace("request_id", Node(id_request));
    result.emplace("map", Node(map_str));

    return Node(result);
}

void JSONReader::ExecuteQueries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, RenderSettings& render_settings) {
    std::vector<Node> result_request;

    for (StatRequest req : stat_requests) {
        if (req.type == "Stop") {
            result_request.push_back(ExecuteMakeNodeStop(req.id, request_handler_.StopQuery(catalogue, req.name)));
        }
        else if (req.type == "Bus") {
            result_request.push_back(ExecuteMakeNodeBus(req.id, request_handler_.BusQuery(catalogue, req.name)));
        }
        else if (req.type == "Map") {
            result_request.push_back(ExecuteMakeNodeMap(req.id, catalogue, render_settings));
        }

    }
    document_ = Document{ Node(result_request) };

}

const Document& JSONReader::GetDocument() {
    return document_;
}
    
} //end namespace json
} //end namespace detail
} //end namespace transport_catalogue