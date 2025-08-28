#include <string>
#include <vector>

#include <unordered_map>
//#include "log_duration.h"
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
    request_handler::RequestHandler &rHandler_;
    transport_router::RoutingSettings rSettings_;
    graph::DirectedWeightedGraph<double> dwg_;
    std::vector<domain::RoutePoint> edgeList_;
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
            const auto index = vertexIndex_.size() * 2;
            vertexIndex_[std::string(stop)] = index;

            dwg_.AddEdge(graph::Edge<double> (index, index + 1, rSettings_.bus_wait_time));
            edgeList_.push_back({"Wait"s, std::string(stop), rSettings_.bus_wait_time, 0});
        }
    }
    void SetRoutes(){
        auto const& buses = rHandler_.GetBuses();
        for (auto it = buses.begin(); it != buses.end(); ++it) {
            SetRoute(*it);
        }
    }
    void SetRoute (const domain::Bus *const bus){
        using namespace std::string_literals;
        constexpr double KMH_TO_MS = 60.0 / 1000.0;
        for (auto stop_from = bus->stops.begin(); stop_from != bus->stops.end(); ++stop_from){
            double accDistance = 0;
            for (auto stop_to = next(stop_from); stop_to != bus->stops.end(); ++stop_to){
                auto distance = rHandler_.GetDistance(&(*(*prev(stop_to))), &(*(*stop_to)));
                accDistance += static_cast<double>(distance) / rSettings_.bus_velocity * KMH_TO_MS;
                auto fromIndex = GetStopVertex((*stop_from)->name);
                auto toIndex = GetStopVertex((*stop_to)->name);
                auto span = std::distance(stop_from, stop_to);
                dwg_.AddEdge({fromIndex + 1, toIndex, accDistance});
                edgeList_.push_back({"Bus"s, bus->name, accDistance, static_cast<int>(span)});
            }
        }
        return;
    }
public:
    RouteContainer(request_handler::RequestHandler &rHandler, const transport_router::RoutingSettings& routingSettings)
        : rHandler_(rHandler)
        , rSettings_(routingSettings)
        , dwg_(rHandler.GetStopNumber() * 2)
        {
            SetStops();
            SetRoutes();
            router_ = std::make_unique<graph::Router<double>>(dwg_);
            return;
        }

    std::optional<domain::Route> GetRoute(const std::string& stopFrom, const std::string& stopTo) const {
        using namespace std::string_literals;
        domain::Route route;
        auto [from, to] = GetStopVertexes(stopFrom, stopTo);
        std::optional<graph::Router<double>::RouteInfo> routeFromRouter = router_->BuildRoute(from, to);
        dwg_.PrintGraph(std::cerr);
        if (!routeFromRouter.has_value()){
            return std::nullopt;
        }
        route.total_time = routeFromRouter.value().weight;
        for (size_t point: routeFromRouter.value().edges){
            route.routePoints.emplace_back(edgeList_[point]);
        }
        return route;
    };
    
};

} //transport_router
