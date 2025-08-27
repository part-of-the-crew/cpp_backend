#include <string>
#include <vector>

#include <unordered_map>
#include "log_duration.h"
#include <memory>
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
    mutable std::unique_ptr<graph::Router<double>> router_;  // 🔥 lazy router
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
        , dwg_(rHandler.GetNumberOfRoutePoints() + rHandler.GetStopNumber())
        {
            {
            LOG_DURATION("set duration");
            SetStops();
            SetRoutes();
            }
            {
                LOG_DURATION("router duration");
                router_ = std::make_unique<graph::Router<double>>(dwg_);
            }
            return;
        }

    std::optional<domain::Route> GetRoute(const std::string& stopFrom, const std::string& stopTo) const {
        using namespace std::string_literals;
        domain::Route route;
        auto [from, to] = GetStopVertexes(stopFrom, stopTo);
        std::optional<graph::Router<double>::RouteInfo> routeFromRouter = router_->BuildRoute(from, to);
        //dwg_.PrintGraph();
        if (!routeFromRouter.has_value()){
            return std::nullopt;
        }
        route.total_time = routeFromRouter.value().weight;
        for (size_t point: routeFromRouter.value().edges){//size_t
            graph::Edge<double> edge = dwg_.GetEdge(point);
            auto type = edgeList_[point];
            domain::RoutePoint routePoint{type.name, edge.weight, type.type, 1};
            if (!route.routePoints.empty()){
                domain::RoutePoint& prev = route.routePoints.back();
                if (prev.type ==  routePoint.type){
                    ++prev.span_count;
                    prev.time += routePoint.time;
                    continue;
                }
            }
            route.routePoints.emplace_back(std::move(routePoint));
        }
        return route;


    };
    
};

} //transport_router
