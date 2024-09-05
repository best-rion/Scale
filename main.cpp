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

sf::Vector2f vectorSubstract(sf::Vector2f v1, sf::Vector2f v2){

    return sf::Vector2f(v1.x-v2.x, v1.y-v2.y);
}


class ImageSprite: public sf::Sprite {

private:

// Variables

    sf::Vector2f fittingScale; // to fit the view

    sf::Texture texture;

// Methods

    void setfittingScaleFromTexture() {

        sf::Vector2u sizeOfTexture = texture.getSize();

        fittingScale.x = (float) (WIDTH - 2*150) / sizeOfTexture.x;
        fittingScale.y = (float) (HEIGHT - 2*150) / sizeOfTexture.y;

        if(fittingScale.x > fittingScale.y) // minimizing dimension to fit the view
            fittingScale.x = fittingScale.y;
        else
            fittingScale.y = fittingScale.x;

    }

    void setImage(const std::string &filename) {


        if( !texture.loadFromFile(filename) )
            std::cout << "Error while loading image file" << std::endl;

        setfittingScaleFromTexture();

        this->setTexture(texture);
    }

public:

// Variables

    float normalizedScale;

// Methods

    ImageSprite(const std::string &filename) {

        normalizedScale = 1.0;

        setImage(filename);
        this->setPosition(150, 150);
        this->setScale(fittingScale);

    }

    void updateScale(float zoomRatio) {

        normalizedScale *= zoomRatio;

        this->setScale(
            vectorMultScalar( fittingScale , normalizedScale )
        );
    }

};



class Button: public sf::RectangleShape {

private:

// Variables

    sf::Texture normal, clicked;

    static short buttonNumber;

public:

// Variables

    bool isClicked;

    Button(const std::string &normalFilename, const std::string &clickedFilename ) {

        isClicked = 0;

        if( !clicked.loadFromFile(clickedFilename) )
            std::cout << "Error while loading image file for clicked" << std::endl;
        if( !normal.loadFromFile(normalFilename) )
            std::cout << "Error while loading image file for normal" << std::endl;
        this->setPosition( sf::Vector2f(100*(buttonNumber+1),25) );
        this->setSize( sf::Vector2f(50,50) );
        this->setTexture(&normal);

        buttonNumber++;
        std::cout <<buttonNumber<< std::endl;

    }

    void setClickedTexture(){
        this->setTexture(&clicked, true);
    }

    void setNormalTexture(){
        this->setTexture(&normal, true);
    }

    short getButtonNumber(){
        return buttonNumber;
    }

};
short Button::buttonNumber = 0;


class Navbar: public sf::RectangleShape {

public:

    Navbar(float width, float height) {

        this->setSize( sf::Vector2f(width, height) );
        this->setFillColor( sf::Color(100,100,100) );
    }
};


int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");

    Button button1( "unitNormal.png", "unitClicked.png");
    Button button2( "images.jpeg", "screw.jpg");
    Navbar navbar( WIDTH, 100 );


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
                {
                    float zoomRatio = std::pow(1.05/* Zoom-Rate */, -event.mouseWheelScroll.delta);


                    sf::Vector2f distance;
                    distance.x = event.mouseWheelScroll.x - imageSprite.getPosition().x;
                    distance.y = event.mouseWheelScroll.y - imageSprite.getPosition().y;

                    sf::Vector2f newDistance = vectorMultScalar(distance, zoomRatio);

                    imageSprite.setPosition(
                        event.mouseWheelScroll.x - newDistance.x,
                        event.mouseWheelScroll.y - newDistance.y
                    ); // 1. start position changed

                    imageSprite.updateScale(zoomRatio); // 2. scale changed
                    break;
                }

                case sf::Event::MouseButtonPressed:

                    if(event.mouseButton.x >100 && event.mouseButton.x <150
                    && event.mouseButton.y >25 && event.mouseButton.y <75 ){

                        if(!button1.isClicked){
                            button1.isClicked=1;
                            button1.setClickedTexture();
                        }else{
                            button1.isClicked=0;
                            button1.setNormalTexture();
                        }

                    }
                    break;

                default:
                    break;

            }
        }

        window.clear();
        window.draw(imageSprite);
        window.draw(navbar);
        window.draw(button1);
        window.draw(button2);
        window.display();
    }

    return 0;
}
