#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "geometryengine.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QBasicTimer>
#include "camera.h"

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();
    void initTextures();

    // QWidget interface
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    void calculateProjectionMatrix(int h, int w);
    void calculateProjectionMatrix();
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries = nullptr;
    QOpenGLTexture *texture = nullptr;
    QMatrix4x4 projection;
    QVector2D mousePressPosition;

    Camera* camera;
    const QVector<QVector3D> cubePositions;
};
#endif // WIDGET_H
