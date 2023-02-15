#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cmath>
#include <iostream>

// instead of second vector could be used touples
using Lines = std::vector<std::vector<sf::Vertex>>;
Lines lines;
sf::CircleShape light_source;
sf::RenderWindow window;

using Polygons = std::vector<std::vector<sf::Vector2f>>;

Polygons polygons;

void createWindow() {
  const int width = 2560;
  const int height = 1440;
  sf::VideoMode screen(width, height);
  window.create(screen, "Ray Casting Demo") /* sf::Style::Fullscreen) */;
  window.setFramerateLimit(60);
}

void eventSystem() {
  sf::Event event;
  while (window.pollEvent(event)) {
    switch (event.type) {
    case sf::Event::Closed:
      window.close();
      break;
    case sf::Event::KeyPressed: {
      switch (event.key.code) {
      case sf::Keyboard::Escape:
        window.close();
        break;
      default:
        break;
      }
      break;
    }
    case sf::Event::MouseButtonPressed: {
      sf::Event::MouseButtonEvent mouse = event.mouseButton;
      std::cout << mouse.x << " " << mouse.y << std::endl;
      break;
    }
    case sf::Event::MouseMoved: {
      sf::Event::MouseMoveEvent mouse = event.mouseMove;
      light_source.setPosition(mouse.x, mouse.y);
      break;
    }
    default:
      break;
    }
  }
}

void initPolygons() {
  polygons = {
      {
          sf::Vector2f(230, 100),
          sf::Vector2f(125, 245),
          sf::Vector2f(215, 345),
          sf::Vector2f(330, 125),
      },
      {
          sf::Vector2f(390, 385),
          sf::Vector2f(150, 460),
          sf::Vector2f(460, 510),
      },
      {
          sf::Vector2f(740, 550),
          sf::Vector2f(533, 620),
          sf::Vector2f(720, 650),
      },
  };
}

std::vector<sf::Vertex> line(2);
void initLines() {
  float theta;
  float dx;
  float dy;
  for (const auto &poly : polygons) {
    const int size = poly.size();
    for (int i = 0; i < size; i++) {
      line[0] = sf::Vertex(poly[i]);
      if (i == size - 1)
        line[1] = sf::Vertex(poly[0]);
      else
        line[1] = sf::Vertex(poly[i + 1]);
      lines.emplace_back(line);
    }
  }

  line[0].position = sf::Vector2f(0, window.getSize().y);
  line[1].position = sf::Vector2f(0, window.getSize().y);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(0, 0);
  line[1].position = sf::Vector2f(0, 0);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(window.getSize().x, 0);
  line[1].position = sf::Vector2f(window.getSize().x, 0);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(window.getSize().x, window.getSize().y);
  line[1].position = sf::Vector2f(window.getSize().x, window.getSize().y);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(0, 0);
  line[1].position = sf::Vector2f(0, window.getSize().y);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(0, window.getSize().y);
  line[1].position = sf::Vector2f(window.getSize().x, window.getSize().y);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(window.getSize().x, window.getSize().y);
  line[1].position = sf::Vector2f(window.getSize().x, 0);
  lines.emplace_back(line);

  line[0].position = sf::Vector2f(window.getSize().x, 0);
  line[1].position = sf::Vector2f(0, 0);
  lines.emplace_back(line);
}

void initLightSource() {
  const int radius = 15;
  light_source.setRadius(radius);
  light_source.setOrigin(radius, radius);
  const sf::Vector2u wsize = window.getSize();
  light_source.setPosition(wsize.x / 2.f, wsize.y / 2.f);
}

void drawSinglePoly(std::vector<sf::Vector2f> vertices_list) {
  const int size = vertices_list.size();
  for (int i = 0; i < size; i++) {
    sf::Vertex line[2] = {sf::Vertex(vertices_list[i]),
                          sf::Vertex(vertices_list[i + 1])};
    if (i == size - 1)
      line[1] = sf::Vertex(vertices_list[0]);
    window.draw(line, 2, sf::Lines);
  }
}

void drawPolygons() {
  for (const auto &poly : polygons)
    drawSinglePoly(poly);
}

sf::Vector2f subtractVectors(sf::Vector2f a, sf::Vector2f b) {
  const float x = (b.x - a.x);
  const float y = (b.y - a.y);
  return sf::Vector2f(x, y);
}

float cross(sf::Vector2f a, sf::Vector2f b) { return a.x * b.y - a.y * b.x; }

struct Intersect {
  bool result;
  sf::Vector2f pos;
};

Intersect lineIntersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c,
                        sf::Vector2f d) {
  sf::Vector2f r = subtractVectors(a, b);
  sf::Vector2f s = subtractVectors(c, d);
  float rxs = cross(r, s);
  sf::Vector2f cma = subtractVectors(a, c);
  float t = cross(cma, s) / rxs;
  float u = cross(cma, r) / rxs;
  if (t > 0 && t < 1 && u > 0 && u < 1)
    return {true, sf::Vector2f(a.x + (t * r.x), a.y + (t * r.y))};
  else
    return {false, sf::Vector2f(0, 0)};
}

