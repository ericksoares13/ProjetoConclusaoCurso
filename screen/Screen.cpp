//
// Created by erick on 14/09/2025.
//

#include "Screen.h"
#include "../helper/GridHelper.h"

#include <set>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>


Screen::Screen() {
    // Inicia a tela
    this->window.create(sf::VideoMode(this->windowWidth, this->windowHeight), "TCC - Erick");
    this->view.reset(sf::FloatRect(0, 0, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight)));
    this->window.setView(this->view);
    this->backgroundTexture.create(this->windowWidth, this->windowHeight);
    this->backgroundTexture.clear(sf::Color::White);
}

// Converte as coordenadas geográficas para as cartesianas da tela
sf::Vector2f Screen::latLonToScreen(const DynamicGraph &graph, const double lon, const double lat) const {
    const double x = (lon - graph.getMinLon()) / (graph.getMaxLon() - graph.getMinLon()) * this->windowWidth;
    const double y = (lat - graph.getMinLat()) / (graph.getMaxLat() - graph.getMinLat()) * this->windowHeight;

    return {static_cast<float>(x), static_cast<float>(this->windowHeight - y)};
}

// Converte as coordenadas cartesianas da tela para as geográficas
sf::Vector2f Screen::screenToLatLon(const DynamicGraph &graph, const float screenX, const float screenY) const {
    const float normalizedX = screenX / static_cast<float>(this->windowWidth);
    const float normalizedY = 1.0f - (screenY / static_cast<float>(this->windowHeight));

    const double lon = graph.getMinLon() + normalizedX * (graph.getMaxLon() - graph.getMinLon());
    const double lat = graph.getMinLat() + normalizedY * (graph.getMaxLat() - graph.getMinLat());

    return {static_cast<float>(lon), static_cast<float>(lat)};
}

// Desenha a grade uniforme na textura de fundo fixa
void Screen::drawBackgroundGrid(const DynamicGraph &graph) {
    const int iMin = floor(graph.getMinLon() / graph.getCellSize());
    const int iMax = floor(graph.getMaxLon() / graph.getCellSize());
    const int jMin = floor(graph.getMinLat() / graph.getCellSize());
    const int jMax = floor(graph.getMaxLat() / graph.getCellSize());

    for (int i = iMin; i <= iMax; i++) {
        for (int j = jMin; j <= jMax; j++) {
            Cell c(i,j);

            const double lon0 = i * graph.getCellSize();
            const double lat0 = j * graph.getCellSize();
            double lon1 = lon0 + graph.getCellSize();
            double lat1 = lat0 + graph.getCellSize();

            if (lon1 > graph.getMaxLon()) lon1 = graph.getMaxLon();
            if (lat1 > graph.getMaxLat()) lat1 = graph.getMaxLat();

            sf::Vector2f topLeft = latLonToScreen(graph, lon0, lat1);
            const sf::Vector2f bottomRight = latLonToScreen(graph, lon1, lat0);

            sf::RectangleShape cellShape(sf::Vector2f(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y));
            cellShape.setPosition(topLeft);

            if (!graph.getUniformGrid().getGrid().contains(c) || graph.getUniformGrid().getGrid().find(c)->second.empty()) {
                cellShape.setFillColor(sf::Color(255, 200, 200, 200));
            } else {
                cellShape.setFillColor(sf::Color::Transparent);
            }

            cellShape.setOutlineColor(sf::Color(200, 200, 200));
            cellShape.setOutlineThickness(1);
            this->backgroundTexture.draw(cellShape);
        }
    }
}

// Desenha as arestas na textura de fundo fixa
void Screen::drawBackgroundEdges(const DynamicGraph &graph) {
    for (auto &[id, edges]: graph.getAdj()){
        for (const auto &edge : edges) {
            const sf::Vertex line[] = {
                sf::Vertex(latLonToScreen(graph, edge.getU()->getX(), edge.getU()->getY()), sf::Color::Black),
                sf::Vertex(latLonToScreen(graph, edge.getV()->getX(), edge.getV()->getY()), sf::Color::Black)
            };
            this->backgroundTexture.draw(line, 2, sf::Lines);
        }
    }
}

// Desenha o background (textura fixa de fundo)
void Screen::drawBackground(const DynamicGraph &graph) {
    this->drawBackgroundGrid(graph);
    this->drawBackgroundEdges(graph);

    this->backgroundTexture.display();
    this->background.setTexture(backgroundTexture.getTexture());
}

// Desenha de vermelho as arestas que possuem interseção com os polígonos
// Utiliza a grade unifrome para acelerar esse cálculo
void Screen::drawEdges(const DynamicGraph &graph) {
    for (const auto &cell : GridHelper::getOccupiedCells(graph.getPolygons(), graph.getUniformGrid())) {
        const auto it = graph.getUniformGrid().getGrid().find(cell);

        if (it != graph.getUniformGrid().getGrid().end()) {
            for (const auto edge : it->second) {
                for (const auto &polygon : graph.getPolygons()) {
                    if (PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge->getU()) ||
                        PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge->getV())) {
                        const sf::Vertex line[] = {
                            sf::Vertex(latLonToScreen(graph, edge->getU()->getX(), edge->getU()->getY()), sf::Color::Red),
                            sf::Vertex(latLonToScreen(graph, edge->getV()->getX(), edge->getV()->getY()), sf::Color::Red)
                        };
                        this->window.draw(line, 2, sf::Lines);
                    }
                }
            }
        }
    }
}

