// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Sol2D/Rect.h>
#include <vector>
#include <optional>
#include <cmath>

namespace Sol2D::Utils {

class Rotation
{
public:
    enum Unit
    {
        Radian,
        Degree
    };

public:
    Rotation(double _value, Unit _unit);
    Rotation(double _sine, double _cosine);
    bool isRotated() const { return m_is_rotated; }
    double getRadians() const;
    double getDegrees() const;
    double getSine() const;
    double getCosine() const;

private:
    bool m_is_rotated;
    mutable std::optional<double> m_radians;
    mutable std::optional<double> m_degrees;
    mutable std::optional<double> m_sine;
    mutable std::optional<double> m_cosine;
};

class VectorRotator
{
public:
    explicit VectorRotator(const Rotation & _rotation) :
        m_sine(_rotation.getSine()),
        m_cosine(_rotation.getCosine())
    {
    }

    template<PointConcept Vector>
    Vector rotate(const Vector & _vector)
    {
        return Vector
        {
            .x = static_cast<float>(m_cosine * _vector.x - m_sine * _vector.y),
            .y  = static_cast<float>(m_sine * _vector.x + m_cosine * _vector.y)
        };
    }

private:
    const double m_sine;
    const double m_cosine;
};

inline double degreesToRadians(double _degrees)
{
    return _degrees == 0.0 ? 0.0 : (_degrees * std::numbers::pi) / 180.0;
}

inline double radiansToDegrees(double _radians)
{
    return _radians == 0.0 ? 0.0 : (_radians * 180.0) / std::numbers::pi;
}

void rotateVectors(std::vector<Point> & _vectors, const Rotation & _rotation);

} // namespace Sol2D::Utils
