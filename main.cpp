#include <SFML/Graphics.hpp>
#include <iostream>

#define HEIGHT 720
#define WIDTH 1080


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

    ImageSprite imageSprite("screw.jpg");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(imageSprite);
        window.display();
    }

    return 0;
}

/*



int main()
{
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Scale");

    sf::Texture texture;

    if( !texture.loadFromFile("screw.jpg") ) {
        // Handle an error.
        std::cout << "Can't load the image" << std::endl;
    }

    sf::Sprite sprite(texture);

    sprite.setScale(0.3,0.3);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
*/
