#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#define HEIGHT 720
#define WIDTH 1080

#define NAV_HEIGHT 80
#define MARGIN 10

#define ICON_SIZE 40

double cmPerPixel;

double realScale = 1.0;


sf::Vector2f vectorMultScalar(sf::Vector2f v, float s){
    v.x *= s;
    v.y *= s;
    return v;
}


float distance( sf::Vector2f p1, sf::Vector2f p2 ) {

    return std::sqrt(
        std::pow( ( p1.x - p2.x ), 2 )
        +
        std::pow( ( p1.y - p2.y ), 2 )

    );
}




class ImageSprite: public sf::Sprite {

private:

// Variables

    sf::Vector2f fittingScale; // to fit the view

    sf::Texture texture;

// Methods

    void setfittingScaleFromTexture() {

        sf::Vector2u sizeOfTexture = texture.getSize();

        fittingScale.x = (float) (WIDTH - 2*MARGIN) / sizeOfTexture.x;
        fittingScale.y = (float) (HEIGHT - 2*MARGIN - NAV_HEIGHT) / sizeOfTexture.y;

        if(fittingScale.x > fittingScale.y) // minimizing dimension to fit the view
            fittingScale.x = fittingScale.y;
        else
            fittingScale.y = fittingScale.x;

    }

    void setImage(const std::string &filename) {


        if( !texture.loadFromFile(filename) )
            std::cout << "Error while loading image file" << std::endl;

        setfittingScaleFromTexture();

        setTexture(texture);
    }

public:

// Variables


// Methods

    ImageSprite(const std::string &filename) {

        setImage(filename);
        setPosition( MARGIN, NAV_HEIGHT + MARGIN );
        setScale(fittingScale);

    }

    void updateScale(float zoomRatio) {

        setScale(
            vectorMultScalar( fittingScale , realScale )
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

        setPosition( sf::Vector2f( 50 + 70*buttonNumber,20) );
        setSize( sf::Vector2f( ICON_SIZE, ICON_SIZE ) );
        setTexture(&normal);

        buttonNumber++;
    }

    void setClickedTexture(){
        setTexture(&clicked, true);
    }

    void setNormalTexture(){
        setTexture(&normal, true);
    }

    short getButtonNumber(){
        return buttonNumber;
    }

    bool checkIfClicked( sf::Event event ) {

        sf::Vector2f position = getPosition();

        sf::Vector2f _size = getSize();

        if( event.mouseButton.x > position.x && event.mouseButton.x < (position.x + _size.x)
         && event.mouseButton.y > position.y && event.mouseButton.y < (position.y + _size.y) ) {

            if(!isClicked){
                isClicked=1;
                setClickedTexture();

                return 1;

            }else{
                isClicked=0;
                setNormalTexture();
                return 0;
            }

        }else{
            return 0;
        }
    }

};
short Button::buttonNumber = 0;


class Navbar: public sf::RectangleShape {

public:

    Navbar(float width, float height) {

        setSize( sf::Vector2f(width, height) );
        setFillColor( sf::Color(60,80,90) );
    }
};



class Line{

public:

    sf::Vertex endPoints[2];

    bool firstPointDone;
    bool secondPointDone;

    void setColor(sf::Color color){

        endPoints[0].color= color;
        endPoints[1].color= color;
    }

    Line() {

        setColor( sf::Color(255,0,0) );

        firstPointDone = false;
        secondPointDone = false;
    }

    double getLengthInPX(){

        return distance(endPoints[0].position, endPoints[1].position);
    }


    double getLengthInCM(){

        return getLengthInPX()*cmPerPixel;
    }

