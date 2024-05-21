#include "widget.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>

#include <cmath>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent), cubePositions({
        QVector3D( 0.0f,  0.0f,  0.0f),
        QVector3D( 2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D( 2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D( 1.3f, -2.0f, -2.5f),
        QVector3D( 1.5f,  2.0f, -2.5f),
        QVector3D( 1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)})
{
    setWindowTitle(QString(QStringLiteral("Camera OpenGL")));
    resize(1280, 960);
}

Widget::~Widget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    delete camera;
    doneCurrent();
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->position());
}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->position()) - mousePressPosition;
    camera->turn(diff.x(), diff.y());
    update();
}

void Widget::timerEvent(QTimerEvent *)
{
    update();
}

void Widget::initializeGL()
{
    qDebug() << size();
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    geometries = new GeometryEngine;
    camera     = new Camera(QVector3D(0.0f, 0.0f, 3.0f));

    // Use QBasicTimer because its faster than QTimer
    timer.start(60, this);
}

void Widget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/assets/shaders/vshader.glsl")))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/assets/shaders/fshader.glsl")))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void Widget::initTextures()
{
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(QStringLiteral(":/assets/textures/cube.png")).mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    float dt = 60.0f / 1000.0f * 2.0f;
    if(event->key() == Qt::Key_W) {
        camera->move(Camera::MOVEMENT::FORWARD, dt);
        update();
    }
    if(event->key() == Qt::Key_S) {
        camera->move(Camera::MOVEMENT::BACKWARD, dt);
        update();
    }
    if(event->key() == Qt::Key_D) {
        camera->move(Camera::MOVEMENT::RIGHT, dt);
        update();
    }
    if(event->key() == Qt::Key_A) {
        camera->move(Camera::MOVEMENT::LEFT, dt);
        update();
    }

    if (event->key() == Qt::Key_Up) {
        camera->move(Camera::MOVEMENT::UP, dt);
        update();
    }
    if (event->key() == Qt::Key_Down) {
        camera->move(Camera::MOVEMENT::DOWN, dt);
        update();
    }
    if (event->key() == Qt::Key_Left) {
        camera->move(Camera::MOVEMENT::TURN_LEFT, dt);
        update();
    }
    if (event->key() == Qt::Key_Right) {
        camera->move(Camera::MOVEMENT::TURN_RIGHT, dt);
        update();
    }

    if (event->key() == Qt::Key_Q) {
        camera->move(Camera::MOVEMENT::TURN_UP, dt);
        update();
    }
    if (event->key() == Qt::Key_E) {
        camera->move(Camera::MOVEMENT::TURN_DOWN, dt);
        update();
    }

    QOpenGLWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
    QOpenGLWidget::keyReleaseEvent(event);
}

void Widget::calculateProjectionMatrix()
{
    // Calculate aspect ratio
    qreal aspect = qreal(size().width()) / qreal(size().height() ? size().height() : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(camera->zoom(), aspect, zNear, zFar);
}

void Widget::wheelEvent(QWheelEvent *event)
{
    camera->setZoom(event->angleDelta().manhattanLength() / 100.0);

    calculateProjectionMatrix();
    update();
}

void Widget::calculateProjectionMatrix(int h, int w)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(camera->zoom(), aspect, zNear, zFar);
}

void Widget::resizeGL(int w, int h)
{
    calculateProjectionMatrix(h, w);
}

void Widget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    texture->bind();
    program.bind();


    for(int i = 0; i < cubePositions.size(); ++i)
    {
        QMatrix4x4 model;
        // Calculate model view transformation
        model.setToIdentity();
        model.translate(cubePositions[i] * 3.0f);
        model.rotate(25.0f * i, 1.0f, 0.3f, 0.5f);
        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * camera->GetViewMatrix() * model);

        // Use texture unit 0 which contains cube.png
        program.setUniformValue("texture", 0);

        // Draw cube geometry
        geometries->drawCubeGeometry(&program);
    }


}
