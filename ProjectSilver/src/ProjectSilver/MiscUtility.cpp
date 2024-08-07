#include <ProjectSilver/MiscUtility.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{

    double Math::GetAngleDifference(double firstDirection, double secondDirection)
    {
        double delta = abs(secondDirection - firstDirection);

        if (delta > 180.0)
        {
            if (secondDirection > 180.0)
            {
                delta = -delta;
            }
        }
        else
        {
            if (secondDirection - firstDirection < 0.0)
            {
                delta = -delta;
            }
        }
        return delta;
    }

    const Vector2 Vector2::Zero = {0, 0};

    Vector2::Vector2() : x(0), y(0)
    {
    }

    Vector2::Vector2(double x, double y) : x(x), y(y)
    {
    }

    Vector2::Vector2(int x, int y) : x(double(x)), y(double(y))
    {
    }

    double Vector2::Length()
    {
        return sqrt(x * x + y * y);
    }

    //  Calculates angle from OY- to Vector,
    double Vector2::Angle()
    {
        return atan2(y, x) * 180 / Math::PI + ((y < 0 && x < 0) ? 450.0 : 90.0);
    }

    int Utility::ClampValue(int val, int min, int max)
    {
        return val < min ? min : (val >= max ? max : val);
    }

    double Utility::ClampValue(double val, double min, double max)
    {
        return val < min ? min : (val >= max ? max : val);
    }

    double Utility::ScrollValue(double val, double leftBorder, double rightBorder)
    {
        double dist = rightBorder - leftBorder;
        if (dist <= 0.0)
        {
            return val;
        }
        while (val > rightBorder)
        {
            val -= dist;
        }
        while (val < leftBorder)
        {
            val += dist;
        }
        return val;
    }

    bool Utility::InRange(int val, int min, int max)
    {
        return val >= min && val <= max;
    }

    // Reminder: direction is from North, clockwise, starting at 0
    Vector2 Vector2::NormalVector(double direction)
    {
        return Vector2(sin(Math::DegToRads(direction)),
                       cos(Math::DegToRads(direction + 180)));
    }

    Transform::Transform() : position(0.0, 0.0), center(0.0, 0.0), direction(0.0)
    {
    }

    Transform::Transform(const Vector2& position, const Vector2& center, double direction) :
    position(position),
    center(center),
    direction(direction)
    {
    }
} // namespace Game