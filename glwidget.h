#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

#include <GL/glu.h>

#include <QGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_1_3> // OpenGL 1.3 for support glMultiTexCoord2fv

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void setMappingIndex(int);
    void setTextureIndex(int);

    // Позиции источника света и наблюдателя
    void setLightPosition( float x, float y, float z );
    void setEyePosition( float x, float y, float z );
    void setSteps( float value );
    void setScale( float value );
    void setBias( float value );

    // Управление таймерами
    void startPerformanceTimers();
    void stopPerformanceTimers();

public slots:
    void setXRotation(GLfloat angle);
    void setYRotation(GLfloat angle);
    void setZRotation(GLfloat angle);

    void onTimer();

signals:
    void xRotationChanged(GLfloat angle);
    void yRotationChanged(GLfloat angle);
    void zRotationChanged(GLfloat angle);


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    void mousePressEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );

    QOpenGLFunctions_1_3 *gl_functions;
    bool gl_functions_isInitialized;
    QOpenGLContext *gl_context;

private:
    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    QPoint lastPos;

    QVector3D eyePosition;
    QVector3D lightPosition;

    QVector3D from;

    // Удалять невидимые грани
    bool removeInvisibleFaces;

    // Номер модели маппинга
    int mappingIndex;

    // Номер текстуры
    int textureIndex;

    // Обновление проекции
    void updateProjection( int width, int height );
    void updateProjection();

    // Текстуры для parallax mapping
    GLuint decalMap;
    GLuint heightMap;
    GLuint bumpMap;

    GLint steps;
    GLfloat scale;
    GLfloat bias;

    // Инициализация маппинга
    void setMapping();
    void updateMapping();

    // Инициализация текстуры
    void setTexture();

//    void changeShaderProgram();

    // Инициализация освещения
    void initializeLight();

    // Рисование осей координат
    void drawAxes();

    // Рисование фигуры
    void drawFigure();

    // Рисование источника света в виде куба с центром x, y, z
    // и заданным цветом red, green, blue
    void drawLight( QVector3D &position, QVector3D &color );

    QOpenGLShaderProgram shaderProgram;

    // Таймеры для измерения производительности
    QTimer timer1;
    QTimer timer2;
    uint frames;
    bool isPerformanceTest;
};

#endif // GLWIDGET_H
