# include "Vector.h"
# include "Box.h"
#include <cmath>


// создание линии для камеры
class Line {
public:
    Vector2 start_line;
    Vector2 finish_line;

    Line(float x_0, float y_0, float x_1, float y_1)
    : start_line(x_0, y_0)
    , finish_line(x_1, y_1) {}

    Line(const Vector2& v0, Vector2& v1)
    : start_line(v0)
    , finish_line(v1) {}

    Line() = default;
};

// Класс для создании трапеции (камеры)
class Trapezoid {
private:
    Line front;
    Line back;
    Line left;
    Line right;

    /// \param angle - угол поворота камеры
    /// \param fov - угол обзора камеры
    /// \param near_plane - растояние до ближайшей плоскости
    /// \param far_plane - растояние до дальней плоскости


    Trapezoid(Vector2 position, float angle, float fov, float near_plane, float far_plane) {
        // предполагаем position = 0; angle = 0;
        Vector2 v0(near_plane, tan(fov / 2) * near_plane);
        Vector2 v1(near_plane, -tan(fov / 2) * near_plane);
        Vector2 v2(far_plane, tan(fov / 2) * far_plane);
        Vector2 v3(far_plane, -tan(fov / 2) * far_plane);

        v0 = rotate_and_position(v0, position, angle);
        v1 = rotate_and_position(v1, position, angle);
        v2 = rotate_and_position(v2, position, angle);
        v3 = rotate_and_position(v3, position, angle);

        front = Line(v2, v3);
        back = Line(v0, v1);
        left = Line(v0, v2);
        right = Line(v1, v3);
    }
    Vector2 rotate_and_position(Vector2& vect,
                               Vector2& position,
                               float angle) {
        // преобразование координат при изменении angle
        float x_1 = cos(angle) * vect.X - sin(angle) * vect.Y;
        float y_1 = sin(angle) * vect.X + cos(angle) * vect.Y;
        // преобразование position
        float x_2 = x_1 - position.X;
        float y_2 = x_1 - position.Y;

        return {x_2, y_2};
    };
};