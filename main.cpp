#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#define HEIGHT 720
#define WIDTH 1080

#define NAV_HEIGHT 80
#define MARGIN 10

#define ICON_SIZE 40

sf::Color unitColor(0,255,0),
            lineColor(255,0,0),
            circleColor(0,255,255),
            angleColor(255,0,255);



double cmPerPixel;

double dynamicScale = 1.0;

float zoom;


sf::Vector2f vectorXscalar(sf::Vector2f v, float s)
{
    v.x *= s;
    v.y *= s;
    return v;
}



float sq(float a) {return a*a;}



float distance( sf::Vector2f p1, sf::Vector2f p2 )
{
    return std::sqrt( sq(p1.x-p2.x) + sq(p1.y-p2.y) );
}



float slopeAngle(sf::Vector2f p1, sf::Vector2f p2)
{
    if (p1.x - p2.x == 0)
        return std::atan(INFINITY);
    else
        return std::atan( (p1.y-p2.y) / (p1.x-p2.x) );
}



class ImageSprite: public sf::Sprite
{

private:

// Variables

    sf::Vector2f staticScale; // to fit the view

    sf::Texture texture;

// Methods

    void setStaticScaleFromTexture()
    {

        sf::Vector2u sizeOfTexture = texture.getSize();

        staticScale.x = (float) (WIDTH - 2*MARGIN) / sizeOfTexture.x;
        staticScale.y = (float) (HEIGHT - 2*MARGIN - NAV_HEIGHT) / sizeOfTexture.y;

        if(staticScale.x > staticScale.y) // minimizing dimension to fit the view
            staticScale.x = staticScale.y;
        else
            staticScale.y = staticScale.x;

    }

    void setImage(const std::string &filename) {


        if( !texture.loadFromFile(filename) )
            std::cout << "Error while loading image file" << std::endl;

        setStaticScaleFromTexture();

        setTexture(texture);
    }

public:

// Variables


// Methods

    ImageSprite(const std::string &filename) {

        setImage(filename);
        setPosition( MARGIN, NAV_HEIGHT + MARGIN );
        setScale(staticScale);

    }

    void updateScale(float zoom) {

        setScale( vectorXscalar(staticScale ,dynamicScale) );
    }

};





//.. RESETABLE INTERFACE ..//

class Resetable
{

public:

    virtual void reset() = 0;
};





class Button: public sf::RectangleShape
{

private:

// Variables

    sf::Texture imageTexture;

    static short buttonNumber;

    sf::Color color;

// Methods

    void setClickedColor() { this->setFillColor(color); }

    void setNormalColor() { this->setFillColor(sf::Color(255,255,255)); }

public:

// Variables

    bool isClicked;


// Methods

    Button(const std::string &filename, sf::Color color)
    {

        this->color = color;

        isClicked = false;

        if( !imageTexture.loadFromFile(filename) )
            std::cout << "Error while loading image file for imageTexture" << std::endl;

        setPosition( sf::Vector2f( 50 + 70*buttonNumber,20) );
        setSize( sf::Vector2f( ICON_SIZE, ICON_SIZE ) );
        setTexture(&imageTexture);

        buttonNumber++;
    }


    bool checkIfClicked( sf::Event event, Resetable &resetableObject )
    {
        sf::Vector2f position = getPosition();

        sf::Vector2f _size = getSize();

        if( event.mouseButton.x > position.x && event.mouseButton.x < (position.x + _size.x)
         && event.mouseButton.y > position.y && event.mouseButton.y < (position.y + _size.y) )
        {


            if(!isClicked)
            {
                isClicked = true;
                setClickedColor();

                resetableObject.reset(); // Reset

                return 1;
            }
            else
            {
                isClicked = false;
                setNormalColor();
                return 0;
            }

        }
        else
        {
            return 0;
        }
    }

};
short Button::buttonNumber = 0;


class Navbar: public sf::RectangleShape
{
public:

    Navbar(float width, float height)
    {
        setSize( sf::Vector2f(width, height) );
        setFillColor( sf::Color(60,80,90) );
    }
};



class Line: public Resetable
{
public:

    sf::Vertex endPoints[2];

    bool firstPointDone;
    bool secondPointDone;

    void reset()
    {
        firstPointDone = false;
        secondPointDone = false;
    }

    void setColor(sf::Color color)
    {
        endPoints[0].color= color;
        endPoints[1].color= color;
    }

    Line( sf::Color color )
    {
        setColor( color );

        reset();
    }

