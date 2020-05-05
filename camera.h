#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 5.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

class Camera
{
public:
    Camera();
    Camera(QVector3D _position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D _up = QVector3D(0.0f, 1.0f, 0.0f),
        float _yaw = YAW, float _pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch);

    QMatrix4x4 GetViewMatrix() const noexcept;
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) noexcept;
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) noexcept;
    void ProcessMouseScroll(float yoffset) noexcept;
    QVector3D getEyePos() const noexcept;

private:
    void updateCameraVectors() noexcept;

    QVector3D position;
    QVector3D front;
    QVector3D up;
    QVector3D right;
    QVector3D worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};

#endif // CAMERA_H