float dist(const sf::Vector2f &lhs, const sf::Vector2f &rhs) {
  return sqrt(pow((lhs.x - rhs.x), 2) + pow(lhs.y - rhs.y, 2));
}

int main(void) {
  sf::CircleShape IntersectPoint;
  IntersectPoint.setRadius(5);
  IntersectPoint.setOrigin(IntersectPoint.getRadius(),
                           IntersectPoint.getRadius());
  createWindow();
  initPolygons();
  initLines();
  initLightSource();
  const float MARGIN = 0.00005;
  float theta, dx, dy;
  while (window.isOpen()) {
    eventSystem();

    window.clear(sf::Color::Black);

    // [DRAWING]
    // drawPolygons();

    // for (const auto &poly : polygons) {
    //   for (const auto &vertex : poly) {
    //     sf::Vector2f light_pos = light_source.getPosition();
    //     // Intersect inter = lineIntersect(light_pos, vertex, );
    //     sf::Vertex line[] = {sf::Vertex(vertex), sf::Vertex(light_pos)};
    //     line[0].color = sf::Color(200, 0, 0);
    //     line[1].color = sf::Color(200, 0, 0);
    //     window.draw(line, 2, sf::Lines);
    //   }
    // }
    for (const auto &line : lines) {
      // Draw Polygons
      sf::Vertex l[] = {line[0], line[1]};
      window.draw(l, 2, sf::Lines);
      sf::Vector2f light_pos = light_source.getPosition();
      sf::Vector2f destination = line[0].position;

      sf::Vertex left_connection[] = {sf::Vertex(destination),
                                      sf::Vertex(light_pos)};

      dx = destination.x - light_pos.x;
      dy = destination.y - light_pos.y;
      theta = std::atan2(dy, dx);

      left_connection[0].position.x =
          20 * dx + destination.x + cos(theta + MARGIN);
      left_connection[0].position.y =
          20 * dy + destination.y + sin(theta + MARGIN);

      left_connection[0].color = sf::Color(200, 0, 0);
      left_connection[1].color = sf::Color(200, 0, 0);

      sf::Vertex right_connection[] = {sf::Vertex(destination),
                                       sf::Vertex(light_pos)};

      right_connection[0].position.x =
          20 * dx + destination.x + cos(theta - MARGIN);
      right_connection[0].position.y =
          20 * dy + destination.y + sin(theta - MARGIN);

      right_connection[0].color = sf::Color(200, 0, 0);
      right_connection[1].color = sf::Color(200, 0, 0);

      // dist(light_source.getPosition(),
      // destination); // TODO: optimize this calculation
      for (const auto &other_line : lines) {
        if (other_line[0].position == line[0].position)
          continue;

        Intersect inter =
            lineIntersect(light_pos, line[0].position, other_line[0].position,
                          other_line[1].position);

        if (inter.result && dist(light_source.getPosition(), destination) >
                                dist(light_source.getPosition(), inter.pos)) {
          destination = inter.pos;
          IntersectPoint.setPosition(destination);
        }
        // window.draw(IntersectPoint);

        Intersect left_inter =
            lineIntersect(light_pos, left_connection[0].position,
                          other_line[0].position, other_line[1].position);

        if (left_inter.result &&
            dist(light_source.getPosition(), left_connection[0].position) >
                dist(light_source.getPosition(), left_inter.pos)) {

          left_connection[0].position = left_inter.pos;
          IntersectPoint.setPosition(left_connection[0].position);
        }

        Intersect right_inter =
            lineIntersect(light_pos, right_connection[0].position,
                          other_line[0].position, other_line[1].position);

        if (right_inter.result &&
            dist(light_source.getPosition(), right_connection[0].position) >
                dist(light_source.getPosition(), right_inter.pos)) {

          right_connection[0].position = right_inter.pos;
          IntersectPoint.setPosition(right_connection[0].position);
        }

        // window.draw(IntersectPoint);
      }

      sf::Vertex connection[] = {sf::Vertex(destination),
                                 sf::Vertex(light_pos)};

      connection[0].color = sf::Color(200, 0, 0);
      connection[1].color = sf::Color(200, 0, 0);

      window.draw(connection, 2, sf::Lines);
      window.draw(left_connection, 2, sf::Lines);
      window.draw(right_connection, 2, sf::Lines);
    }

    window.draw(light_source);
    window.display();
  }
  return 0;
}

// could be done bug resolution for the left/right connections
// you will increase the scalar only if they don't collide as they are
// perhaps the solution could be just handling better the collision with the
// right and left ones
// the issues is that the collision is not detected 'cause the destination of
// the right or left is a bit more to the left or right, so that won't lie on
// the line for that reason the collision if found only on the next line which
// could be in the polygon
