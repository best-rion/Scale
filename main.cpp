#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#define HEIGHT 720
#define WIDTH 1080



sf::Vector2f vectorMultScalar(sf::Vector2f v, float s){

    v.x *= s;
    v.y *= s;

    return v;
}


class ImageSprite: public sf::Sprite {

private:

// Variables

    sf::Vector2f spriteScale;

    sf::Texture texture;

// Methods

    void setSpriteScaleFromTexture() {

        sf::Vector2u sizeOfTexture = texture.getSize();

        spriteScale.x = (float) (WIDTH - 2*50) / sizeOfTexture.x;
        spriteScale.y = (float) (HEIGHT - 2*50) / sizeOfTexture.y;

        if(spriteScale.x > spriteScale.y) // minimizing dimension to fit the view
            spriteScale.x = spriteScale.y;
        else
            spriteScale.y = spriteScale.x;

    }

    void setImage(const std::string &filename) {


        if( !texture.loadFromFile(filename) )
            std::cout << "Error while loading image file" << std::endl;

        setSpriteScaleFromTexture();

        this->setTexture(texture);
    }

public:

    ImageSprite(const std::string &filename, sf::Vector2f position) {

        setImage(filename);
        this->setScale(spriteScale);
        this->setPosition(position);

    }

    void updateScale(float normalizedScale) {

        this->setScale(
            vectorMultScalar( spriteScale , normalizedScale )
        );
    }

};



int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");


    sf::Vector2f position(50, 50);
    float normalizedScale = 1.0;
    ImageSprite imageSprite("screw.jpg", position);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            switch (event.type){

                case sf::Event::Closed:

                    window.close();
                    break;

                case sf::Event::MouseWheelScrolled: // Scaling the image on mouse-scroll
                {
                    float zoomRatio = std::pow(1.05/* Zoom-Rate */, -event.mouseWheelScroll.delta);

                    normalizedScale *= zoomRatio; // normalizedScale amar lagbe karon pore eta 1 er niche gele ar zoom out korte debo na.

                    sf::Vector2f distance;

                    distance.x = event.mouseWheelScroll.x - position.x;
                    distance.y = event.mouseWheelScroll.y - position.y;

                    sf::Vector2f newDistance = vectorMultScalar(distance, zoomRatio); // ekhane normalizedScale use kora jabe na karon prottekbar cursor position change hocce

                    position.x = event.mouseWheelScroll.x - newDistance.x;
                    position.y = event.mouseWheelScroll.y - newDistance.y;

                    imageSprite.setPosition(position); // 1. start position changed

                    imageSprite.updateScale(normalizedScale); // 2. scale changed
                    break;
                }
                default:
                    break;

            }
        }

        window.clear();
        window.draw(imageSprite);
        window.display();
    }

    return 0;
}