    double getLengthInPX()
    {
        return distance(endPoints[0].position, endPoints[1].position);
    }


    double getLengthInCM()
    {
        return getLengthInPX()*cmPerPixel;
    }

    void draw_me( sf::RenderWindow &window )
    {
        // Drawing line
        if(secondPointDone)
        {
            window.draw(endPoints,2,sf::Lines);
        }
        else if( firstPointDone && !secondPointDone )
        {
            sf::Mouse mouse;
            endPoints[1].position.x = (float) mouse.getPosition(window).x;
            endPoints[1].position.y = (float) mouse.getPosition(window).y;

            window.draw(endPoints,2,sf::Lines);
        }
    }
};

Line line( lineColor );

void createLine(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if(!line.firstPointDone)
        {
           line.endPoints[0].position.x = event.mouseButton.x;
           line.endPoints[0].position.y = event.mouseButton.y;

           line.firstPointDone = true;

           line.secondPointDone = false;
        }
        else
        {
           line.endPoints[1].position.x = event.mouseButton.x;
           line.endPoints[1].position.y = event.mouseButton.y;

           line.firstPointDone = false;

           line.secondPointDone = true;

           // Showing line length
           std::cout << "Length of line = " << line.getLengthInCM() * 10 << " mm" << std::endl;
        }
    }
}



Line unit( unitColor );

void createUnit(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if(!unit.firstPointDone)
        {
           unit.endPoints[0].position.x = event.mouseButton.x;
           unit.endPoints[0].position.y = event.mouseButton.y;

           unit.firstPointDone = true;

           unit.secondPointDone = false;
        }
        else
        {
           unit.endPoints[1].position.x = event.mouseButton.x;
           unit.endPoints[1].position.y = event.mouseButton.y;

           unit.firstPointDone = false;

           unit.secondPointDone = true;

           // setting Unit
           cmPerPixel = 1.0/unit.getLengthInPX();
        }
    }
}



// CIRCLE

class Point: public sf::CircleShape
{
public:

    sf::Vector2f position;

    Point()
    {
        setPointCount(4);
        setRadius(4);
        setFillColor(sf::Color(255,255,0));
    }

    void setPosition(sf::Vector2f center)
    {
        position = center;
        CircleShape::setPosition( sf::Vector2f(center.x-getRadius(), center.y-getRadius()) );
    }

    void draw_me(sf::RenderWindow &window)
    {
        window.draw(*this);
    }
};



class Circle: public sf::CircleShape, public Resetable
{
public:

// Variables

    sf::Vector2f center;
    double radius;

    Point points[3];
    short pointCount;

    Point centerPoint;

// Methods

    void reset()
    {
        pointCount = 0;
    }

    Circle()
    {
        reset();

        setOutlineColor( circleColor );
        setOutlineThickness(2);
        setFillColor(sf::Color(0,0,0,0));
    }



    void calculateCenter()
    {

        float a1 = -2 * ( points[0].position.x - points[1].position.x );
        float b1 = -2 * ( points[0].position.y - points[1].position.y );
        float c1 = // x1^2 - x2^2 + y1^2 - y2^2
            sq(points[0].position.x) - sq(points[1].position.x)
            +
            sq(points[0].position.y) - sq(points[1].position.y);



        float a2 = -2 * ( points[1].position.x - points[2].position.x );
        float b2 = -2 * ( points[1].position.y - points[2].position.y );
        float c2 = // x2^2 - x3^2 + y2^2 - y3^2
            sq(points[1].position.x) - sq(points[2].position.x)
            +
            sq(points[1].position.y) - sq(points[2].position.y);



        float x = (b1*c2 - b2*c1) / (a1*b2 - a2*b1);
        float y = (c1*a2 - c2*a1) / (a1*b2 - a2*b1);


        center = sf::Vector2f(x,y);
    }

    void calculateRadius()
    {
        radius = distance(center, points[0].position);
    }

    double getRadiusInCM()
    {
        return radius * cmPerPixel;
    }


    void addPoint( sf::Vector2f point )
    {
        points[pointCount].setPosition( point );
        pointCount++;

        if ( pointCount == 3 )
        {
            calculateCenter();
            calculateRadius();

            setRadius(radius);
            setPosition( center - sf::Vector2f(radius, radius) );


            // Showing circle's radius
            std::cout << "Radius of curve = " << getRadiusInCM()*10 << " mm" << std::endl;

            // Initialize centerPoint
            centerPoint.setPosition(center);
            centerPoint.setFillColor(sf::Color(255,127,0));
        }
    }


