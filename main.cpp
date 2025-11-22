#include <fstream>
#include <iostream>

#include "geometry/Polygon.h"
#include "graph/DynamicGraph.h"
#include "screen/Agent.h"
#include "screen/Screen.h"

void initGraph(DynamicGraph &graph, std::ifstream &inputFile) {
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
}

void runTest(DynamicGraph &graph, std::ofstream &csvFile, const int numPolygons, const double polygonRadius) {
    graph.clearPolygons();
    for(int i = 0; i < numPolygons; i++) {
        graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), polygonRadius));
    }

    const auto agents = Agent::initAgents(graph);
    const Agent* dynamicAgent = agents.at(0);
    const Agent* staticAgent = agents.at(1);

    bool running = true;

    while (running) {
        graph.updatePolygonsPosition();

        for (auto &agent : agents) {
            agent->move(graph);
        }

        const bool dynamicArrived = (dynamicAgent->getCurrentId() == dynamicAgent->getEndId());
        const bool staticArrived = (staticAgent->getCurrentId() == staticAgent->getEndId());

        if (dynamicArrived && staticArrived) running = false;
    }

    csvFile << staticAgent->moves << ';' << staticAgent->dist << ';'
            << staticAgent->aStarQnt << ';' << staticAgent->processTimeMS << ';'
            << dynamicAgent->moves << ';' << dynamicAgent->dist << ';'
            << dynamicAgent->aStarQnt << ';' << dynamicAgent->processTimeMS << ';'
            << (dynamicAgent->moves == staticAgent->moves ? '0' : dynamicAgent->moves < staticAgent->moves ? '1' : '2') << '\n';

    delete dynamicAgent;
    delete staticAgent;
}

void runTest(DynamicGraph &graph, const int numPolygons, const double polygonRadius) {
    std::ofstream csvFile("resultados_" + std::to_string(numPolygons) + "poligonos_raio" + std::to_string(polygonRadius) + "_tcc.csv");
    csvFile << "TicksStatic;DistStatic;AStarQntStatic;ProcessTimeMSStatic;";
    csvFile << "TicksDynamic;DistDynamic;AStarQntDynamic;ProcessTimeMSDynamic;Result\n";

    for(int i = 0; i < 500; i++) {
        runTest(graph, csvFile, numPolygons, polygonRadius);
        std::cout << "Rodando teste: " << i << '\n';
    }

    csvFile.close();
}

void displayGraph(DynamicGraph &graph, const int numPolygons, const double polygonRadius) {
    for (int i = 0; i < numPolygons; i++) {
        Polygon poly = Polygon::generateHexInGrid(graph.getUniformGrid(), polygonRadius);
        poly.setDraggable(true);
        graph.addPolygon(poly);
    }

    Screen screen;
    screen.drawBackground(graph);

    for (int i = 0; i < 100; i++) {
        std::vector<Agent*> agents = Agent::initAgents(graph);

        bool simulationRunning = true;

        while (screen.windowIsOpen() && simulationRunning) {
            screen.processEvents(graph);
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

        for (auto agent : agents) {
            delete agent;
        }
    }

    while (screen.windowIsOpen()) {
        screen.processEvents(graph);
        screen.update();
        screen.render(graph, {});
    }
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Uso: ./ProjetoConclusaoCurso <arquivo> <test|exhibition> <numPolygons> <radius>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string mode = argv[2];
    int numPolygons = std::stoi(argv[3]);
    double polygonRadius = std::stod(argv[4]);

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " + filename + '\n';
        return 1;
    }

    DynamicGraph graph;
    initGraph(graph, inputFile);

    if (mode == "test") {
        runTest(graph, numPolygons, polygonRadius);
    } else if (mode == "exhibition") {
        displayGraph(graph, numPolygons, polygonRadius);
    } else {
        std::cerr << "Modo inválido! Use 'test' ou 'exhibition'.\n";
        return 1;
    }

    return 0;
}
