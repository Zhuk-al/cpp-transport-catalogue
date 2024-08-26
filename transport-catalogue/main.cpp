#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

using namespace std;
using namespace transport_catalogue;
using namespace map_renderer;
using namespace request_handler;
using namespace transport_catalogue::detail::json;
using namespace transport_catalogue::detail::router;

int main() {
    vector<StatRequest> stat_request;
    RenderSettings render_settings;
    TransportCatalogue catalogue;
    RoutingSettings routing_settings;

    JSONReader json_reader;
    RequestHandler request_handler;

    json_reader = JSONReader(cin);
    json_reader.Parse(catalogue, stat_request, render_settings, routing_settings);

    request_handler = RequestHandler();
    json_reader.ExecuteQueries(catalogue, stat_request, render_settings, routing_settings);
    transport_catalogue::detail::json::Print(json_reader.GetDocument(), cout);
}