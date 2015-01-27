#include "widget.h"
#include "ui_widget.h"

void Widget::on_doubleSpinBox_lampX_valueChanged(double value)
{
    this->ui_lampX = value;
    this->updateLamp();
}

void Widget::on_doubleSpinBox_lampY_valueChanged(double value)
{
    this->ui_lampY = value;
    this->updateLamp();
}

void Widget::on_doubleSpinBox_lampZ_valueChanged(double value)
{
    this->ui_lampZ = value;
    this->updateLamp();
}

void Widget::on_doubleSpinBox_eyeX_valueChanged(double value)
{
    this->ui_eyeX = value;
    this->updateEye();
}

void Widget::on_doubleSpinBox_eyeY_valueChanged(double value)
{
    this->ui_eyeY = value;
    this->updateEye();
}

void Widget::on_doubleSpinBox_eyeZ_valueChanged(double value)
{
    this->ui_eyeZ = value;
    this->updateEye();
}

void Widget::on_doubleSpinBox_steps_valueChanged(double value)
{
    this->ui_steps = value;
    this->ui->widget_canvas->setSteps( value );
}

void Widget::on_doubleSpinBox_scale_valueChanged(double value)
{
    this->ui_scale = value;
    this->ui->widget_canvas->setScale( value );
}

void Widget::on_doubleSpinBox_bias_valueChanged(double value)
{
    this->ui_bias = value;
    this->ui->widget_canvas->setBias( value );
}

void Widget::on_comboBox_mapping_currentIndexChanged(int value)
{
    this->ui_currentMappingIndex = value;
    this->ui->widget_canvas->setMappingIndex(value);
    this->updateForm();
}

void Widget::on_comboBox_texture_currentIndexChanged(int value)
{
    this->ui_currentTextureIndex = value;
    this->ui->widget_canvas->setTextureIndex(value);
}

void Widget::on_pushButton_timer_clicked(bool checked)
{
    if( checked )
    {
        this->ui->label_fps->setVisible(true);
        this->ui->label_fps_value->setVisible(true);
        this->ui->widget_canvas->startPerformanceTimers();
        this->ui->pushButton_timer->setText("Остановить таймеры");
    }
    else
    {
        this->ui->label_fps->setVisible(false);
        this->ui->label_fps_value->setVisible(false);
        this->ui->widget_canvas->stopPerformanceTimers();
        this->ui->pushButton_timer->setText("Запустить таймеры");
    }
}
