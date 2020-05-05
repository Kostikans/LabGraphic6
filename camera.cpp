#include <QtMath>
#include "camera.h"

Camera::Camera(QVector3D _position, QVector3D _up, float _yaw, float _pitch)
    : front(QVector3D(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch)
    : front(QVector3D(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = QVector3D(posX, posY, posZ);
    worldUp = QVector3D(upX, upY, upZ);
    yaw = _yaw;
    pitch = _pitch;
    updateCameraVectors();
}

QMatrix4x4 Camera::GetViewMatrix() const noexcept
{
    QMatrix4x4 viewMatrix;
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(position, position + front, up);
    return viewMatrix;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) noexcept
{
    float velocity = movementSpeed * deltaTime;

    if (direction == FORWARD)
        position += front * velocity;

    if (direction == BACKWARD)
        position -= front * velocity;

    if (direction == LEFT)
        position -= right * velocity;

    if (direction == RIGHT)
        position += right * velocity;

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) noexcept
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) noexcept
{
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;
}

QVector3D Camera::getEyePos() const noexcept
{
    return position;
}

void Camera::updateCameraVectors() noexcept
{
    QVector3D _front;
    _front.setX(cosf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch)));
    _front.setY(sinf(qDegreesToRadians(pitch)));
    _front.setZ(sinf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch)));

    front = _front;
    front.normalize();

    right = QVector3D::crossProduct(front, worldUp);
    right.normalize();

    up = QVector3D::crossProduct(right, front);
    up.normalize();
}