    void drawPoints( sf::RenderWindow &window )
    {
        for( short i=0; i<pointCount; i++)
            points[i].draw_me(window);
    }

    void draw_me( sf::RenderWindow &window )
    {
        if(pointCount == 3)
        {
            window.draw(*this);

            centerPoint.draw_me(window);
        }
        drawPoints(window);
    }

    void update()
    {
        setPosition(center - sf::Vector2f(radius, radius));
        setRadius(radius);
    }
};

Circle circle;

void createCircle( sf::Event event )
{
    if (event.type == sf::Event::MouseButtonPressed)
        circle.addPoint( sf::Vector2f(event.mouseButton.x, event.mouseButton.y) );
}

class Angle: public Resetable
{

public:

// Variables

    float angle;

    sf::Vertex verteces[3];

    short vertexCount;

    bool firstPointDone, secondPointDone, thirdPointDone;

// Methods

    void reset()
    {
        vertexCount = 0;

        firstPointDone = false;
        secondPointDone = false;
        thirdPointDone = false;
    }

    Angle()
    {
        reset();

        for (short i=0; i<3; i++)
            verteces[i].color = angleColor;
    }

    void calculateAngle()
    {
        float slopeAngle01 = slopeAngle(verteces[0].position, verteces[1].position);
        float slopeAngle12 = slopeAngle(verteces[1].position, verteces[2].position);

        angle = std::abs(slopeAngle12 - slopeAngle01);

        angle = angle * 180 / 3.14159;

        if ( // c^2 < a^2 + b^2 [acute angle] (pythagoras)

            sq( distance(verteces[0].position, verteces[2].position) )
            <
            sq( distance(verteces[0].position, verteces[1].position) )
            +
            sq( distance(verteces[1].position, verteces[2].position) )
        )
        { // Then it is an Acute angle

            if (angle > 90)
                angle = 180 - angle; // Less than 90 degree
        }
        else
        { // Else it is an Obtuse angle

            if (angle < 90)
                angle = 180 - angle; // More than 90 degree
        }
    }


    void addPoint( sf::Vector2f position )
    {
        verteces[vertexCount].position = position;
        vertexCount++;

        if ( vertexCount == 3 )
        {
            thirdPointDone = true;

            calculateAngle();

            // Showing Angle
            std::cout << "Angle = " << angle << " Degree" << std::endl;
        }
        else if ( vertexCount == 1 )
        {
            firstPointDone = true;
        }
        else if ( vertexCount == 2 )
        {
            secondPointDone = true;
        }
    }

    void draw_me( sf::RenderWindow &window )
    {
        sf::Mouse mouse;

        // Drawing line

        if (thirdPointDone)
        {
            window.draw(verteces,2,sf::Lines);
            window.draw(verteces+1,2,sf::Lines);
        }
        else if(firstPointDone && !secondPointDone)
        {
            verteces[1].position.x = (float) mouse.getPosition(window).x;
            verteces[1].position.y = (float) mouse.getPosition(window).y;

            window.draw(verteces,2,sf::Lines);
        }
        else if( secondPointDone && !thirdPointDone )
        {
            verteces[2].position.x = (float) mouse.getPosition(window).x;
            verteces[2].position.y = (float) mouse.getPosition(window).y;

            window.draw(verteces,2,sf::Lines);
            window.draw(verteces+1,2,sf::Lines);
        }
    }
};

Angle angle;

