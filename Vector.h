#ifndef VECTOR2_H
#define VECTOR2_H

// Простой 2D вектор класс
class Vector2
{
public:

    // Конструктор
    Vector2();
    Vector2(float a_fValue);
    Vector2(float a_fX, float a_fY);

    // Операторры
    Vector2 operator +(const Vector2 &a_v2);   // '+' оператор
    Vector2 &operator +=(const Vector2 &a_v2); // '+=' operator
    Vector2 operator -(const Vector2 &a_v2);   // '-' operator
    Vector2 &operator -=(const Vector2 &a_v2); // '-=' operator

    bool operator ==(Vector2 &a_v2); // '==' (Equality) operator
    bool operator !=(Vector2 &a_v2); // '!=' (Not equal to) operator


    float X; // X (horizontal) axis
    float Y; // Y (vertical) axis
};

#endif // VECTOR2_H