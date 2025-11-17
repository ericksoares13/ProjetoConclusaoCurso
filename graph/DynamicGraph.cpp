//
// Created by erick on 14/09/2025.
//

#include "DynamicGraph.h"

#include <iostream>
#include <queue>
#include <random>
#include <omp.h>

#include "../helper/PointHelper.h"

DynamicGraph::DynamicGraph()
    : uniformGrid(UniformGrid(0.01)),
      cellSize(0.01),
      minLon(std::numeric_limits<double>::infinity()),
      maxLon(-std::numeric_limits<double>::infinity()),
      minLat(std::numeric_limits<double>::infinity()),
      maxLat(-std::numeric_limits<double>::infinity()) {}

void DynamicGraph::addPoint(const long long id, const double x, const double y) {
    this->idToPoint[id] = Point(id, x, y);
    this->adj[id] = {};

    minLon = std::min(minLon, x);
    maxLon = std::max(maxLon, x);
    minLat = std::min(minLat, y);
    maxLat = std::max(maxLat, y);
}

void DynamicGraph::addEdge(const long long idU, const long long idV, const double dist) {
    Point *pointU = &idToPoint[idU];
    Point *pointV = &idToPoint[idV];

    this->adj[idU].emplace_back(pointU, pointV, dist, true);
    this->uniformGrid.insertEdge(&this->adj[idU].back());
}

void DynamicGraph::addPolygon(const Polygon &polygon) {
    this->polygons.push_back(polygon);
}

void DynamicGraph::updatePolygonsPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-Polygon::maxMoveDistance, Polygon::maxMoveDistance);

    #pragma omp parallel for
    for (auto &polygon : this->polygons) {
        bool validMove = false;
        int attempts = 0;

        while (!validMove && attempts < 10) {
            const double ax = dist(gen) * Polygon::acceleration;
            const double ay = dist(gen) * Polygon::acceleration;

            polygon.setVelocityX(Polygon::inertia * polygon.getVelocityX() + ax);
            polygon.setVelocityY(Polygon::inertia * polygon.getVelocityY() + ay);

            const double speed = std::sqrt(polygon.getVelocityX() * polygon.getVelocityX() + polygon.getVelocityY() * polygon.getVelocityY());
            if (speed > Polygon::maxMoveDistance) {
                polygon.setVelocityX(polygon.getVelocityX() * (Polygon::maxMoveDistance / speed));
                polygon.setVelocityY(polygon.getVelocityY() * (Polygon::maxMoveDistance / speed));
            }

            validMove = polygon.updatePosition(polygon.getVelocityX(), polygon.getVelocityY(), this->getUniformGrid());
            attempts++;
        }
    }
}

std::vector<long long> DynamicGraph::findPathAStar(const long long idU, const long long idV) {
    std::vector<long long> path;

    if (!this->idToPoint.contains(idU) || !this->idToPoint.contains(idV)) {
        return path;
    }

    const Point& target = this->idToPoint.at(idV);

    std::unordered_map<long long, double> gCosts;
    std::unordered_map<long long, long long> previous;
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<>> pq;

    for (const auto& [id, point] : this->idToPoint) {
        gCosts[id] = std::numeric_limits<double>::infinity();
        previous[id] = -1;
    }

    gCosts[idU] = 0.0;
    double initialHCost = PointHelper::haversineDistance(this->idToPoint.at(idU), target);
    pq.emplace(idU, 0.0, initialHCost);

    while (!pq.empty()) {
        const AStarNode current = pq.top();
        pq.pop();

        const long long u = current.id;

        if (u == idV) {
            break;
        }

        if (current.gCost > gCosts[u]) {
            continue;
        }

        for (const Edge& edge : this->adj[u]) {
            const long long v = edge.getV()->getId();
            const double weight = edge.getDist();
            const double newGCost = gCosts[u] + weight;

            if (newGCost < gCosts[v]) {
                gCosts[v] = newGCost;
                previous[v] = u;

                const double hCost = PointHelper::haversineDistance(this->idToPoint.at(v), target);
                const double fCost = newGCost + hCost;

                pq.emplace(v, newGCost, fCost);
            }
        }
    }

    if (!previous.contains(idV) || previous.at(idV) == -1) {
        return path;
    }

    long long current = idV;
    while (current != idU && previous.at(current) != -1) {
        path.push_back(current);
        current = previous.at(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<long long> DynamicGraph::findPathAStarConsideringPolygons(const long long idU, const long long idV) {
    std::vector<long long> path;

    if (!this->idToPoint.contains(idU) || !this->idToPoint.contains(idV)) {
        return path;
    }

    const Point& target = this->idToPoint.at(idV);

    for (const Polygon& polygon : this->polygons) {
        if (PointHelper::pointInConvexPolygon(polygon.getPoints(), target)) {
            return findPathAStar(idU, idV);
        }
    }

    std::unordered_map<long long, double> gCosts;
    std::unordered_map<long long, long long> previous;
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<>> pq;

    for (const auto& [id, point] : this->idToPoint) {
        gCosts[id] = std::numeric_limits<double>::infinity();
        previous[id] = -1;
    }

    gCosts[idU] = 0.0;
    double initialHCost = PointHelper::haversineDistance(this->idToPoint.at(idU), target);
    pq.emplace(idU, 0.0, initialHCost);

    while (!pq.empty()) {
        const AStarNode current = pq.top();
        pq.pop();

        const long long u = current.id;

        if (u == idV) {
            break;
        }

        if (current.gCost > gCosts[u]) {
            continue;
        }

        for (const Edge& edge : this->adj[u]) {
            const long long v = edge.getV()->getId();
            const double weight = edge.getDist();

            bool edgeIntersectsPolygon = false;
            #pragma omp parallel for
            for (const Polygon& polygon : this->polygons) {
                if (!edgeIntersectsPolygon) {
                    if (PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge.getU()) ||
                        PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge.getV())) {
                        #pragma omp atomic write
                        edgeIntersectsPolygon = true;
                    }
                }
            }

            if (edgeIntersectsPolygon) {
                continue;
            }

            const double newGCost = gCosts[u] + weight;

            if (newGCost < gCosts[v]) {
                gCosts[v] = newGCost;
                previous[v] = u;

                const double hCost = PointHelper::haversineDistance(this->idToPoint.at(v), target);
                const double fCost = newGCost + hCost;

                pq.emplace(v, newGCost, fCost);
            }
        }
    }

    if (!previous.contains(idV) || previous.at(idV) == -1) {
        return findPathAStar(idU, idV);
    }

    long long current = idV;
    while (current != idU && previous.at(current) != -1) {
        path.push_back(current);
        current = previous.at(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}