// Desenha os poígonos
void Screen::drawPolygons(const DynamicGraph &graph) {
    for (auto &poly : graph.getPolygons()) {
        sf::ConvexShape shape;

        const size_t n = poly.getPoints().size();
        shape.setPointCount(n);

        for (size_t i = 0; i < n; i++) {
            sf::Vector2f pos = latLonToScreen(graph, poly.getPoints()[i].getX(), poly.getPoints()[i].getY());
            shape.setPoint(i, pos);
        }

        if (poly.getDragging()) {
            shape.setFillColor(sf::Color(255, 0, 0, 100));
        } else {
            shape.setFillColor(sf::Color::Transparent);
        }

        shape.setOutlineColor(sf::Color::Green);
        shape.setOutlineThickness(2.0f);

        this->window.draw(shape);
    }
}

// Desenha os agentes com os seus percursos e origem/destino
void Screen::drawAgents(const DynamicGraph& graph, const std::vector<Agent*>& agents) {
    for (auto &agent : agents) {
        if (agent->getPath().size() < 2) continue;

        sf::Color pathColor = agent->getType() == Agent::Dynamic ? sf::Color::Magenta : sf::Color::Blue;

        for (size_t i = 0; i < agent->getPath().size() - 1; i++) {
            long long id1 = agent->getPath()[i];
            long long id2 = agent->getPath()[i + 1];

            const Point* p1 = &graph.getIdToPoint().at(id1);
            const Point* p2 = &graph.getIdToPoint().at(id2);

            sf::RectangleShape pathSegment;
            sf::Vector2f start = latLonToScreen(graph, p1->getX(), p1->getY());
            sf::Vector2f end = latLonToScreen(graph, p2->getX(), p2->getY());

            sf::Vector2f direction = end - start;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length;

            pathSegment.setSize(sf::Vector2f(length, 3.0f));
            pathSegment.setPosition(start);
            pathSegment.setRotation(static_cast<float>(std::atan2(direction.y, direction.x) * 180 / M_PI));
            pathSegment.setFillColor(pathColor);

            this->window.draw(pathSegment);
        }
    }

    for (auto &agent : agents) {
        sf::CircleShape agentShape(6.0f);
        sf::Vector2f screenPos = latLonToScreen(graph, agent->getCurrentPosition().getX(), agent->getCurrentPosition().getY());
        agentShape.setPosition(screenPos.x - 6, screenPos.y - 6);

        sf::Color agentColor = agent->getType() == Agent::Dynamic ? sf::Color::Red : sf::Color::Cyan;
        agentShape.setFillColor(agentColor);
        agentShape.setOutlineColor(sf::Color::Black);
        agentShape.setOutlineThickness(1.0f);

        this->window.draw(agentShape);
    }

    if (agents.empty()) return;

    const long long destId = agents[0]->getEndId();
    if (destId == -1 || !graph.getIdToPoint().contains(destId)) {
        return;
    }

    const Point& dest = graph.getIdToPoint().at(destId);
    const sf::Vector2f screenPos = latLonToScreen(graph, dest.getX(), dest.getY());

    sf::CircleShape destShape(8.0f);
    destShape.setPosition(screenPos.x - 8, screenPos.y - 8);
    destShape.setFillColor(sf::Color::Green);
    destShape.setOutlineColor(sf::Color::Black);
    destShape.setOutlineThickness(2.0f);

    this->window.draw(destShape);
}

Polygon* getPolygonAtPosition(const DynamicGraph &graph, const float worldX, const float worldY) {
    for (auto& polygons = const_cast<std::vector<Polygon>&>(graph.getPolygons()); auto& poly : polygons) {
        if (poly.getDraggable() && poly.containsPoint(worldX, worldY)) {
            return &poly;
        }
    }

    return nullptr;
}

// Lida com o movimento dos polígonos via mouse
void Screen::handleMouseDrag(const DynamicGraph &graph, const sf::Event& event) {
    const sf::Vector2i mousePixelPos = sf::Mouse::getPosition(this->window);
    const sf::Vector2f mouseWorldPos = this->window.mapPixelToCoords(mousePixelPos, this->view);

    const sf::Vector2f mouseLatLon = screenToLatLon(graph, mouseWorldPos.x, mouseWorldPos.y);

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            this->draggedPolygon = getPolygonAtPosition(graph, mouseLatLon.x, mouseLatLon.y);
            if (this->draggedPolygon) {
                this->draggedPolygon->setDragging(true);
                this->lastMousePos = mouseLatLon;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && this->draggedPolygon) {
            this->draggedPolygon->setDragging(false);
            this->draggedPolygon = nullptr;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (this->draggedPolygon && this->draggedPolygon->getDragging()) {
            const sf::Vector2f currentMousePos = mouseLatLon;
            this->draggedPolygon->moveTo(currentMousePos.x, currentMousePos.y);
            this->lastMousePos = currentMousePos;
        }
    }
}

bool Screen::windowIsOpen() const {
    return this->window.isOpen();
}

// Processa os eventos de entrada do usuário
void Screen::processEvents(const DynamicGraph &graph) {
    sf::Event event{};

    while (this->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            this->window.close();
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                this->view.zoom(0.9f);
            } else {
                this->view.zoom(1.1f);
            }

            this->window.setView(this->view);
        }

        this->handleMouseDrag(graph, event);
    }
}

// Atualiza as informações lidas do usuário
void Screen::update() {
    constexpr float panSpeed = 0.5f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        this->view.move(-panSpeed, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        this->view.move(panSpeed, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        this->view.move(0, -panSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        this->view.move(0, panSpeed);
    }

    this->window.setView(this->view);
}

// Renderiza a tela com todas as informações do ciclo atual desenhadas
void Screen::render(const DynamicGraph &graph, const std::vector<Agent*>& agents) {
    this->window.clear(sf::Color::White);
    this->window.draw(this->background);
    this->drawEdges(graph);
    this->drawPolygons(graph);
    this->drawAgents(graph, agents);
    this->window.display();
}
