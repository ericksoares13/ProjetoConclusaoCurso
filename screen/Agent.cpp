//
// Created by erick on 02/11/2025.
//

#include "Agent.h"
#include "../helper/PointHelper.h"

#include <random>


Agent::Agent(DynamicGraph& graph, const Type type, const Point &currentPosition, const long long startId, const long long endId) :
    type(type),
    currentPosition(currentPosition),
    path(std::vector<long long>()),
    startId(startId),
    currentId(startId),
    endId(endId),
    pathStaticAgentId(0),
    pathStaticAgent(std::vector<long long>()) {
    if (type == Static) {
        pathStaticAgent = graph.findPathAStar(startId, endId);
    }
}

std::pair<long long, long long> Agent::chooseRandomStartAndEnd(const DynamicGraph& graph) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<long long> ids;
    for (const auto& [id, point] : graph.getIdToPoint()) {
        ids.push_back(id);
    }

    std::uniform_int_distribution<> dist(0, static_cast<int>(ids.size() - 1));

    long long startId = ids[dist(gen)];
    long long endId = startId;

    while (endId == startId) {
        endId = ids[dist(gen)];
    }

    return {startId, endId};
}

std::vector<Agent*> Agent::initAgents(DynamicGraph& graph) {
    auto [startId, endId] = chooseRandomStartAndEnd(graph);

    auto* dynamicAgent = new Agent(graph, Dynamic, graph.getIdToPoint().at(startId), startId, endId);
    auto* staticAgent = new Agent(graph, Static, graph.getIdToPoint().at(startId), startId, endId);

    dynamicAgent->addPathMovent(startId);
    staticAgent->addPathMovent(startId);

    return {dynamicAgent, staticAgent};
}

void Agent::addPathMovent(long long id) {
    this->path.push_back(id);
}

void Agent::setCurrentId(const DynamicGraph& graph, long long id) {
    this->currentId = id;
    this->currentPosition = graph.getIdToPoint().at(id);
}

void Agent::move(DynamicGraph& graph) {
    if (this->currentId == this->endId) return;

    long long nextPointId;
    if (this->type == Static) {
        nextPointId = this->pathStaticAgent[this->pathStaticAgentId];
        const Point& currentPoint = graph.getIdToPoint().at(this->currentId);
        const Point& nextPoint = graph.getIdToPoint().at(nextPointId);
        bool validPath = true;

        for (const Polygon& polygon : graph.getPolygons()) {
            if (PointHelper::pointInConvexPolygon(polygon.getPoints(), currentPoint) ||
                PointHelper::pointInConvexPolygon(polygon.getPoints(), nextPoint)) {
                nextPointId = -1;
                validPath = false;
                break;
                }
        }

        if (validPath) {
            this->pathStaticAgentId++;
        }
    } else {
        nextPointId = graph.nextPointConsideringPolygonsAStar(this->currentId, this->endId);
    }

    if (nextPointId != -1) {
        this->addPathMovent(nextPointId);
        this->setCurrentId(graph, nextPointId);
    }
}

