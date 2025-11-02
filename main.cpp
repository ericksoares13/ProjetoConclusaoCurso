#include <fstream>
#include <iostream>

#include "geometry/Polygon.h"
#include "graph/DynamicGraph.h"
#include "screen/Agent.h"
#include "screen/Screen.h"

int main() {
    int inputNumber;
    printf("Opcoes de entrada\n");
    printf("1 - Belo Horizonte\n");
    printf("2 - Ipatinga\n");
    printf("3 - Viçosa\n");
    printf("Escolha a entrada: ");
    scanf("%d", &inputNumber);

    std::string filename = "input" + std::to_string(inputNumber) + ".txt";
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " + filename + "\n";
        return 1;
    }

    DynamicGraph graph;
    int numPoints;
    int numEdges;

    inputFile >> numPoints;
    for (int i = 0; i < numPoints; i++) {
        long long id;
        double x;
        double y;

        inputFile >> id >> x >> y;
        graph.addPoint(id, x, y);
    }

    inputFile >> numEdges;
    for (int i = 0; i < numEdges; i++) {
        long long idU;
        long long idV;
        double dist;

        inputFile >> idU >> idV >> dist;
        graph.addEdge(idU, idV, dist);
    }

    inputFile.close();

    graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), 0.009));
    graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), 0.009));
    graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), 0.009));

    Screen screen;
    screen.drawBackground(graph);

    std::vector<Agent*> agents = Agent::initAgents(graph);

    bool simulationRunning = true;

    while (screen.windowIsOpen() && simulationRunning) {
        screen.processEvents();
        screen.update();
        graph.updatePolygonsPosition();

        bool allAgentsArrived = true;
        for (auto &agent : agents) {
            agent->move(graph);

            if (agent->getCurrentId() != agent->getEndId()) {
                allAgentsArrived = false;
            }
        }

        if (allAgentsArrived) {
            simulationRunning = false;
        }

        screen.render(graph, agents);
    }

    while (screen.windowIsOpen()) {
        screen.processEvents();
        screen.update();
        screen.render(graph, agents);
    }

    for (auto agent : agents) {
        delete agent;
    }

    return 0;
}
