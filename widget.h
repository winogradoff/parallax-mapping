#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:

    explicit Widget( QWidget *parent = 0 );
    ~Widget();
    void setFPS( uint fps );

private slots:

    // Auto-Connect syntax: void on_<object name>_<signal name>(<signal parameters>)
    void on_doubleSpinBox_lampX_valueChanged(double);
    void on_doubleSpinBox_lampY_valueChanged(double);
    void on_doubleSpinBox_lampZ_valueChanged(double);

    void on_doubleSpinBox_eyeX_valueChanged(double);
    void on_doubleSpinBox_eyeY_valueChanged(double);
    void on_doubleSpinBox_eyeZ_valueChanged(double);

    void on_doubleSpinBox_steps_valueChanged(double);
    void on_doubleSpinBox_scale_valueChanged(double);
    void on_doubleSpinBox_bias_valueChanged(double);

    void on_comboBox_mapping_currentIndexChanged(int);
    void on_comboBox_texture_currentIndexChanged(int);
    void on_pushButton_timer_clicked(bool checked);

private:

    virtual void paintEvent( QPaintEvent * );
    Ui::Widget *ui;

    void updateLamp();
    void updateEye();

    // Создание фигуры
    void renderFugure();

    // Обновление отображения формы
    void hideAllFields();
    void updateForm();

    // Переменные для значений формы
    int ui_currentMappingIndex;
    int ui_currentTextureIndex;

    int ui_steps;
    float ui_scale;
    float ui_bias;

    float ui_lampX;
    float ui_lampY;
    float ui_lampZ;

    float ui_eyeX;
    float ui_eyeY;
    float ui_eyeZ;
};

#endif // WIDGET_H
