#include "camera.h"

Camera::Camera(QObject *parent)
    : QObject{parent}
{

}

Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch)
    : m_front(QVector3D(0.0f, 0.0f, -1.0f)), m_movementSpeed(2.5f), m_mouseSensitivity(0.5f), m_zoom(45.0f)
{
    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

QMatrix4x4 Camera::GetViewMatrix()
{
    QMatrix4x4 matrix;
    matrix.lookAt(m_position, m_position + m_front, m_up);
    return matrix;
}

void Camera::move(MOVEMENT direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;
    if (direction == FORWARD) {
        m_position += m_front * velocity;
    }
    if (direction == BACKWARD) {
        m_position -= m_front * velocity;
    }
    if (direction == LEFT) {
        m_position -= m_right * velocity;
    }
    if (direction == RIGHT) {
        m_position += m_right * velocity;
    }
    if (direction == UP) {
        m_position += m_up * velocity;
    }
    if (direction == DOWN) {
        m_position -= m_up * velocity;
    }
    if (direction == TURN_LEFT) {
        turn(-velocity * 5, 0.0f);
    }
    if (direction == TURN_RIGHT) {
        turn(+velocity * 5, 0.0f);
    }
    if (direction == TURN_UP) {
        turn(0.0f, +velocity * 5);
    }
    if (direction == TURN_DOWN) {
        turn(0.0f, -velocity * 5);
    }
}

void Camera::turn(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::setZoom(float yoffset)
{
    m_zoom -= (float)yoffset;
    if (m_zoom < 1.0f) {
        m_zoom = 1.0f;
    }
    if (m_zoom > 45.0f) {
        m_zoom = 45.0f;
    }
}

float Camera::zoom() const
{
    return m_zoom;
}

void Camera::updateCameraVectors()
{
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    front.setY(qSin(qDegreesToRadians(m_pitch)));
    front.setZ(qSin(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    m_front = front.normalized();

    // also re-calculate the Right and Up vector
    m_right = QVector3D::crossProduct(m_front, m_worldUp).normalized();
    m_up    = QVector3D::crossProduct(m_right, m_front).normalized();
}
