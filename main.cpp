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

    sf::Vector2f spriteScale;

    sf::Texture texture;

    void setSpriteScale(sf::Image image) {

        sf::Vector2u sizeOfImage = image.getSize();

        spriteScale.x = (float) WIDTH / sizeOfImage.x;
        spriteScale.y = (float) HEIGHT / sizeOfImage.y;

        if(spriteScale.x > spriteScale.y) { // minimizing dimension to fit the view

            spriteScale.x = spriteScale.y;

        }else{

            spriteScale.y = spriteScale.x;

        }
    }


    sf::Image getImage(const std::string &filename) {

        sf::Image image;
        image.loadFromFile(filename);

        setSpriteScale(image);

        return image;
    }

    sf::Texture getTexture(const std::string &filename) {

        sf::Image image = getImage(filename);

        sf::Texture texture;
        texture.loadFromImage(image);

        return texture;
    }

public:

    ImageSprite(const std::string &filename) {

        texture = getTexture(filename);
        this->setTexture(texture);
        this->setScale(spriteScale);

    }

};



int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");

    ImageSprite imageSprite("10001.jpg");


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

                    imageSprite.setScale( vectorMultScalar( imageSprite.getScale() ,  std::pow(1.01, event.mouseWheelScroll.delta * -1) ) );

            }
        }

        window.clear();
        window.draw(imageSprite);
        window.display();
    }

    return 0;
}
