#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "geometry/Polygon.h"
#include "graph/DynamicGraph.h"
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

    while (screen.windowIsOpen()){
        screen.processEvents();
        screen.update();
        screen.render(graph);
        graph.updatePolygonsPosition();
    }

    return 0;
}
