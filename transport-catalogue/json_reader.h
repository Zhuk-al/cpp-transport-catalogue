#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
 
namespace transport_catalogue {
namespace detail {
namespace json {

using namespace request_handler;
    
class JSONReader{
public:
    JSONReader() = default;    
    JSONReader(Document doc);
    JSONReader(std::istream& input);
    
    void ParseNodeBase(const Node& root, TransportCatalogue& catalogue);
    void ParseNodeStat(const Node& root, std::vector<StatRequest>& stat_request);
    void ParseNodeRender(const Node& node, map_renderer::RenderSettings& render_settings);
    void ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
    void Parse(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
    
    Stop ParseNodeStop(Node& node);
    Bus ParseNodeBus(Node& node, TransportCatalogue& catalogue);
    std::vector<Distance> ParseNodeDistances(Node& node, TransportCatalogue& catalogue);

    Node ExecuteMakeNodeStop(int id_request, StopQueryInfo query_result);
    Node ExecuteMakeNodeBus(int id_request, BusQueryInfo query_result);
    Node ExecuteMakeNodeMap(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings);
    void ExecuteQueries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, RenderSettings& render_settings);

    const Document& GetDocument();

private:
    Document document_;
    RequestHandler request_handler_;
};
    
} //end namespace json
} //end namespace detail
} //end namespace transport_catalogue