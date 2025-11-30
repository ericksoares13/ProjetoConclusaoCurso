//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_SCREEN_H
#define PROJETOCONCLUSAOCURSO_SCREEN_H
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include "Agent.h"
#include "../graph/DynamicGraph.h"


class Screen {
    // Tamanho da tela
    const int windowWidth = 900;
    const int windowHeight = 900;

    // Textura de fundo (grade uniforme e arestas) que é sempre a mesma
    sf::RenderTexture backgroundTexture;
    sf::RenderWindow window;
    sf::Sprite background;
    sf::View view;

    Polygon* draggedPolygon = nullptr;
    sf::Vector2f lastMousePos;

    sf::Vector2f latLonToScreen(const DynamicGraph &graph, double lon, double lat) const;
    sf::Vector2f screenToLatLon(const DynamicGraph &graph, float screenX, float screenY) const;

    void drawBackgroundGrid(const DynamicGraph &graph);
    void drawBackgroundEdges(const DynamicGraph &graph);
    void drawEdges(const DynamicGraph &graph);
    void drawPolygons(const DynamicGraph &graph);
    void drawAgents(const DynamicGraph& graph, const std::vector<Agent*>& agents);

    void handleMouseDrag(const DynamicGraph &graph, const sf::Event& event);

public:
    Screen();

    void drawBackground(const DynamicGraph &graph);
    bool windowIsOpen() const;
    void processEvents(const DynamicGraph &graph);
    void update();
    void render(const DynamicGraph &graph, const std::vector<Agent*>& agents);
};


#endif //PROJETOCONCLUSAOCURSO_SCREEN_H