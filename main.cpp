#include <SFML/Graphics.hpp>
#include <iostream>

#include "IOAddons.h"
#include "MapSystem.h"

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

    bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
    {
        sf::Image image;

        if (!image.loadFromFile(tileset))
            return false;

        image.createMaskFromColor(sf::Color(255, 0, 255));
        m_tileset.loadFromImage(image);

        // resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);

        // populate the vertex array, with one quad per tile
        for (unsigned int i = 0; i < width; ++i)
            for (unsigned int j = 0; j < height; ++j)
            {
                // get the current tile number
                int tileNumber = tiles[i + j * width];

                // find its position in the tileset texture
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

                // define its 4 corners
                quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                // define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
                quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
            }

        return true;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};

int main()
{
    //loading map
    MapSystem *mapSystem = new MapSystem;

    if (mapSystem->loadMap("C:\\Counter-Strike 2D\\maps\\de_dust2.map") > 0)
        return -1;

    int i = 0;
    int ROW_COUNT = mapSystem->mapWidth + 1;
    int COL_COUNT = mapSystem->mapHeight + 1;
    int array1D[ROW_COUNT * COL_COUNT];
    for (int col = 0; col < COL_COUNT; col++)
    {
        for (int row = 0; row < ROW_COUNT; row++)
        {
            array1D[i] = mapSystem->tileFrame[row][col];
            i++;
        }
    }
    // create the window
    sf::RenderWindow window(sf::VideoMode(640, 480), "Vertex", sf::Style::Close);
    window.setFramerateLimit(50);

    sf::View view(sf::FloatRect(0, 0, 640, 480));

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("C:\\Counter-Strike 2D\\gfx\\tiles\\" + mapSystem->tilesetFileName, sf::Vector2u(32, 32), array1D, ROW_COUNT, COL_COUNT))
        return -1;

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            view.move(0, -8);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            view.move(-8, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            view.move(0, 8);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            view.move(8, 0);
        }

        // draw the map
        window.clear();
        //view.setCenter(0, 0);
        window.setView(view);
        window.draw(map);
        //sf::View currentView = window.getView();
        window.display();
    }

    return 0;
}