    void draw_me( sf::RenderWindow &window ) {

        // Drawing line
        if(secondPointDone){

            window.draw(endPoints,2,sf::Lines);

        }else if( firstPointDone && !secondPointDone ){
            sf::Mouse mouse;
            endPoints[1].position.x = (float) mouse.getPosition(window).x;
            endPoints[1].position.y = (float) mouse.getPosition(window).y;

            window.draw(endPoints,2,sf::Lines);
        }
    }
};

Line measurementLine;

void createMeasurementLine(sf::Event event) {

    if (event.type == sf::Event::MouseButtonPressed){

        if(!measurementLine.firstPointDone){

           measurementLine.endPoints[0].position.x = event.mouseButton.x;
           measurementLine.endPoints[0].position.y = event.mouseButton.y;

           measurementLine.firstPointDone = true;

           measurementLine.secondPointDone = false;

        }else{
           measurementLine.endPoints[1].position.x = event.mouseButton.x;
           measurementLine.endPoints[1].position.y = event.mouseButton.y;

           measurementLine.firstPointDone = false; // firstPoint of the NEXT line is not done

           measurementLine.secondPointDone = true;

           // Showing line length real

           std::cout << "Length of line = " << measurementLine.getLengthInCM() * 10 << " mm" << std::endl;
        }

    }
}

void createUnitLine(sf::Event event, Line &unitLine) {

    if (event.type == sf::Event::MouseButtonPressed){

        if(!unitLine.firstPointDone){
           unitLine.endPoints[0].position.x = event.mouseButton.x;
           unitLine.endPoints[0].position.y = event.mouseButton.y;

           unitLine.firstPointDone = true;

           unitLine.secondPointDone = false;
        }else{
           unitLine.endPoints[1].position.x = event.mouseButton.x;
           unitLine.endPoints[1].position.y = event.mouseButton.y;

           unitLine.firstPointDone = false;

           unitLine.secondPointDone = true;

           // setting Unit

           cmPerPixel = 1.0/unitLine.getLengthInPX();
        }

    }
}



// CIRCLE

class Point: sf::CircleShape {

public:

    sf::Vector2f position;

    Point() {
        setPointCount(4);
        setRadius(4);
        setFillColor(sf::Color(255,255,0));
    }

    void setPosition(sf::Vector2f center) {
        position = center;
        CircleShape::setPosition( sf::Vector2f(center.x-getRadius(), center.y-getRadius()) );
    }

    void draw_me(sf::RenderWindow &window) {
        window.draw(*this);
    }
};



class Circle: sf::CircleShape {

public:

    sf::Vector2f center;
    double radius;

    Point points[3];
    short pointCount;

    Circle() {

        pointCount = 0;

        setOutlineColor(sf::Color(0,255,255));
        setOutlineThickness(2);
        setFillColor(sf::Color(0,0,0,0));
    }



    void calculateCenter() {

        float a1 = -2 * ( points[0].position.x - points[1].position.x );
        float b1 = -2 * ( points[0].position.y - points[1].position.y );
        float c1 =
            std::pow(points[0].position.x, 2)
            -
            std::pow(points[1].position.x, 2)
            +
            std::pow(points[0].position.y, 2)
            -
            std::pow(points[1].position.y, 2);

        float a2 = -2 * ( points[1].position.x - points[2].position.x );
        float b2 = -2 * ( points[1].position.y - points[2].position.y );
        float c2 =
            std::pow(points[1].position.x, 2)
            -
            std::pow(points[2].position.x, 2)
            +
            std::pow(points[1].position.y, 2)
            -
            std::pow(points[2].position.y, 2);
        float x = (b1*c2 - b2*c1) / (a1*b2 - a2*b1);
        float y = (c1*a2 - c2*a1) / (a1*b2 - a2*b1);


        center = sf::Vector2f(x,y);

    }

    void calculateRadius() {

        radius = distance(center, points[0].position);

    }

    double getRadiusInCM(){
        return radius * cmPerPixel;
    }


    void addPoint( sf::Vector2f point ){

        points[pointCount].setPosition( point );
        pointCount++;

        if ( pointCount == 3 ) {

            calculateCenter();
            calculateRadius();

            setRadius(radius);
            setPosition( center - sf::Vector2f(radius, radius) );

            std::cout << "Radius of curve = " << getRadiusInCM()*10 << " mm" << std::endl;


        }
    }


    void drawPoints( sf::RenderWindow &window ) {

        for( short i=0; i<pointCount; i++){
            points[i].draw_me(window);
        }
    }

    void draw_me( sf::RenderWindow &window ) {

        if(pointCount == 3) {

            window.draw(*this);
        }
    }

    void update() {
        setPosition(center - sf::Vector2f(radius, radius));
        setRadius(radius);
    }

};

Circle measurementCircle;


void createCircle( sf::Event event ) {

    if (event.type == sf::Event::MouseButtonPressed){

        measurementCircle.addPoint( sf::Vector2f(event.mouseButton.x, event.mouseButton.y) );

    }

}



int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");

    Button unitButton( "icons/unitNormal.png", "icons/unitClicked.png");
    Button lineButton( "icons/lineNormal.png", "icons/lineClicked.png");
    Button circleButton( "icons/circleNormal.png", "icons/circleClicked.png");

    Navbar navbar( WIDTH, NAV_HEIGHT );

