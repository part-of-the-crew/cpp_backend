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
    struct EdgeInfo {
        std::string type;
        std::string name;
    };
    
    request_handler::RequestHandler &rHandler_;
    transport_router::RoutingSettings rSettings_;
    graph::DirectedWeightedGraph<double> dwg_;
    std::vector<EdgeInfo> edgeList_;
    std::unordered_map<std::string, size_t> vertexIndex_;

    std::pair <int, int> GetStopVertexes(const std::string &from, const std::string &to) const {
        auto it_from = vertexIndex_.find(from);
        auto it_to = vertexIndex_.find(to);
        if (it_from == vertexIndex_.end() || it_to == vertexIndex_.end())
            throw std::runtime_error ("Unknown vertex");
        
        return {it_from->second, it_to->second};
    }
    size_t GetStopVertex(const std::string &vertex) const {
        using namespace std::string_literals;
        auto it = vertexIndex_.find(vertex);
        if (it == vertexIndex_.end())
            throw std::runtime_error ("Unknown vertex "s + vertex);
        return it->second;
    }
    void SetStops(){
        using namespace std::string_literals;
        for (auto const& stop: rHandler_.GetAllStopNames()){
            const auto index = vertexIndex_.size();
            vertexIndex_[std::string(stop)] = index;


            //dwg_.AddEdge(graph::Edge<double> (index + 1, index, 0));
            //edgeList_.push_back({"NoWait"s, ""s});
        }
    }
    void SetRoutes(){
        auto const& buses = rHandler_.GetBuses();
        auto waitVertex = vertexIndex_.size();
        for (auto it = buses.begin(); it != buses.end(); ++it) {
            waitVertex = SetRoute(*it, waitVertex);
        }
    }
    void SetWaitVertex(std::string stop, size_t stopIndex){
        using namespace std::string_literals;
        //const auto index = edgeList_.size();
        dwg_.AddEdge(graph::Edge<double> (GetStopVertex(stop), stopIndex, rSettings_.bus_wait_time));
        edgeList_.push_back({"Wait"s, stop});
        //return stopIndex;
    }
    std::size_t SetRoute (const domain::Bus *const bus, std::size_t waitVertex){
        using namespace std::string_literals;
        constexpr double KMH_TO_MS = 60.0 / 1000.0;
        auto const& stops = bus->stops;
        //auto waitVertex = vertexIndex_.size();
        for (size_t i = 0; i < stops.size() - 1; i++){
            auto const& stop_from = stops[i];
            auto const& stop_to = stops[i + 1];
            auto distance = rHandler_.GetDistance(&(*stop_from), &(*stop_to));
            auto time = static_cast<double>(distance) / rSettings_.bus_velocity * KMH_TO_MS;
            //auto fromIndex = GetStopVertex(stop_from->name);
            auto toIndex = GetStopVertex(stop_to->name);
            SetWaitVertex(stop_from->name, waitVertex);
            //auto waitToIndex = SetWaitVertex(stop_to->name, toIndex);

            if ( i < (bus->stops.size() - 2)){
                //Go
                dwg_.AddEdge({waitVertex, waitVertex + 1, time});
                edgeList_.push_back({"Bus"s, bus->name});
            }
            //Stoping
            dwg_.AddEdge({waitVertex, toIndex, time});
            edgeList_.push_back({"Bus"s, bus->name});
/*
            std::cout << "  Edge " << i
               << ": ●" << curStopVertex << "("<< stop_from->name << ")" 
               << " --" << curNextStopVertex << "--> ●(" << stop_to->name << " time = " << time
               << " bus_wait_time = " << rSettings_.bus_wait_time
               << ")\n";
*/
            waitVertex++;
        }
        return waitVertex;
    }
public:
    RouteContainer(request_handler::RequestHandler &rHandler, const transport_router::RoutingSettings& routingSettings)
        : rHandler_(rHandler)
        , rSettings_(routingSettings)
        , dwg_(rHandler.GetStopNumber()*8)
        //, edgeList_(rHandler.GetStopNumber()*8)
        {
            SetStops();
            SetRoutes();
            return;
        }
            /*
            size_t stopVertex = 0;
            size_t nextStopVertex = 2;

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
                        curNextStopVertex = numberTo.value();
                    }
                   


                    if (!numberFrom.has_value()){
                        graph::Edge<double> vWait1{curStopVertex, curStopVertex + 1, rSettings_.bus_wait_time};
                        dwg_.AddEdge(vWait1);
                        edgeList_.push_back("Wait");
                        vertexIndex_[stop_from->name] = curStopVertex;
                    } else {
                        ;//std::cout << "  why " << std::endl;
                    }
                    if (!numberTo.has_value()){
                        ;//vertexIndex_[stop_to->name] = curNextStopVertex;
                    }
                    std::cout << "  Edge " << i
                       << ": ●" << curStopVertex << "("<< stop_from->name << ")" 
                       << " --" << curNextStopVertex << "--> ●(" << stop_to->name << " time = " << time
                       << " bus_wait_time = " << rSettings_.bus_wait_time
                       << ")\n";
                    if (stop_from->name != stop_to->name){
                        graph::Edge<double> vStoping1{curStopVertex + 1, curNextStopVertex, time};
                        dwg_.AddEdge(vStoping1);
                        edgeList_.push_back(bus->name);
                        ++stopVertex;
                    } else {
                        graph::Edge<double> vWait2{curStopVertex, curNextStopVertex + 1, rSettings_.bus_wait_time};
                        dwg_.AddEdge(vWait2);
                        edgeList_.push_back("Wait");
                    }
                    ++nextStopVertex;
                    if ( i != (bus->stops.size() - 1)){
                        graph::Edge<double> vGo1{curStopVertex + 1, curNextStopVertex + 1, time};
                        dwg_.AddEdge(vGo1);
                        edgeList_.push_back(bus->name);
                    }
                }
            }
        }
        */
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
    std::string name;
    double time;
    std::string type;
    int span_count;
            {
                "stop_name": "Lipetskaya ulitsa 46",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "289",
                "span_count": 1,
                "time": 24.8,
                "type": "Bus"
            }};
*/

    std::optional<domain::Route> GetRoute(const std::string& stopFrom, const std::string& stopTo) const {
        using namespace std::string_literals;
        domain::Route route;
        auto [from, to] = GetStopVertexes(stopFrom, stopTo);
        graph::Router<double> router(this->dwg_);
        std::optional<graph::Router<double>::RouteInfo> routeFromRouter = router.BuildRoute(from, to);
        //dwg_.PrintGraph();
        if (!routeFromRouter.has_value()){
            return std::nullopt;
            //throw std::runtime_error ("No Path from "s + stopFrom + " to "s + stopTo);
        }
        auto weight = routeFromRouter.value().weight;
        route.total_time = weight;
        for (size_t point: routeFromRouter.value().edges){//size_t
            graph::Edge<double> edge = dwg_.GetEdge(point);
            auto type = edgeList_.at(point);
            domain::RoutePoint routePoint{type.name, edge.weight, type.type, 1};
            if (!route.routePoints.empty()){
                domain::RoutePoint& prev = route.routePoints.back();
                if (prev.type ==  routePoint.type){
                    ++prev.span_count;
                    prev.time += routePoint.time;
                } else {
                    route.routePoints.emplace_back(std::move(routePoint));
                }
            } else {
                route.routePoints.emplace_back(std::move(routePoint));
            }

        }
        return route;


    };
    
};

} //transport_router
