#include "glwidget.h"
#include "widget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    this->xRot = 0.0;
    this->yRot = 0.0;
    this->zRot = 0.0;

    // Включить удаление невидимых граней по-умолчанию
    this->removeInvisibleFaces = true;

    gl_functions_isInitialized = false;

    this->mappingIndex = 0;
    this->textureIndex = 0;

    this->isPerformanceTest = false;
    this->frames = 0;
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    gl_functions = new QOpenGLFunctions_1_3;
    gl_functions->initializeOpenGLFunctions();
    gl_functions_isInitialized = true;

    qDebug() << "OpenGL";
    qDebug() << "\t" << "version:  " << (const char*) gl_functions->glGetString(GL_VERSION);
    qDebug() << "\t" << "vendor:   " << (const char*) gl_functions->glGetString(GL_VENDOR);
    qDebug() << "\t" << "renderer: " << (const char*) gl_functions->glGetString(GL_RENDERER);
    qDebug() << "\t" << "shaders:  " << (const char*) gl_functions->glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Фоновый цвет
    qglClearColor(Qt::black);

    // Тест грубины
    gl_functions->glEnable(GL_DEPTH_TEST);
    gl_functions->glDisable(GL_CULL_FACE);

    // Инициализировать маппинг и текстуру
    this->setMapping();
    this->setTexture();

    //
    // Таймер для подсчёта (вызывается раз в секунду)
    //

    // Каждую секунду...
    timer1.setInterval(1000);
    timer1.setSingleShot(false);

    // ...вызывать onTimer(), чтобы собрать статистику
    connect( &timer1, SIGNAL( timeout() ), this, SLOT( onTimer() ) );


    //
    // Таймер для обновления (вызывается как можно чаще)
    //

    // Каждую мили секунду...
    timer2.setInterval(0);
    timer2.setSingleShot(false);

    // ...вызывать перерисовку
    connect( &timer2, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
}

void GLWidget::paintGL()
{
    gl_functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Матрица модели
    gl_functions->glMatrixMode(GL_MODELVIEW);
    gl_functions->glLoadIdentity();

    QVector3D lightColor( 1.0, 1.0, 1.0 );
    this->drawLight(this->lightPosition, lightColor);

    // Повернуть
    gl_functions->glRotatef( this->xRot, 1.0, 0.0, 0.0);
    gl_functions->glRotatef( this->yRot, 0.0, 1.0, 0.0);
    gl_functions->glRotatef( this->zRot, 0.0, 0.0, 1.0);

    gl_functions->glPushMatrix();

        if( !this->isPerformanceTest )
        {
            // Оси OX, OY, OZ
            this->drawAxes();
        }

        gl_functions->glTranslatef(0.0, 0.0, 0.1);
        gl_functions->glScalef(1.0, 1.0, 1.0);
        gl_functions->glRotatef(0.0, 1.0, 0.0, 0.0);
        gl_functions->glRotatef(0.0, 0.0, 1.0, 0.0);
        gl_functions->glRotatef(0.0, 0.0, 0.0, 1.0);
        gl_functions->glColor4f(1.0, 1.0, 1.0, 1.0);

        this->shaderProgram.bind();

            // Установка текстур
            gl_functions->glActiveTexture( GL_TEXTURE0_ARB );
            gl_functions->glEnable( GL_TEXTURE_2D );
            gl_functions->glBindTexture( GL_TEXTURE_2D, this->decalMap );
            gl_functions->glActiveTexture( GL_TEXTURE1_ARB );
            gl_functions->glEnable( GL_TEXTURE_2D );
            gl_functions->glBindTexture( GL_TEXTURE_2D, this->heightMap );
            gl_functions->glActiveTexture( GL_TEXTURE2_ARB );
            gl_functions->glEnable( GL_TEXTURE_2D );
            gl_functions->glBindTexture( GL_TEXTURE_2D, this->bumpMap );

            this->shaderProgram.setUniformValue( "eye", QVector4D( this->eyePosition ) );
            this->shaderProgram.setUniformValue( "light", QVector4D( this->lightPosition ) );
            this->shaderProgram.setUniformValue( "decalMap",  0 );
            this->shaderProgram.setUniformValue( "heightMap", 1 );

            this->shaderProgram.setUniformValue( "steps", this->steps );
            this->shaderProgram.setUniformValue( "scale", this->scale );

            if( this->mappingIndex == 0 )
            {
                this->shaderProgram.setUniformValue( "bias", this->bias );
            }

            // Нарисовать фигуру
            this->drawFigure();

            // Отключение текстур
            gl_functions->glActiveTexture( GL_TEXTURE0_ARB );
            gl_functions->glDisable( GL_TEXTURE_2D );
            gl_functions->glActiveTexture( GL_TEXTURE1_ARB );
            gl_functions->glDisable( GL_TEXTURE_2D );
            gl_functions->glActiveTexture( GL_TEXTURE2_ARB );
            gl_functions->glDisable( GL_TEXTURE_2D );

        this->shaderProgram.release();

    gl_functions->glPopMatrix();    

    if( this->isPerformanceTest )
    {
        // Увеличить счётчик кадров
        this->frames++;
    }
}

