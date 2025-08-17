#include <string>
#include <vector>

#include <unordered_map>

#include <optional>

#include "domain.h"
#include "ranges.h"
#include "graph.h"
#include "router.h"

namespace transport_router {

struct RoutingSettings {
    int bus_wait_time;
    int bus_velocity;
};


class RouteContainer {
    //request_handler::RequestHandler &rhandler_;
    transport_router::RoutingSettings rSettings_;
    graph::DirectedWeightedGraph<double> dwg_;
    std::unordered_map<std::string, int> vertexIndex_;
    std::pair <int, int> GetStopVertexes(const std::string &from, const std::string &to) const {
        auto it_from = vertexIndex_.find(from);
        auto it_to = vertexIndex_.find(to);
        if (it_from == vertexIndex_.end() || it_to == vertexIndex_.end())
            throw std::runtime_error ("Unknown vertex");
        
        return {it_from->second, it_to->second};
    }
    std::optional<int> GetStopVertex(const std::string &vertex) const {
        auto it = vertexIndex_.find(vertex);
        if (it == vertexIndex_.end())
            return std::nullopt;
        return it->second;
    }

    //void FillGraph(void);
public:
    RouteContainer(request_handler::RequestHandler &rHandler, transport_router::RoutingSettings routingSettings)
        : rSettings_(routingSettings)
        , dwg_(rHandler.GetStopNumber())
        {

            int stopVertex = 0;
            int nextStopVertex = 1;
            for (auto const& bus: rHandler.GetBuses()){

                for (ssize_t i = 0; i < bus->stops.size() - 1; i++){
                    auto const& stop_from = bus->stops[i];
                    auto const& stop_to = bus->stops[i + 1];
                    auto distance = rHandler.GetDistance(&(*stop_from), &(*stop_to));
                    auto time = static_cast<double>(distance)/rSettings_.bus_velocity;
                    auto numberFrom = GetStopVertex(stop_from->name);
                    auto numberTo = GetStopVertex(stop_to->name);
                    int stopVertex = stopVertex;
                    int nextStopVertex = nextStopVertex;
                    if (!numberFrom.has_value()){
                        stopVertex = numberFrom.value();
                    }
                    if (!numberTo.has_value()){
                        nextStopVertex = numberFrom.value();
                    }


                    graph::Edge<double> vWait1{currentEdge, nextEdge, rSettings_.bus_wait_time};
                    graph::Edge<double> vStoping1{nextEdge, nextEdge + 1, time};
                    graph::Edge<double> vGo1{nextEdge, nextEdge + 2, time};
                    graph::Edge<double> vWait2{nextEdge + 1, nextEdge + 2, rSettings_.bus_wait_time};
                    graph::Edge<double> vStoping2{nextEdge + 2, currentEdge, time};
                    graph::Edge<double> vGo2{nextEdge + 2, nextEdge, time};

                    dwg_.AddEdge(vWait1);
                    dwg_.AddEdge(vStoping1);
                    dwg_.AddEdge(vGo1);
                    dwg_.AddEdge(vWait2);
                    dwg_.AddEdge(vStoping2);
                    dwg_.AddEdge(vGo2);
                }
            }
        }
    std::optional<domain::Route> GetRoute(std::string from, std::string to) const {
        domain::Route route;
        auto [from, to] = GetStopVertexes(from, to);

        return route;


    };
    
};

} //transport_router
