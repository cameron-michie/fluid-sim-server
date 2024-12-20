#pragma once

struct Vector2f {
    float x;
    float y;

    // Default constructor
    Vector2f() : x(0), y(0) {}

    // Parameterized constructor
    Vector2f(float x, float y) : x(x), y(y) {}

    // Copy constructor
    Vector2f(const Vector2f& other) : x(other.x), y(other.y) {}

    // Assignment operator
    Vector2f& operator=(const Vector2f& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    // Addition operator
    Vector2f operator+(const Vector2f& other) const {
        return Vector2f(x + other.x, y + other.y);
    }

    // Addition assignment operator
    Vector2f& operator+=(const Vector2f& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Subtraction operator
    Vector2f operator-(const Vector2f& other) const {
        return Vector2f(x - other.x, y - other.y);
    }

    // Scalar multiplication operator
    Vector2f operator*(float scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    // Scalar multiplication assignment operator
    Vector2f& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Scalar division operator
    Vector2f operator/(float scalar) const {
        return Vector2f(x / scalar, y / scalar);
    }

    // Scalar division assignment operator
    Vector2f& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Equality operator
    bool operator==(const Vector2f& other) const {
        return x == other.x && y == other.y;
    }

    // Inequality operator
    bool operator!=(const Vector2f& other) const {
        return !(*this == other);
    }

    // Friend function for scalar multiplication with float on the left-hand side
    friend Vector2f operator*(float scalar, const Vector2f& vec) {
        return Vector2f(vec.x * scalar, vec.y * scalar);
    }
};
