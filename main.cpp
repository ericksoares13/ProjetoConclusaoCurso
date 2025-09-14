#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "geometry/Polygon.h"
#include "graph/DynamicGraph.h"
#include "screen/Screen.h"

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo input.txt\n";
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

    Screen screen;
    screen.drawBackground(graph);

    Polygon h = Polygon::generateHexInGrid(graph.getUniformGrid(), 0.005);

    for (auto &p : h.getVertices()) {
        std::cout << "(" << p.getY() << ", " << p.getX() << ")\n";
    }

    while (screen.windowIsOpen()){
        screen.processEvents();
        screen.update();
        screen.render();
    }

    return 0;
}
