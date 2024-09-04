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

sf::Vector2f vectorAddScalar(sf::Vector2f v, float s){

    v.x += s;
    v.y += s;

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

        spriteScale.x = (float) WIDTH / sizeOfTexture.x;
        spriteScale.y = (float) HEIGHT / sizeOfTexture.y;

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

    ImageSprite(const std::string &filename) {

        setImage(filename);
        this->setScale(spriteScale);
        // this->setPosition(50, 50);

    }

};



int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");

    ImageSprite imageSprite("screw.jpg");


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

                    imageSprite.setScale(
                        vectorMultScalar(
                            imageSprite.getScale() ,
                            std::pow(1.05/* Zoom-Rate */, -event.mouseWheelScroll.delta)
                        )
                    );
                    break;

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
