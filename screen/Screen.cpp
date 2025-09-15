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
    this->window.create(sf::VideoMode(this->windowWidth, this->windowHeight), "TCC - Erick");
    this->view.reset(sf::FloatRect(0, 0, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight)));
    this->window.setView(this->view);
    this->backgroundTexture.create(this->windowWidth, this->windowHeight);
    this->backgroundTexture.clear(sf::Color::White);
}

sf::Vector2f Screen::latLonToScreen(const double lon, const double lat, const DynamicGraph &graph) const {
    const double x = (lon - graph.getMinLon()) / (graph.getMaxLon() - graph.getMinLon()) * this->windowWidth;
    const double y = (lat - graph.getMinLat()) / (graph.getMaxLat() - graph.getMinLat()) * this->windowHeight;

    return {static_cast<float>(x), static_cast<float>(this->windowHeight - y)};
}

void Screen::drawGrid(const DynamicGraph &graph) {
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

            sf::Vector2f topLeft = latLonToScreen(lon0, lat1, graph);
            const sf::Vector2f bottomRight = latLonToScreen(lon1, lat0, graph);

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

void Screen::drawPoints(const DynamicGraph &graph) {
    for(auto &[id, point]: graph.getIdToPoint()){
        sf::CircleShape circle(1);
        circle.setFillColor(sf::Color::Blue);
        const sf::Vector2f pos = latLonToScreen(point.getX(), point.getY(), graph);
        circle.setPosition(pos.x - 1, pos.y - 1);
        this->backgroundTexture.draw(circle);
    }
}

void Screen::drawBackground(const DynamicGraph &graph) {
    this->drawGrid(graph);
    // this->drawPoints(graph);

    this->backgroundTexture.display();
    this->background.setTexture(backgroundTexture.getTexture());
}

void Screen::drawEdges(const DynamicGraph &graph) {
    std::set<const Edge *> cellEdges;

    for (const auto &cell : GridHelper::getOccupiedCells(graph.getPolygons(), graph.getUniformGrid())) {
        const auto it = graph.getUniformGrid().getGrid().find(cell);

        if (it != graph.getUniformGrid().getGrid().end()) {
            for (const auto &edge : it->second) {
                if (cellEdges.contains(edge)) continue;

                for (const auto &polygon : graph.getPolygons()) {
                    if (PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge->getU()) ||
                        PointHelper::pointInConvexPolygon(polygon.getPoints(), *edge->getV())) {
                        cellEdges.insert(edge);
                    }
                }
            }
        }
    }

    for (auto &[id, edges]: graph.getAdj()){
        for (auto &edge : edges) {
            sf::Color color = cellEdges.contains(&edge) ? sf::Color::Red : sf::Color::Black;

            const sf::Vertex line[] = {
                sf::Vertex(latLonToScreen(edge.getU()->getX(), edge.getU()->getY(), graph), color),
                sf::Vertex(latLonToScreen(edge.getV()->getX(), edge.getV()->getY(), graph), color)
            };
            this->window.draw(line, 2, sf::Lines);
        }
    }
}

void Screen::drawPolygons(const DynamicGraph &graph) {
    for (auto &poly : graph.getPolygons()) {
        sf::ConvexShape shape;

        const size_t n = poly.getPoints().size();
        shape.setPointCount(n);

        for (size_t i = 0; i < n; i++) {
            sf::Vector2f pos = latLonToScreen(poly.getPoints()[i].getX(), poly.getPoints()[i].getY(), graph);
            shape.setPoint(i, pos);
        }

        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color::Green);
        shape.setOutlineThickness(2.0f);

        this->window.draw(shape);
    }
}

bool Screen::windowIsOpen() const {
    return this->window.isOpen();
}

void Screen::processEvents() {
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
    }
}

void Screen::update() {
    constexpr float panSpeed = 5.0f;

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

void Screen::render(const DynamicGraph &graph) {
    this->window.clear(sf::Color::White);
    this->window.draw(this->background);
    this->drawEdges(graph);
    this->drawPolygons(graph);
    this->window.display();
}