    ImageSprite imageSprite("coin.jpg");

    Line unitLine;
    unitLine.setColor(sf::Color(0,255,0));



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

                    realScale *= zoomRatio;

                    if ( unitLine.secondPointDone ){ // cmPerPixel doesn't exist before unitLine

                        cmPerPixel /= zoomRatio;

                    }

                // UPDATING IMAGE WHILE ZOOMING


                    sf::Vector2f distance;
                    distance.x = event.mouseWheelScroll.x - imageSprite.getPosition().x;
                    distance.y = event.mouseWheelScroll.y - imageSprite.getPosition().y;

                    sf::Vector2f newDistance = vectorMultScalar(distance, zoomRatio);

                    imageSprite.setPosition(
                        event.mouseWheelScroll.x - newDistance.x,
                        event.mouseWheelScroll.y - newDistance.y
                    ); // 1. start position changed

                    imageSprite.updateScale(zoomRatio); // 2. scale changed


                // UPDATING UNIT-LINE WHILE ZOOMING

                    for(short i=0; i<2; i++){

                        distance.x = event.mouseWheelScroll.x - unitLine.endPoints[i].position.x;
                        distance.y = event.mouseWheelScroll.y - unitLine.endPoints[i].position.y;

                        newDistance = vectorMultScalar(distance, zoomRatio);

                        unitLine.endPoints[i].position.x = event.mouseWheelScroll.x - newDistance.x;
                        unitLine.endPoints[i].position.y = event.mouseWheelScroll.y - newDistance.y;
                    }

                // UPDATING UNIT-LINE WHILE ZOOMING

                    for(short i=0; i<2; i++){

                        distance.x = event.mouseWheelScroll.x - measurementLine.endPoints[i].position.x;
                        distance.y = event.mouseWheelScroll.y - measurementLine.endPoints[i].position.y;

                        newDistance = vectorMultScalar(distance, zoomRatio);

                        measurementLine.endPoints[i].position.x = event.mouseWheelScroll.x - newDistance.x;
                        measurementLine.endPoints[i].position.y = event.mouseWheelScroll.y - newDistance.y;
                    }


                // UPDATING Point WHILE ZOOMING

                    for(short i=0; i<measurementCircle.pointCount; i++){

                        distance.x = event.mouseWheelScroll.x - measurementCircle.points[i].position.x;
                        distance.y = event.mouseWheelScroll.y - measurementCircle.points[i].position.y;

                        newDistance = vectorMultScalar(distance, zoomRatio);

                        measurementCircle.points[i].setPosition(
                            sf::Vector2f(
                                event.mouseWheelScroll.x - newDistance.x,
                                event.mouseWheelScroll.y - newDistance.y
                            )
                        );
                    }

                // UPDATING Circle WHILE ZOOMING

                    // center

                    distance.x = event.mouseWheelScroll.x - measurementCircle.center.x;
                    distance.y = event.mouseWheelScroll.y - measurementCircle.center.y;

                    newDistance = vectorMultScalar(distance, zoomRatio);

                    measurementCircle.center.x = event.mouseWheelScroll.x - newDistance.x;
                    measurementCircle.center.y = event.mouseWheelScroll.y - newDistance.y;

                    // radius

                    measurementCircle.radius *= zoomRatio;
                    measurementCircle.update();
                    break;
                }

                case sf::Event::MouseButtonPressed:
                {
                    bool unitButtonHasBeenClicked = unitButton.checkIfClicked(event);
                    bool lineButtonHasBeenClicked = lineButton.checkIfClicked(event);
                    bool circleButtonHasBeenClicked = circleButton.checkIfClicked(event);

                    if ( unitButton.isClicked && !unitButtonHasBeenClicked ) {
                        createUnitLine(event, unitLine);
                    }

                    if ( lineButton.isClicked && !lineButtonHasBeenClicked ) {
                        createMeasurementLine(event);
                    }

                    if ( circleButton.isClicked && !circleButtonHasBeenClicked ) {
                        createCircle(event);
                    }

                    break;
                }
                default:
                    break;

            }
        }

        window.clear();
        window.draw(imageSprite);

        unitLine.draw_me(window);
        measurementLine.draw_me(window);
        measurementCircle.draw_me(window);

        measurementCircle.drawPoints(window);

        window.draw(navbar);
        window.draw(unitButton);
        window.draw(lineButton);
        window.draw(circleButton);


        window.display();
    }

    return 0;
}
