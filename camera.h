#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    enum MOVEMENT {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        TURN_LEFT,
        TURN_RIGHT,
        TURN_UP,
        TURN_DOWN,
        UNKNOWN
    };

    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    QMatrix4x4 GetViewMatrix();
    void move(MOVEMENT direction, float deltaTime);
    void turn(float xoffset, float yoffset, bool constrainPitch = true);
    void setZoom(float yoffset);
    float zoom() const;
signals:

private:
    void updateCameraVectors();
    // Default camera values
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;
    const float SPEED       =  2.5f;
    const float SENSITIVITY =  0.1f;
    const float ZOOM        =  45.0f;

    //camera atributes
    QVector3D m_position;
    QVector3D m_front;
    QVector3D m_up;
    QVector3D m_right;
    QVector3D m_worldUp;
    // euler Angles
    float m_yaw;
    float m_pitch;
    // camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
};

#endif // CAMERA_H