void GLWidget::resizeGL(int width, int height)
{
    this->updateProjection( width, height );
}

void GLWidget::onTimer()
{
    ( (Widget *) this->parentWidget() )->setFPS( this->frames );
    this->frames = 0;
}

void GLWidget::startPerformanceTimers()
{
    this->isPerformanceTest = true;
    gl_functions->glDisable(GL_DEPTH_TEST);
    this->frames = 0;
    this->timer1.start();
    this->timer2.start();
}

void GLWidget::stopPerformanceTimers()
{
    this->isPerformanceTest = false;
    this->timer1.stop();
    this->timer2.stop();
    ( (Widget *) this->parentWidget() )->setFPS( 0 );
    gl_functions->glEnable(GL_DEPTH_TEST);
    this->updateGL();
}

void GLWidget::updateProjection( int width, int height )
{
    if( gl_functions_isInitialized )
    {
        gl_functions->glViewport(0, 0, width, height);

        gl_functions->glMatrixMode(GL_PROJECTION);
        gl_functions->glLoadIdentity();

        GLdouble aspect = (GLdouble) width / (GLdouble) height;
        gluPerspective(30.0, aspect, 0.1, 100.0);

        gluLookAt(
            // координаты позиции глаза налюдателя
            this->eyePosition.x(),
            this->eyePosition.y(),
            this->eyePosition.z(),

            // координаты точки, распологающейся в центре экрана
            0.0, 0.0, 0.0,

            // направление вектора, задающего верх
            0.0, 0.0, 1.0
        );
    }
}

void GLWidget::updateProjection()
{
    this->updateProjection( this->width(), this->height() );
}

void GLWidget::setLightPosition( float x, float y, float z )
{    
    this->lightPosition.setX(x);
    this->lightPosition.setY(y);
    this->lightPosition.setZ(z);

    this->updateGL();
}

void GLWidget::setEyePosition( float x, float y, float z )
{    
    this->eyePosition.setX(x);
    this->eyePosition.setY(y);
    this->eyePosition.setZ(z);

    this->updateProjection();
    this->updateGL();
}

void GLWidget::setSteps( float value )
{
    this->steps = value;
    this->updateGL();
}

void GLWidget::setScale( float value )
{
    this->scale = value;
    this->updateGL();
}

void GLWidget::setBias( float value )
{
    this->bias = value;
    this->updateGL();
}

void GLWidget::setMappingIndex(int index)
{
    this->mappingIndex = index;
    this->updateMapping();
    this->updateGL();
}

void GLWidget::setTextureIndex(int index)
{
    this->textureIndex = index;
    this->setTexture();
    this->updateGL();
}

void GLWidget::setMapping()
{
    this->shaderProgram.removeAllShaders();

    switch( this->mappingIndex )
    {
        // Parallax Mapping
        case 0:
        default:

            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Vertex,
                ":/resources/shaders/parallax-mapping/parallax0.vert"
            );
            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Fragment,
                ":/resources/shaders/parallax-mapping/parallax0.frag"
            );

        break;

        // Steep Parallex Mapping
        case 1:

            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Vertex,
                ":/resources/shaders/parallax-mapping/parallax1.vert"
            );
            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Fragment,
                ":/resources/shaders/parallax-mapping/parallax1.frag"
            );

        break;

        // Relief Mapping
        case 2:

            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Vertex,
                ":/resources/shaders/parallax-mapping/parallax2.vert"
            );
            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Fragment,
                ":/resources/shaders/parallax-mapping/parallax2.frag"
            );

        break;

        // Parallax Occlusion Mapping
        case 3:

            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Vertex,
                ":/resources/shaders/parallax-mapping/parallax3.vert"
            );
            this->shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Fragment,
                ":/resources/shaders/parallax-mapping/parallax3.frag"
            );

        break;


    }

    this->shaderProgram.link();
}

