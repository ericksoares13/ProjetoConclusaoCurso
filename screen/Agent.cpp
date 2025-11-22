//
// Created by erick on 02/11/2025.
//

#include "Agent.h"

#include <chrono>

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
    pathAgentId(0),
    pathAgent(std::vector<long long>()),
    progressAlongEdge(0.0),
    currentSpeed(100),
    nextNodeId(-1),
    isMoving(false) {
    // Start time
    const auto start = std::chrono::high_resolution_clock::now();

    if (type == Static) {
        this->pathAgent = graph.findPathAStar(startId, endId);
    } else {
        this->pathAgent = graph.findPathAStarConsideringPolygons(startId, endId);
    }

    // End time
    const auto end = std::chrono::high_resolution_clock::now();
    this->processTimeMS += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (!this->pathAgent.empty()) {
        this->nextNodeId = this->pathAgent[0];
        this->isMoving = true;
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

    #pragma omp parallel for
    for (int i = 0; i < polygons.size(); i++) {
        const Polygon& poly = polygons[i];
        auto newCells = GridHelper::getOccupiedCells(poly, graph.getUniformGrid());

        #pragma omp critical(cache_access)
        {
            const auto it = this->polygonToCellsCache.find(i);
            if (it == this->polygonToCellsCache.end() || it->second != newCells) {
                this->polygonToCellsCache[i] = std::move(newCells);
                changed = true;
            }
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
    if (this->currentId == this->endId) {
        this->isMoving = false;
        return;
    }

    // Start time
    const auto start = std::chrono::high_resolution_clock::now();
    this->moves++;

    if (!this->isMoving) {
        if (this->type == Dynamic) {
            bool currentPathValid = !this->pathAgent.empty();

            if (currentPathValid) {
                this->updateOccupiedCellsCache(graph);

                for (int i = this->pathAgentId; i < this->pathAgent.size(); i++) {
                    const Cell pointCell = GridHelper::getCellPoint(graph.getIdToPoint().at(this->pathAgent[i]), graph.getUniformGrid().getCellSize());

                    if (this->hasLastIntersection && this->lastIntersectionCell == pointCell) {
                        break;
                    }

                    if (!isPointSafeCache(graph.getIdToPoint().at(this->pathAgent[i]), graph)) {
                        currentPathValid = false;
                        break;
                    }
                }
            }

            if (!currentPathValid) {
                this->aStarQnt++;
                this->pathAgent = graph.findPathAStarConsideringPolygons(this->currentId, this->endId);
                this->pathAgentId = 0;
            }
        }

        if (!this->pathAgent.empty() && this->pathAgentId < this->pathAgent.size()) {
            this->nextNodeId = this->pathAgent[this->pathAgentId];
            const Point& currentPoint = graph.getIdToPoint().at(this->currentId);
            const Point& nextPoint = graph.getIdToPoint().at(this->nextNodeId);
            bool validPath = true;

            for (const Polygon& polygon : graph.getPolygons()) {
                if (PointHelper::pointInConvexPolygon(polygon.getPoints(), currentPoint) ||
                    PointHelper::pointInConvexPolygon(polygon.getPoints(), nextPoint)) {
                    validPath = false;
                    break;
                }
            }

            if (validPath) {
                this->isMoving = true;
                this->progressAlongEdge = 0.0;
            }
        }
    }

    if (this->isMoving) {
        const Point& startPoint = graph.getIdToPoint().at(this->currentId);
        const Point& endPoint = graph.getIdToPoint().at(this->nextNodeId);

        const double edgeDistance = PointHelper::haversineDistance(startPoint, endPoint);
        const double progressIncrement = this->currentSpeed / edgeDistance;
        this->progressAlongEdge += progressIncrement;

        const double newX = startPoint.getX() + (endPoint.getX() - startPoint.getX()) * this->progressAlongEdge;
        const double newY = startPoint.getY() + (endPoint.getY() - startPoint.getY()) * this->progressAlongEdge;

        this->currentPosition.setX(newX);
        this->currentPosition.setY(newY);

        if (this->progressAlongEdge >= 1.0) {
            this->addPathMovent(this->nextNodeId);
            this->setCurrentId(graph, this->nextNodeId);
            this->pathAgentId++;
            this->progressAlongEdge = 0.0;
            this->isMoving = false;
            this->dist += edgeDistance;
        }
    }

    // End time
    const auto end = std::chrono::high_resolution_clock::now();
    this->processTimeMS += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
