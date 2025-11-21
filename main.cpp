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

// TODO: melhorar dados armazenados
void runTest(DynamicGraph &graph, std::ofstream &csvFile, const int numPolygons, const int runId) {
    graph.clearPolygons();
    for(int i = 0; i<numPolygons; i++) {
        graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), 0.005));
    }

    const auto agents = Agent::initAgents(graph);
    const Agent* dynamicAgent = agents.at(0);
    const Agent* staticAgent = agents.at(1);

    int ticks = 0;
    bool running = true;
    int collisionsStatic = 0;
    int collisionsDynamic = 0;

    while (running) {
        graph.updatePolygonsPosition();

        for (auto &agent : agents) {
            agent->move(graph);

            // Verifica colisão para métricas
            // (Você precisará expor uma função para checar se o ponto atual está num polígono)
            // Exemplo hipotético:
            // if (graph.checkCollision(agent->getCurrentPosition())) {
            //     if (agent->getType() == Agent::Static) collisionsStatic++;
            //     else collisionsDynamic++;
            // }
        }

        bool dynamicArrived = (dynamicAgent->getCurrentId() == dynamicAgent->getEndId());
        bool staticArrived = (staticAgent->getCurrentId() == staticAgent->getEndId());

        if (dynamicArrived && staticArrived) running = false;
        ticks++;
    }

    csvFile << runId << "," << numPolygons << "," << ticks << ","
            << staticAgent->getPath().size() << "," << dynamicAgent->getPath().size() << ","
            << collisionsStatic << "," << collisionsDynamic << "\n";

    delete dynamicAgent;
    delete staticAgent;
}

void runTest(DynamicGraph &graph) {
    std::ofstream csvFile("resultados_tcc.csv");
    csvFile << "RunID,NumPoligonos,Ticks,DistStatic,DistDynamic,ColisoesStatic,ColisoesDynamic\n";

    // TESTE 1: Baixa Densidade (3 polígonos) - 50 rodadas
    for(int i=0; i<50; i++) {
        runTest(graph, csvFile, 3, i);
        std::cout << "Rodando teste baixa densidade: " << i << "\n";
    }

    // TESTE 2: Alta Densidade (15 polígonos) - 50 rodadas
    for(int i=0; i<50; i++) {
        runTest(graph, csvFile, 15, i + 50);
        std::cout << "Rodando teste alta densidade: " << i << "\n";
    }

    csvFile.close();
}

void displayGraph(DynamicGraph &graph) {
    for (int _ = 0; _ < 5; _++) {
        graph.addPolygon(Polygon::generateHexInGrid(graph.getUniformGrid(), 0.005));
    }

    Screen screen;
    screen.drawBackground(graph);

    for (int i = 0; i < 100; i++) {
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

        for (auto agent : agents) {
            delete agent;
        }
    }

    while (screen.windowIsOpen()) {
        screen.processEvents();
        screen.update();
        screen.render(graph, {});
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: ./ProjetoConclusaoCurso <arquivo-input> <test|exhibition>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string mode = argv[2];

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " + filename + "\n";
        return 1;
    }

    DynamicGraph graph;
    initGraph(graph, inputFile);

    if (mode == "test") {
        runTest(graph);
    } else if (mode == "exhibition") {
        displayGraph(graph);
    } else {
        std::cerr << "Modo inválido! Use 'test' ou 'exhibition'.\n";
        return 1;
    }

    return 0;
}