void GLWidget::setTexture()
{
    switch( this->textureIndex )
    {
        // Каменная стена
        case 0:
        default:

            this->decalMap = this->bindTexture( QImage(":/resources/textures/rockwall/rockwall.png") );
            this->bumpMap = this->bindTexture( QImage(":/resources/textures/rockwall/rockwall_normal.png") );

            if( this->mappingIndex == 0 )
            {
                this->heightMap = this->bindTexture( QImage(":/resources/textures/rockwall/rockwall_height.png") );
            }
            else
            {
                this->heightMap = this->bindTexture( QImage(":/resources/textures/rockwall/rockwall_relief.png") );
            }

        break;

        // Дерево
        case 1:

            this->decalMap = this->bindTexture( QImage(":/resources/textures/wood/wood.jpg") );
            this->bumpMap = this->bindTexture( QImage(":/resources/textures/wood/wood_normal.png") );

            if( this->mappingIndex == 0 )
            {
                this->heightMap = this->bindTexture( QImage(":/resources/textures/wood/wood_height.png") );
            }
            else
            {
                this->heightMap = this->bindTexture( QImage(":/resources/textures/wood/wood_relief.png") );
            }

        break;

        // Камень
        case 2:

            this->decalMap = this->bindTexture( QImage(":/resources/textures/rock/rock.jpg") );
            this->bumpMap = 0;
            this->heightMap = this->bindTexture( QImage(":/resources/textures/rock/rock_relief.png") );

        break;

        // Кирпич
        case 3:

            this->decalMap = this->bindTexture( QImage(":/resources/textures/brick/brick.jpg") );
            this->bumpMap = 0;
            this->heightMap = this->bindTexture( QImage(":/resources/textures/brick/brick_relief.png") );

        break;
    }
}

void GLWidget::updateMapping()
{
    if( this->shaderProgram.isLinked() )
    {
        this->setMapping();
        this->setTexture();
    }
    // иначе шейдер ещё не был инициализирован
}

//
// Обработка движений мыши
//

static void qNormalizeAngle(GLfloat &angle)
{
    while (angle < 0)
    {
        angle += 360;
    }

    while (angle > 360)
    {
        angle -= 360;
    }
}

void GLWidget::setXRotation(GLfloat angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot)
    {
        xRot = angle;
        emit xRotationChanged(angle);
        this->updateProjection();
        this->updateGL();
    }
}

void GLWidget::setYRotation(GLfloat angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot)
    {
        yRot = angle;
        emit yRotationChanged(angle);
        this->updateProjection();
        this->updateGL();
    }
}

void GLWidget::setZRotation(GLfloat angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot)
    {
        zRot = angle;
        emit zRotationChanged(angle);
        this->updateProjection();
        this->updateGL();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();

    // Установить фокус в OpenGL виджет, чтобы работал keyPressEvent
    this->setFocusPolicy(Qt::StrongFocus);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat dx = GLfloat( event->x() - lastPos.x() ) / 5.0;
    GLfloat dy = GLfloat( event->y() - lastPos.y() ) / 5.0;

    setXRotation(xRot + dy);

    if(event->buttons() & Qt::LeftButton)
    {
        setYRotation(yRot + dx);
    }
    else if(event->buttons() & Qt::RightButton)
    {
        setZRotation(zRot + dx);
    }

    lastPos = event->pos();
}


