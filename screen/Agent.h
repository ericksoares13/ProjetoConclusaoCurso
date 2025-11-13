//
// Created by erick on 02/11/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_AGENT_H
#define PROJETOCONCLUSAOCURSO_AGENT_H
#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../graph/DynamicGraph.h"


class Agent {
public:
    enum Type { Dynamic, Static };

private:
    Type type;
    Point currentPosition;
    std::vector<long long> path;
    long long startId;
    long long currentId;
    long long endId;

    std::vector<long long> pathAgent;
    int pathAgentId;

    double progressAlongEdge;
    double currentSpeed;
    long long nextNodeId;
    bool isMoving;

    std::unordered_set<Cell, Cell::Hash> lastOccupiedCells;
    std::unordered_map<size_t, std::unordered_set<Cell, Cell::Hash>> polygonToCellsCache;

    Cell lastIntersectionCell;
    bool hasLastIntersection;

    explicit Agent(DynamicGraph &graph, Type type, const Point &currentPosition, long long startId,
                   long long endId);

    static std::pair<long long, long long> chooseRandomStartAndEnd(const DynamicGraph& graph);

    bool isPointSafeCache(const Point& point, const DynamicGraph& graph);
    void updateOccupiedCellsCache(const DynamicGraph& graph);

public:
    ~Agent() = default;

    static std::vector<Agent*> initAgents(DynamicGraph& graph);
    void addPathMovent(long long id);
    void setCurrentId(const DynamicGraph& graph, long long id);
    void move(DynamicGraph& graph);

    const Type &getType() const { return this->type; }
    const Point &getCurrentPosition() const { return this->currentPosition; }
    const std::vector<long long> &getPath() const { return this->path; }
    long long getStartId() const { return this->startId; }
    long long getCurrentId() const { return this->currentId; }
    long long getEndId() const { return this->endId; }
};


#endif //PROJETOCONCLUSAOCURSO_AGENT_H