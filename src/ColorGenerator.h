#ifndef COLORGENERATOR_H
#define COLORGENERATOR_H

#include <QObject>
#include <cstdlib>
#include <ctime>
#include <cmath>

class MyColor : public QObject
{
    Q_OBJECT
public:
    explicit MyColor(QObject *parent = nullptr) : QObject(parent), r(0), g(0), b(0), a(1.0f) {} // Инициализация альфа-канала

    Q_PROPERTY(int red READ getRed WRITE setRed NOTIFY colorChanged)   // Красный компонент
    Q_PROPERTY(int green READ getGreen WRITE setGreen NOTIFY colorChanged) // Зеленый компонент
    Q_PROPERTY(int blue READ getBlue WRITE setBlue NOTIFY colorChanged) // Синий компонент
    Q_PROPERTY(float alpha READ getAlpha WRITE setAlpha NOTIFY colorChanged) // Альфа-канал (значение прозрачности, 0.0-1.0)

    int getRed() const { return r; } // Получение красного компонента
    int getGreen() const { return g; } // Получение зеленого компонента
    int getBlue() const { return b; } // Получение синего компонента
    float getAlpha() const { return a; } // Получение альфа-канала (прозрачность)

    void setRed(int value) { r = value; emit colorChanged(); } // Установка красного компонента
    void setGreen(int value) { g = value; emit colorChanged(); } // Установка зеленого компонента
    void setBlue(int value) { b = value; emit colorChanged(); } // Установка синего компонента
    void setAlpha(float value) {
        a = std::clamp(value, 0.0f, 1.0f); // Установка альфа-канала с ограничением 0.0-1.0
        emit colorChanged();
    } // Установка альфа-канала

signals:
    void colorChanged(); // Сигнал, который вызывается при изменении цвета

private:
    int r, g, b; // Красный, зелёный и синий компоненты
    float a; // Альфа-канал (прозрачность)
};

class ColorGenerator : public QObject
{
    Q_OBJECT
public:
    explicit ColorGenerator(QObject *parent = nullptr) : QObject(parent) {
        std::srand(std::time(nullptr)); // Инициализация генератора случайных чисел
    }

    Q_INVOKABLE void generateColors(MyColor *bg, MyColor *fg) {
        int r = std::rand() % 150 + 50; // Генерация цвета фона
        int g = std::rand() % 150 + 50;
        int b = std::rand() % 150 + 50;
        int o = std::round(((r * 299) + (g * 587) + (b * 114)) / 1000); // Вычисление яркости

        bg->setRed(r);
        bg->setGreen(g);
        bg->setBlue(b);

        // Установка цвета переднего плана в зависимости от яркости фона
        fg->setRed(o > 125 ? 0 : 255);
        fg->setGreen(o > 125 ? 0 : 255);
        fg->setBlue(o > 125 ? 0 : 255);

        // Генерация случайного альфа-значения в диапазоне от 0.0 до 1.0
        float alpha = static_cast<float>(std::rand()) / RAND_MAX;
        fg->setAlpha(alpha); // Установка альфа-канала переднего плана
    }
};

#endif // COLORGENERATOR_H