void createAngle( sf::Event event )
{
    if (event.type == sf::Event::MouseButtonPressed)
        angle.addPoint( sf::Vector2f(event.mouseButton.x, event.mouseButton.y) );
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scale");

    Button unitButton( "icons/unitNormal.png", unitColor);
    Button lineButton( "icons/lineNormal.png", lineColor);
    Button circleButton( "icons/circleNormal.png", circleColor);
    Button angleButton( "icons/angleNormal.png", angleColor);

    Navbar navbar( WIDTH, NAV_HEIGHT );

    ImageSprite imageSprite("coin.jpg");


    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:

                    window.close();
                    break;

                case sf::Event::MouseWheelScrolled: // Scaling the image on mouse-scroll
                {

                    zoom = std::pow(1.05/* Zoom-Rate */, -event.mouseWheelScroll.delta);

                    dynamicScale *= zoom;

                    if ( unit.secondPointDone ) // cmPerPixel doesn't exist before unit
                        cmPerPixel /= zoom;

                    sf::Vector2f distance, newDistance;

                // UPDATING IMAGE WHILE ZOOMING

                    distance.x = event.mouseWheelScroll.x - imageSprite.getPosition().x;
                    distance.y = event.mouseWheelScroll.y - imageSprite.getPosition().y;

                    newDistance = vectorXscalar(distance, zoom);

                    imageSprite.setPosition(
                        event.mouseWheelScroll.x - newDistance.x,
                        event.mouseWheelScroll.y - newDistance.y
                    ); // 1. start position changed

                    imageSprite.updateScale(zoom); // 2. scale changed


                // UPDATING UNIT-LINE WHILE ZOOMING

                    for(short i=0; i<2; i++){

                        distance.x = event.mouseWheelScroll.x - unit.endPoints[i].position.x;
                        distance.y = event.mouseWheelScroll.y - unit.endPoints[i].position.y;

                        newDistance = vectorXscalar(distance, zoom);

                        unit.endPoints[i].position.x = event.mouseWheelScroll.x - newDistance.x;
                        unit.endPoints[i].position.y = event.mouseWheelScroll.y - newDistance.y;
                    }

                // UPDATING MEASUREMENT-LINE WHILE ZOOMING

                    for(short i=0; i<2; i++){

                        distance.x = event.mouseWheelScroll.x - line.endPoints[i].position.x;
                        distance.y = event.mouseWheelScroll.y - line.endPoints[i].position.y;

                        newDistance = vectorXscalar(distance, zoom);

                        line.endPoints[i].position.x = event.mouseWheelScroll.x - newDistance.x;
                        line.endPoints[i].position.y = event.mouseWheelScroll.y - newDistance.y;
                    }


                // UPDATING Point WHILE ZOOMING

                    for(short i=0; i<circle.pointCount; i++){

                        distance.x = event.mouseWheelScroll.x - circle.points[i].position.x;
                        distance.y = event.mouseWheelScroll.y - circle.points[i].position.y;

                        newDistance = vectorXscalar(distance, zoom);

                        circle.points[i].setPosition(
                            sf::Vector2f(
                                event.mouseWheelScroll.x - newDistance.x,
                                event.mouseWheelScroll.y - newDistance.y
                            )
                        );
                    }

                // UPDATING Circle WHILE ZOOMING

                    // center

                    distance.x = event.mouseWheelScroll.x - circle.center.x;
                    distance.y = event.mouseWheelScroll.y - circle.center.y;

                    newDistance = vectorXscalar(distance, zoom);

                    circle.center.x = event.mouseWheelScroll.x - newDistance.x;
                    circle.center.y = event.mouseWheelScroll.y - newDistance.y;

                    // radius

                    circle.radius *= zoom;
                    circle.update();


                    // centerPoint

                    circle.centerPoint.setPosition( circle.center );


                // UPDATING Angle Lines WHILE ZOOMING

                    for(short i=0; i<3; i++){

                        distance.x = event.mouseWheelScroll.x - angle.verteces[i].position.x;
                        distance.y = event.mouseWheelScroll.y - angle.verteces[i].position.y;

                        newDistance = vectorXscalar(distance, zoom);

                        angle.verteces[i].position.x = event.mouseWheelScroll.x - newDistance.x;
                        angle.verteces[i].position.y = event.mouseWheelScroll.y - newDistance.y;
                    }


                    break;
                }

                case sf::Event::MouseButtonPressed:
                {
                    bool unitButtonHasBeenClicked = unitButton.checkIfClicked( event, unit );
                    bool lineButtonHasBeenClicked = lineButton.checkIfClicked( event, line );
                    bool circleButtonHasBeenClicked = circleButton.checkIfClicked( event, circle );
                    bool angleButtonHasBeenClicked = angleButton.checkIfClicked( event, angle );

                    if ( unitButton.isClicked && !unitButtonHasBeenClicked )
                        createUnit(event);

                    if ( lineButton.isClicked && !lineButtonHasBeenClicked )
                        createLine(event);

                    if ( circleButton.isClicked && !circleButtonHasBeenClicked )
                        createCircle(event);

                    if ( angleButton.isClicked && !angleButtonHasBeenClicked )
                        createAngle(event);

                    break;
                }
                default:
                    break;

            }
        }

        window.clear();

        // IMAGE
        window.draw(imageSprite);

        // LINES
        unit.draw_me(window);
        line.draw_me(window);
        circle.draw_me(window);
        angle.draw_me(window);

        // NAV
        window.draw(navbar);

        // BUTTONS
        window.draw(unitButton);
        window.draw(lineButton);
        window.draw(circleButton);
        window.draw(angleButton);

        window.display();
    }

    return 0;
}
