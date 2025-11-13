//
// Created by erick on 02/11/2025.
//

#include "Agent.h"
#include "../helper/GridHelper.h"
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
        this->pathStaticAgent = graph.findPathAStar(startId, endId);
    } else {
        this->pathStaticAgent = graph.findPathAStarConsideringPolygons(startId, endId);
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
    long long startId;
    long long endId;

    do {
        std::tie(startId, endId) = chooseRandomStartAndEnd(graph);
    } while (graph.findPathAStar(startId, endId).empty());

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

void Agent::updateOccupiedCellsCache(const DynamicGraph& graph) {
    bool changed = false;

    const auto& polygons = graph.getPolygons();

    for (int i = 0; i < polygons.size(); i++) {
        const Polygon& poly = polygons[i];
        auto newCells = GridHelper::getOccupiedCells(poly, graph.getUniformGrid());

        const auto it = this->polygonToCellsCache.find(i);
        if (it == this->polygonToCellsCache.end() || it->second != newCells) {
            this->polygonToCellsCache[i] = std::move(newCells);
            changed = true;
        }
    }

    if (changed) {
        this->lastOccupiedCells.clear();

        for (const auto& [id, cells] : this->polygonToCellsCache) {
            this->lastOccupiedCells.insert(cells.begin(), cells.end());
        }
    }
}

bool Agent::isPointSafeCache(const Point& point, const DynamicGraph& graph) {
    const Cell pointCell = GridHelper::getCellPoint(point, graph.getUniformGrid().getCellSize());

    if (!this->lastOccupiedCells.contains(pointCell)) {
        return true;
    }

    for (const Polygon& polygon : graph.getPolygons()) {
        if (PointHelper::pointInConvexPolygon(polygon.getPoints(), point)) {
            this->lastIntersectionCell = pointCell;
            this->hasLastIntersection = true;
            return false;
        }
    }

    this->hasLastIntersection = false;
    return true;
}

void Agent::move(DynamicGraph& graph) {
    if (this->currentId == this->endId) return;

    long long nextPointId = -1;

    if (this->type == Dynamic) {
        bool currentPathValid = !this->pathStaticAgent.empty();

        if (currentPathValid) {
            this->updateOccupiedCellsCache(graph);

            for (int i = this->pathStaticAgentId; i < this->pathStaticAgent.size(); i++) {
                nextPointId = this->pathStaticAgent[i];
                const Cell pointCell = GridHelper::getCellPoint(graph.getIdToPoint().at(nextPointId), graph.getUniformGrid().getCellSize());

                if (this->hasLastIntersection && this->lastIntersectionCell == pointCell) {
                    break;
                }

                if (!isPointSafeCache(graph.getIdToPoint().at(nextPointId), graph)) {
                    currentPathValid = false;
                    nextPointId = -1;
                    break;
                }
            }
        }

        if (!currentPathValid) {
            this->pathStaticAgent = graph.findPathAStarConsideringPolygons(this->currentId, this->endId);
            this->pathStaticAgentId = 0;
        }
    }

    if (!this->pathStaticAgent.empty()) {
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
    }

    if (nextPointId != -1) {
        this->addPathMovent(nextPointId);
        this->setCurrentId(graph, nextPointId);
    }
}
