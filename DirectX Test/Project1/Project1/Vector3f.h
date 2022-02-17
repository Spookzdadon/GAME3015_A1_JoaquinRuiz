#ifndef _VECTOR_3F_H_
#define _VECTOR_3F_H_

class Vector3f
{
private:
    float x, y, z;

public:
    Vector3f();

    Vector3f(const float x, const float y, const float z);

    float getX() const;

    float getY() const;

    float getZ() const;

    void setX(const float value);

    void setY(const float value);

    void setZ(const float value);

    void setValues(const float x, const float y, const float z);

    Vector3f operator + (const Vector3f& vector) const;

    Vector3f operator - (const Vector3f& vector) const;

    Vector3f operator * (const float scale) const;

    Vector3f operator / (const float scale) const;

    Vector3f crossProduct(const Vector3f& vector) const;

    float innerProduct(const Vector3f& vector) const;

    float length() const;

    Vector3f operator - () const;

    void opposite();

    Vector3f normalized() const;

    void normalize();
};

#endif // _VECTOR_3F_H_
