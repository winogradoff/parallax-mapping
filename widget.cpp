#include "widget.h"
#include "ui_widget.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi( this );
    this->setWindowTitle( "Виноградов Иван — Курсовая — Parallax mapping" );

    // Спрятать FPS и TPS по-умолчанию
    this->ui->label_fps->setVisible(false);
    this->ui->label_fps_value->setVisible(false);


    // Сохранить все значения по-умолчанию с формы
    this->ui_lampX = this->ui->doubleSpinBox_lampX->value();
    this->ui_lampY = this->ui->doubleSpinBox_lampY->value();
    this->ui_lampZ = this->ui->doubleSpinBox_lampZ->value();
    this->ui->widget_canvas->setLightPosition( this->ui_lampX, this->ui_lampY, this->ui_lampZ );

    this->ui_eyeX = this->ui->doubleSpinBox_eyeX->value();
    this->ui_eyeY = this->ui->doubleSpinBox_eyeY->value();
    this->ui_eyeZ = this->ui->doubleSpinBox_eyeZ->value();
    this->ui->widget_canvas->setEyePosition( this->ui_eyeX, this->ui_eyeY, this->ui_eyeZ );

    this->ui_steps = this->ui->doubleSpinBox_steps->value();
    this->ui->widget_canvas->setSteps( this->ui_steps );

    this->ui_scale = this->ui->doubleSpinBox_scale->value();
    this->ui->widget_canvas->setScale( this->ui_scale );

    this->ui_bias = this->ui->doubleSpinBox_bias->value();
    this->ui->widget_canvas->setBias( this->ui_bias );

    this->ui_currentTextureIndex = this->ui->comboBox_texture->currentIndex();

    this->ui_currentMappingIndex = ui->comboBox_mapping->currentIndex();
    this->ui->widget_canvas->setMappingIndex(this->ui_currentMappingIndex);

    this->updateForm();
    this->renderFugure();
}


Widget::~Widget()
{
    delete ui;
}

void Widget::hideAllFields()
{
}

// Установка коэффициентов для функций по-умолчанию
// для лучшего отображения в зависимости от выбранной функции
void Widget::updateForm()
{
    int currentTextureIndex = this->ui_currentMappingIndex;

    switch( currentTextureIndex )
    {
        case 0:

            this->ui->label_bias->setVisible(true);
            this->ui->doubleSpinBox_bias->setVisible(true);

        break;

        default:

            this->ui->label_bias->setVisible(false);
            this->ui->doubleSpinBox_bias->setVisible(false);

        break;
    }

    this->update();
}

void Widget::setFPS( uint fps )
{
    this->ui->label_fps_value->setText( QString::number(fps) );
}

void Widget::paintEvent(QPaintEvent *)
{
}

void Widget::renderFugure()
{
    // Передать позиции лампы и глаза в виджет OpenGL
    this->updateLamp();
    this->updateEye();
}

void Widget::updateLamp()
{
    this->ui->widget_canvas->setLightPosition( this->ui_lampX, this->ui_lampY, this->ui_lampZ );
}

void Widget::updateEye()
{
    this->ui->widget_canvas->setEyePosition( this->ui_eyeX, this->ui_eyeY, this->ui_eyeZ );
}
