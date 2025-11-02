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
    const int windowWidth = 900;
    const int windowHeight = 900;

    sf::RenderTexture backgroundTexture;
    sf::RenderWindow window;
    sf::Sprite background;
    sf::View view;

    sf::Vector2f latLonToScreen(double lon, double lat, const DynamicGraph &graph) const;
    void drawBackgroundGrid(const DynamicGraph &graph);
    void drawBackgroundEdges(const DynamicGraph &graph);
    void drawEdges(const DynamicGraph &graph);
    void drawPolygons(const DynamicGraph &graph);
    void drawAgents(const DynamicGraph& graph, const std::vector<Agent*>& agents);

public:
    Screen();

    void drawBackground(const DynamicGraph &graph);
    bool windowIsOpen() const;
    void processEvents();
    void update();
    void render(const DynamicGraph &graph, const std::vector<Agent*>& agents);
};


#endif //PROJETOCONCLUSAOCURSO_SCREEN_H