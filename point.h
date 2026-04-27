#ifndef POINT_H
#define POINT_H

class Point
{
public:
    Point(float x = 0, float y = 0, float z = 0,
          float rotX = 0, float rotY = 0, float rotZ = 0);
    float getX() const {return x;}
    float getY() const {return y;}
    float getZ() const {return z;}
    float getRotX() const {return rotX;}
    float getRotY() const {return rotY;}
    float getRotZ() const {return rotZ;}

    void setPosition(float newX, float newY, float newZ);
    void move(float dx, float dy, float dz);
    void rotate(float drotx, float droty, float drotz);

private:
    float x, y, z;
    float rotX, rotY, rotZ;
};

#endif // POINT_H
