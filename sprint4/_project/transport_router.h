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
    double bus_wait_time;
    double bus_velocity;
};


class RouteContainer {
    //request_handler::RequestHandler &rhandler_;
    transport_router::RoutingSettings rSettings_;
    graph::DirectedWeightedGraph<double> dwg_;
    std::vector<std::string> edgeIndex_;
    std::unordered_map<std::string, size_t> vertexIndex_;
    std::pair <int, int> GetStopVertexes(const std::string &from, const std::string &to) const {
        auto it_from = vertexIndex_.find(from);
        auto it_to = vertexIndex_.find(to);
        if (it_from == vertexIndex_.end() || it_to == vertexIndex_.end())
            throw std::runtime_error ("Unknown vertex");
        
        return {it_from->second, it_to->second};
    }
    std::optional<size_t> GetStopVertex(const std::string &vertex) const {
        auto it = vertexIndex_.find(vertex);
        if (it == vertexIndex_.end())
            return std::nullopt;
        return it->second;
    }

    //void FillGraph(void);
public:
    RouteContainer(request_handler::RequestHandler &rHandler, transport_router::RoutingSettings routingSettings)
        : rSettings_(routingSettings)
        , dwg_(rHandler.GetStopNumber()*3)
        , edgeIndex_(rHandler.GetStopNumber()*3)
        {

            size_t stopVertex = 0;
            size_t nextStopVertex = 1;
            for (auto const& bus: rHandler.GetBuses()){

                for (size_t i = 0; i < bus->stops.size() - 1; i++){
                    auto const& stop_from = bus->stops[i];
                    auto const& stop_to = bus->stops[i + 1];
                    auto distance = rHandler.GetDistance(&(*stop_from), &(*stop_to));
                    auto time = static_cast<double>(distance)/rSettings_.bus_velocity;
                    auto numberFrom = GetStopVertex(stop_from->name);
                    auto numberTo = GetStopVertex(stop_to->name);
                    auto curStopVertex = stopVertex;
                    auto curNextStopVertex = nextStopVertex;
                    if (numberFrom.has_value()){
                        curStopVertex = numberFrom.value();
                    }
                    if (numberTo.has_value()){
                        curNextStopVertex = numberFrom.value();
                    }
                   


                    if (!numberFrom.has_value()){
                        graph::Edge<double> vWait1{curStopVertex, curStopVertex + 1, rSettings_.bus_wait_time};
                        dwg_.AddEdge(vWait1);
                        edgeIndex_.push_back(bus->name);
                        vertexIndex_[stop_from->name] = curStopVertex;
                    }
                    if (!numberTo.has_value()){
                        vertexIndex_[stop_to->name] = curNextStopVertex;
                    }

                    if (stop_from->name != stop_to->name){
                        graph::Edge<double> vStoping1{curStopVertex + 1, curNextStopVertex, time};
                        dwg_.AddEdge(vStoping1);
                        edgeIndex_.push_back(bus->name);
                    } else {
                        graph::Edge<double> vWait2{curStopVertex, curNextStopVertex + 1, rSettings_.bus_wait_time};
                        dwg_.AddEdge(vWait2);
                        edgeIndex_.push_back(bus->name);
                    }

                    if ( i != (bus->stops.size() - 1)){
                        graph::Edge<double> vGo1{curStopVertex + 1, curNextStopVertex + 1, time};
                        dwg_.AddEdge(vGo1);
                        edgeIndex_.push_back(bus->name);
                    }
                }
            }
        }
/*{
        "stop_name": "Apteka",
        "time": 2,
        "type": "Wait"
    },
    {
        "bus": "297",
        "span_count": 1,
        "time": 2.84,
        "type": "Bus"
    },
struct RoutePoint {
    std::string_view name;
    double time;
    std::string type;
    int span_count;
};*/
    std::optional<domain::Route> GetRoute(const std::string& stopFrom, const std::string& stopTo) const {
        using namespace std::string_literals;
        domain::Route route;
        auto [from, to] = GetStopVertexes(stopFrom, stopTo);
        graph::Router<double> router(this->dwg_);
        auto routeFromRouter = router.BuildRoute(from, to);
        if (!routeFromRouter.has_value())
            throw std::runtime_error ("No Path from "s + stopFrom + " to "s + stopTo);
        auto weight = routeFromRouter.value().weight;
        auto edges = routeFromRouter.value().edges;
        route.total_time = weight;

        return route;


    };
    
};

} //transport_router