// Рисование осей координат
void GLWidget::drawAxes()
{
    // Отключить освещение для этого объекта
    gl_functions->glPushAttrib(GL_LIGHTING_BIT);
        gl_functions->glDisable(GL_LIGHTING);

        float axis_length = 1.0;
        gl_functions->glLineWidth(2.0);

        // OX
        gl_functions->glColor3f(1.0, 0.0, 0.0);
        gl_functions->glBegin(GL_LINES);
            gl_functions->glVertex3f(-axis_length, 0.0, 0.0);
            gl_functions->glVertex3f( axis_length, 0.0, 0.0);
        gl_functions->glEnd();
        gl_functions->glBegin(GL_LINE_STRIP);
            gl_functions->glVertex3f( axis_length - 0.02, -0.02, 0.0);
            gl_functions->glVertex3f( axis_length, 0.0, 0.0);
            gl_functions->glVertex3f( axis_length - 0.02, 0.02, 0.0);
        gl_functions->glEnd();

        // OY
        gl_functions->glColor3f(0.0, 1.0, 0.0);
        gl_functions->glBegin(GL_LINES);
            gl_functions->glVertex3f(0.0, -axis_length, 0.0);
            gl_functions->glVertex3f(0.0, axis_length, 0.0);
        gl_functions->glEnd();
        gl_functions->glBegin(GL_LINE_STRIP);
            gl_functions->glVertex3f(-0.02, axis_length - 0.02, 0.0);
            gl_functions->glVertex3f(0.0, axis_length, 0.0);
            gl_functions->glVertex3f(0.02, axis_length - 0.02, 0.0);
        gl_functions->glEnd();

        // OZ
        gl_functions->glColor3f(0.0, 0.0, 1.0);
        gl_functions->glBegin(GL_LINES);
            gl_functions->glVertex3f(0.0, 0.0, -axis_length);
            gl_functions->glVertex3f(0.0, 0.0, axis_length);
        gl_functions->glEnd();
        gl_functions->glBegin(GL_LINE_STRIP);
            gl_functions->glVertex3f(-0.02, -0.02, axis_length - 0.02);
            gl_functions->glVertex3f(0.0, 0.0, axis_length);
            gl_functions->glVertex3f(0.02, 0.02, axis_length - 0.02);
        gl_functions->glEnd();

    gl_functions->glPopAttrib();
}

// Рисование фигуры
void GLWidget::drawFigure()
{
    float size = 0.75;

    GLfloat normal[] = { 0.0, 0.0, 1.0 };

    GLfloat pos = 1.0;

    GLfloat tex1_coord[] = { pos, 0, 0 };
    GLfloat tex2_coord[] = { 0, pos, 0 };

    GLfloat tex0_coord1[] = { 0, 0 };
    GLfloat tex0_coord2[] = { pos, 0 };
    GLfloat tex0_coord3[] = { pos, pos };
    GLfloat tex0_coord4[] = { 0, pos };

    GLfloat vertex1[] = { -size, -size, 0 };
    GLfloat vertex2[] = { size, -size, 0 };
    GLfloat vertex3[] = { size, size, 0 };
    GLfloat vertex4[] = { -size, size, 0 };

    gl_functions->glBegin( GL_QUADS );

        gl_functions->glNormal3fv( normal );
        gl_functions->glMultiTexCoord3fv( GL_TEXTURE1_ARB, tex1_coord );
        gl_functions->glMultiTexCoord3fv( GL_TEXTURE2_ARB, tex2_coord );

        gl_functions->glMultiTexCoord2fv( GL_TEXTURE0_ARB, tex0_coord1 );
        gl_functions->glVertex3fv( vertex1 );

        gl_functions->glMultiTexCoord2fv( GL_TEXTURE0_ARB, tex0_coord2 );
        gl_functions->glVertex3fv( vertex2 );

        gl_functions->glMultiTexCoord2fv( GL_TEXTURE0_ARB, tex0_coord3 );
        gl_functions->glVertex3fv( vertex3 );

        gl_functions->glMultiTexCoord2fv( GL_TEXTURE0_ARB, tex0_coord4 );
        gl_functions->glVertex3fv( vertex4 );

    gl_functions->glEnd();
}

// Рисование источника света в виде куба с центром x, y, z
// и заданным цветом red, green, blue
void GLWidget::drawLight( QVector3D &position, QVector3D &color )
{
    GLfloat x = position.x();
    GLfloat y = position.y();
    GLfloat z = position.z();

    GLfloat red = color.x();
    GLfloat green = color.y();
    GLfloat blue = color.z();

    gl_functions->glColor3f(red, green, blue);

    GLfloat halfSide = 0.03;

    gl_functions->glBegin(GL_QUADS);

        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z - halfSide);
        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z - halfSide);
        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z - halfSide);
        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z - halfSide);

        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z + halfSide);
        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z + halfSide);

        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z - halfSide);
        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z + halfSide);
        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z - halfSide);

        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z - halfSide);
        gl_functions->glVertex3f( x + halfSide,  y + halfSide, z - halfSide);

        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z - halfSide);
        gl_functions->glVertex3f( x - halfSide,  y + halfSide, z - halfSide);

        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z + halfSide);
        gl_functions->glVertex3f( x + halfSide,  y - halfSide, z - halfSide);
        gl_functions->glVertex3f( x - halfSide,  y - halfSide, z - halfSide);

    gl_functions->glEnd();
}
