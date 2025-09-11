#include "mainwindow.h"
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegularExpressionValidator>
#include<QDateTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), updating(false)
{
    setWindowTitle("Color Model Converter");
    setMinimumSize(1000, 800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Левая панель - основные элементы управления
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // Отображение цвета
    colorDisplay = new QLabel(this);
    colorDisplay->setMinimumHeight(100);
    colorDisplay->setStyleSheet("border: 2px solid black;");
    leftLayout->addWidget(colorDisplay);

    // Кнопка выбора цвета
    colorDialogButton = new QPushButton("Выбрать цвет из палитры", this);
    connect(colorDialogButton, &QPushButton::clicked, this, &MainWindow::openColorDialog);
    leftLayout->addWidget(colorDialogButton);

    // Группа RGB
    QGroupBox *rgbGroup = new QGroupBox("RGB Model", this);
    QFormLayout *rgbLayout = new QFormLayout(rgbGroup);

    rSlider = new QSlider(Qt::Horizontal);
    rSlider->setRange(0, 255);
    gSlider = new QSlider(Qt::Horizontal);
    gSlider->setRange(0, 255);
    bSlider = new QSlider(Qt::Horizontal);
    bSlider->setRange(0, 255);

    rSpin = new QSpinBox();
    rSpin->setRange(0, 255);
    gSpin = new QSpinBox();
    gSpin->setRange(0, 255);
    bSpin = new QSpinBox();
    bSpin->setRange(0, 255);

    connect(rSlider, &QSlider::valueChanged, rSpin, &QSpinBox::setValue);
    connect(rSpin, &QSpinBox::valueChanged, rSlider, &QSlider::setValue);
    connect(rSlider, &QSlider::valueChanged, this, &MainWindow::updateFromRGB);

    connect(gSlider, &QSlider::valueChanged, gSpin, &QSpinBox::setValue);
    connect(gSpin, &QSpinBox::valueChanged, gSlider, &QSlider::setValue);
    connect(gSlider, &QSlider::valueChanged, this, &MainWindow::updateFromRGB);

    connect(bSlider, &QSlider::valueChanged, bSpin, &QSpinBox::setValue);
    connect(bSpin, &QSpinBox::valueChanged, bSlider, &QSlider::setValue);
    connect(bSlider, &QSlider::valueChanged, this, &MainWindow::updateFromRGB);

    rgbLayout->addRow("Red:", createSliderSpinLayout(rSlider, rSpin));
    rgbLayout->addRow("Green:", createSliderSpinLayout(gSlider, gSpin));
    rgbLayout->addRow("Blue:", createSliderSpinLayout(bSlider, bSpin));

    leftLayout->addWidget(rgbGroup);

    // Группа CMYK
    QGroupBox *cmykGroup = new QGroupBox("CMYK Model", this);
    QFormLayout *cmykLayout = new QFormLayout(cmykGroup);

    cSlider = new QSlider(Qt::Horizontal);
    cSlider->setRange(0, 1000);
    mSlider = new QSlider(Qt::Horizontal);
    mSlider->setRange(0, 1000);
    ySlider = new QSlider(Qt::Horizontal);
    ySlider->setRange(0, 1000);
    kSlider = new QSlider(Qt::Horizontal);
    kSlider->setRange(0, 1000);

    cSpin = new QDoubleSpinBox();
    cSpin->setRange(0.0, 1.0);
    cSpin->setDecimals(3);
    mSpin = new QDoubleSpinBox();
    mSpin->setRange(0.0, 1.0);
    mSpin->setDecimals(3);
    ySpin = new QDoubleSpinBox();
    ySpin->setRange(0.0, 1.0);
    ySpin->setDecimals(3);
    kSpin = new QDoubleSpinBox();
    kSpin->setRange(0.0, 1.0);
    kSpin->setDecimals(3);

    connect(cSlider, &QSlider::valueChanged, [this](int value) { cSpin->setValue(value / 1000.0); });
    connect(cSpin, &QDoubleSpinBox::valueChanged, [this](double value) { cSlider->setValue(static_cast<int>(value * 1000)); });
    connect(cSlider, &QSlider::valueChanged, this, &MainWindow::updateFromCMYK);

    connect(mSlider, &QSlider::valueChanged, [this](int value) { mSpin->setValue(value / 1000.0); });
    connect(mSpin, &QDoubleSpinBox::valueChanged, [this](double value) { mSlider->setValue(static_cast<int>(value * 1000)); });
    connect(mSlider, &QSlider::valueChanged, this, &MainWindow::updateFromCMYK);

    connect(ySlider, &QSlider::valueChanged, [this](int value) { ySpin->setValue(value / 1000.0); });
    connect(ySpin, &QDoubleSpinBox::valueChanged, [this](double value) { ySlider->setValue(static_cast<int>(value * 1000)); });
    connect(ySlider, &QSlider::valueChanged, this, &MainWindow::updateFromCMYK);

    connect(kSlider, &QSlider::valueChanged, [this](int value) { kSpin->setValue(value / 1000.0); });
    connect(kSpin, &QDoubleSpinBox::valueChanged, [this](double value) { kSlider->setValue(static_cast<int>(value * 1000)); });
    connect(kSlider, &QSlider::valueChanged, this, &MainWindow::updateFromCMYK);

    cmykLayout->addRow("Cyan:", createSliderSpinLayout(cSlider, cSpin));
    cmykLayout->addRow("Magenta:", createSliderSpinLayout(mSlider, mSpin));
    cmykLayout->addRow("Yellow:", createSliderSpinLayout(ySlider, ySpin));
    cmykLayout->addRow("Key (Black):", createSliderSpinLayout(kSlider, kSpin));

    leftLayout->addWidget(cmykGroup);

    // Группа HSV
    QGroupBox *hsvGroup = new QGroupBox("HSV Model", this);
    QFormLayout *hsvLayout = new QFormLayout(hsvGroup);

    hSlider = new QSlider(Qt::Horizontal);
    hSlider->setRange(0, 359);
    sSlider = new QSlider(Qt::Horizontal);
    sSlider->setRange(0, 100);
    vSlider = new QSlider(Qt::Horizontal);
    vSlider->setRange(0, 100);

    hSpin = new QSpinBox();
    hSpin->setRange(0, 359);
    sSpin = new QSpinBox();
    sSpin->setRange(0, 100);
    vSpin = new QSpinBox();
    vSpin->setRange(0, 100);

    connect(hSlider, &QSlider::valueChanged, hSpin, &QSpinBox::setValue);
    connect(hSpin, &QSpinBox::valueChanged, hSlider, &QSlider::setValue);
    connect(hSlider, &QSlider::valueChanged, this, &MainWindow::updateFromHSV);

    connect(sSlider, &QSlider::valueChanged, sSpin, &QSpinBox::setValue);
    connect(sSpin, &QSpinBox::valueChanged, sSlider, &QSlider::setValue);
    connect(sSlider, &QSlider::valueChanged, this, &MainWindow::updateFromHSV);

    connect(vSlider, &QSlider::valueChanged, vSpin, &QSpinBox::setValue);
    connect(vSpin, &QSpinBox::valueChanged, vSlider, &QSlider::setValue);
    connect(vSlider, &QSlider::valueChanged, this, &MainWindow::updateFromHSV);

    hsvLayout->addRow("Hue:", createSliderSpinLayout(hSlider, hSpin));
    hsvLayout->addRow("Saturation:", createSliderSpinLayout(sSlider, sSpin));
    hsvLayout->addRow("Value:", createSliderSpinLayout(vSlider, vSpin));

    leftLayout->addWidget(hsvGroup);

    // Правая панель - ручной ввод для тестирования
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QGroupBox *manualGroup = new QGroupBox("Ручной ввод (для тестирования)", this);
    QFormLayout *manualLayout = new QFormLayout(manualGroup);

    // Поля для ручного ввода
    manualR = new QLineEdit(this);
    manualG = new QLineEdit(this);
    manualB = new QLineEdit(this);
    manualC = new QLineEdit(this);
    manualM = new QLineEdit(this);
    manualY = new QLineEdit(this);
    manualK = new QLineEdit(this);
    manualH = new QLineEdit(this);
    manualS = new QLineEdit(this);
    manualV = new QLineEdit(this);

    // Установка валидаторов для числового ввода
    QRegularExpressionValidator *intValidator = new QRegularExpressionValidator(QRegularExpression("-?\\d*"), this);
    QRegularExpressionValidator *doubleValidator = new QRegularExpressionValidator(QRegularExpression("-?\\d*\\.?\\d*"), this);

    manualR->setValidator(intValidator);
    manualG->setValidator(intValidator);
    manualB->setValidator(intValidator);
    manualC->setValidator(doubleValidator);
    manualM->setValidator(doubleValidator);
    manualY->setValidator(doubleValidator);
    manualK->setValidator(doubleValidator);
    manualH->setValidator(intValidator);
    manualS->setValidator(intValidator);
    manualV->setValidator(intValidator);

    manualLayout->addRow("R:", manualR);
    manualLayout->addRow("G:", manualG);
    manualLayout->addRow("B:", manualB);
    manualLayout->addRow("C:", manualC);
    manualLayout->addRow("M:", manualM);
    manualLayout->addRow("Y:", manualY);
    manualLayout->addRow("K:", manualK);
    manualLayout->addRow("H:", manualH);
    manualLayout->addRow("S:", manualS);
    manualLayout->addRow("V:", manualV);

    applyManualButton = new QPushButton("Применить ручной ввод", this);
    connect(applyManualButton, &QPushButton::clicked, this, &MainWindow::updateFromManualInput);
    manualLayout->addRow(applyManualButton);

    rightLayout->addWidget(manualGroup);

    // Лог сообщений
    logText = new QTextEdit(this);
    logText->setReadOnly(true);
    logText->setMaximumHeight(150);
    logText->setPlaceholderText("Здесь будут отображаться предупреждения...");
    rightLayout->addWidget(new QLabel("Лог предупреждений:"));
    rightLayout->addWidget(logText);

    mainLayout->addWidget(leftPanel, 2);
    mainLayout->addWidget(rightPanel, 1);

    // Установка начального значения
    rSlider->setValue(255);
    gSlider->setValue(0);
    bSlider->setValue(0);

    addLogMessage("Приложение запущено. Вводите значения в правой панели для тестирования.");
}



void MainWindow::updateFromManualInput()
{
    bool hasErrors = false;
    QStringList warnings;

    // Определяем, какие поля были изменены
    bool rgbModified = !manualR->text().isEmpty() || !manualG->text().isEmpty() || !manualB->text().isEmpty();
    bool cmykModified = !manualC->text().isEmpty() || !manualM->text().isEmpty() || !manualY->text().isEmpty() || !manualK->text().isEmpty();
    bool hsvModified = !manualH->text().isEmpty() || !manualS->text().isEmpty() || !manualV->text().isEmpty();

    updating = true;

    if (rgbModified) {
        // Обработка RGB
        int r = manualR->text().isEmpty() ? rSlider->value() : manualR->text().toInt();
        int g = manualG->text().isEmpty() ? gSlider->value() : manualG->text().toInt();
        int b = manualB->text().isEmpty() ? bSlider->value() : manualB->text().toInt();

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            hasErrors = true;
            warnings << "RGB значения обрезаны";
            r = clamp(r, 0, 255);
            g = clamp(g, 0, 255);
            b = clamp(b, 0, 255);
            manualR->setText(QString::number(r));
            manualG->setText(QString::number(g));
            manualB->setText(QString::number(b));
        }

        rSlider->setValue(r);
        gSlider->setValue(g);
        bSlider->setValue(b);

    } else if (cmykModified) {
        // Обработка CMYK - преобразуем в RGB
        double c = manualC->text().isEmpty() ? cSpin->value() : manualC->text().toDouble();
        double m = manualM->text().isEmpty() ? mSpin->value() : manualM->text().toDouble();
        double y = manualY->text().isEmpty() ? ySpin->value() : manualY->text().toDouble();
        double k = manualK->text().isEmpty() ? kSpin->value() : manualK->text().toDouble();

        if (c < 0 || c > 1 || m < 0 || m > 1 || y < 0 || y > 1 || k < 0 || k > 1) {
            hasErrors = true;
            warnings << "CMYK значения обрезаны";
            c = clamp(c, 0.0, 1.0);
            m = clamp(m, 0.0, 1.0);
            y = clamp(y, 0.0, 1.0);
            k = clamp(k, 0.0, 1.0);
            manualC->setText(QString::number(c, 'f', 3));
            manualM->setText(QString::number(m, 'f', 3));
            manualY->setText(QString::number(y, 'f', 3));
            manualK->setText(QString::number(k, 'f', 3));
        }

        // Конвертируем CMYK в RGB
        int r, g, b;
        cmykToRgb(c, m, y, k, r, g, b);
        rSlider->setValue(r);
        gSlider->setValue(g);
        bSlider->setValue(b);

    } else if (hsvModified) {
        // Обработка HSV
        int h = manualH->text().isEmpty() ? hSlider->value() : manualH->text().toInt();
        int s = manualS->text().isEmpty() ? sSlider->value() : manualS->text().toInt();
        int v = manualV->text().isEmpty() ? vSlider->value() : manualV->text().toInt();

        if (h < 0 || h > 359 || s < 0 || s > 100 || v < 0 || v > 100) {
            hasErrors = true;
            warnings << "HSV значения обрезаны";
            h = clamp(h, 0, 359);
            s = clamp(s, 0, 100);
            v = clamp(v, 0, 100);
            manualH->setText(QString::number(h));
            manualS->setText(QString::number(s));
            manualV->setText(QString::number(v));
        }

        // Конвертируем HSV в RGB
        int r, g, b;
        hsvToRgb(h, s, v, r, g, b);
        rSlider->setValue(r);
        gSlider->setValue(g);
        bSlider->setValue(b);
    }

    updating = false;

    // Записать предупреждения в лог
    if (hasErrors) {
        for (const QString &warning : warnings) {
            addLogMessage("Ошибка: " + warning);
        }
        QMessageBox::warning(this, "Предупреждение", "Некоторые значения были обрезаны.");
    }

    // Обновляем все модели из RGB
    updateFromRGB();

    // Очищаем все поля
    manualR->clear();
    manualG->clear();
    manualB->clear();
    manualC->clear();
    manualM->clear();
    manualY->clear();
    manualK->clear();
    manualH->clear();
    manualS->clear();
    manualV->clear();
}
void MainWindow::addLogMessage(const QString &message)
{
    logText->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + message);
}




// Реализация метода createSliderSpinLayout (исправленный тип возвращаемого значения)
QLayout* MainWindow::createSliderSpinLayout(QSlider *slider, QWidget *spin)
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(slider);
    layout->addWidget(spin);
    return layout;
}
void MainWindow::rgbToCmyk(int r, int g, int b, double &c, double &m, double &y, double &k)
{
    double rn = r / 255.0;
    double gn = g / 255.0;
    double bn = b / 255.0;

    k = 1.0 - std::max(std::max(rn, gn), bn);

    if (k == 1.0) {
        c = m = y = 0.0;
    } else {
        c = (1.0 - rn - k) / (1.0 - k);
        m = (1.0 - gn - k) / (1.0 - k);
        y = (1.0 - bn - k) / (1.0 - k);
    }

    c = clamp(c, 0.0, 1.0);
    m = clamp(m, 0.0, 1.0);
    y = clamp(y, 0.0, 1.0);
    k = clamp(k, 0.0, 1.0);
}

void MainWindow::cmykToRgb(double c, double m, double y, double k, int &r, int &g, int &b)
{
    c = clamp(c, 0.0, 1.0);
    m = clamp(m, 0.0, 1.0);
    y = clamp(y, 0.0, 1.0);
    k = clamp(k, 0.0, 1.0);

    r = static_cast<int>(255 * (1.0 - c) * (1.0 - k));
    g = static_cast<int>(255 * (1.0 - m) * (1.0 - k));
    b = static_cast<int>(255 * (1.0 - y) * (1.0 - k));

    r = clamp(r, 0, 255);
    g = clamp(g, 0, 255);
    b = clamp(b, 0, 255);
}

void MainWindow::rgbToHsv(int r, int g, int b, int &h, int &s, int &v)
{
    double rn = r / 255.0;
    double gn = g / 255.0;
    double bn = b / 255.0;

    double cmax = std::max(std::max(rn, gn), bn);
    double cmin = std::min(std::min(rn, gn), bn);
    double delta = cmax - cmin;

    // Value
    v = static_cast<int>(cmax * 100);

    // Saturation
    if (cmax == 0) {
        s = 0;
    } else {
        s = static_cast<int>((delta / cmax) * 100);
    }

    // Hue
    if (delta == 0) {
        h = 0;
    } else if (cmax == rn) {
        h = static_cast<int>(60 * fmod(((gn - bn) / delta), 6));
    } else if (cmax == gn) {
        h = static_cast<int>(60 * (((bn - rn) / delta) + 2));
    } else {
        h = static_cast<int>(60 * (((rn - gn) / delta) + 4));
    }

    if (h < 0) h += 360;
    h = h % 360;
}

void MainWindow::hsvToRgb(int h, int s, int v, int &r, int &g, int &b)
{
    h = clamp(h, 0, 359);
    s = clamp(s, 0, 100);
    v = clamp(v, 0, 100);

    double hh = h / 60.0;
    double ss = s / 100.0;
    double vv = v / 100.0;

    double c = vv * ss;
    double x = c * (1 - fabs(fmod(hh, 2) - 1));
    double m = vv - c;

    double rn, gn, bn;

    if (hh >= 0 && hh < 1) {
        rn = c; gn = x; bn = 0;
    } else if (hh >= 1 && hh < 2) {
        rn = x; gn = c; bn = 0;
    } else if (hh >= 2 && hh < 3) {
        rn = 0; gn = c; bn = x;
    } else if (hh >= 3 && hh < 4) {
        rn = 0; gn = x; bn = c;
    } else if (hh >= 4 && hh < 5) {
        rn = x; gn = 0; bn = c;
    } else {
        rn = c; gn = 0; bn = x;
    }

    r = static_cast<int>((rn + m) * 255);
    g = static_cast<int>((gn + m) * 255);
    b = static_cast<int>((bn + m) * 255);

    r = clamp(r, 0, 255);
    g = clamp(g, 0, 255);
    b = clamp(b, 0, 255);
}

double MainWindow::clamp(double value, double min, double max)
{
    return value < min ? min : (value > max ? max : value);
}

int MainWindow::clamp(int value, int min, int max)
{
    return value < min ? min : (value > max ? max : value);
}

void MainWindow::updateFromRGB()
{
    if (updating) return;
    updating = true;

    int r = rSlider->value();
    int g = gSlider->value();
    int b = bSlider->value();

    // Обновление CMYK
    double c, m, y, k;
    rgbToCmyk(r, g, b, c, m, y, k);
    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    // Обновление HSV
    int h, s, v;
    rgbToHsv(r, g, b, h, s, v);
    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    updateColorDisplay();
    updating = false;
}

void MainWindow::updateFromCMYK()
{
    if (updating) return;
    updating = true;

    double c = cSpin->value();
    double m = mSpin->value();
    double y = ySpin->value();
    double k = kSpin->value();

    // Обновление RGB
    int r, g, b;
    cmykToRgb(c, m, y, k, r, g, b);
    rSpin->setValue(r);
    gSpin->setValue(g);
    bSpin->setValue(b);

    // Обновление HSV
    int h, s, v;
    rgbToHsv(r, g, b, h, s, v);
    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    updateColorDisplay();
    updating = false;
}

void MainWindow::updateFromHSV()
{
    if (updating) return;
    updating = true;

    int h = hSpin->value();
    int s = sSpin->value();
    int v = vSpin->value();

    // Обновление RGB
    int r, g, b;
    hsvToRgb(h, s, v, r, g, b);
    rSpin->setValue(r);
    gSpin->setValue(g);
    bSpin->setValue(b);

    // Обновление CMYK
    double c, m, y, k;
    rgbToCmyk(r, g, b, c, m, y, k);
    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    updateColorDisplay();
    updating = false;
}

void MainWindow::openColorDialog()
{
    QColor color = QColorDialog::getColor(QColor(rSlider->value(), gSlider->value(), bSlider->value()), this);
    if (color.isValid()) {
        updating = true;
        rSpin->setValue(color.red());
        gSpin->setValue(color.green());
        bSpin->setValue(color.blue());
        updating = false;
        updateFromRGB();
    }
}

void MainWindow::updateColorDisplay()
{
    int r = rSlider->value();
    int g = gSlider->value();
    int b = bSlider->value();

    QString style = QString("background-color: rgb(%1, %2, %3); border: 2px solid black;")
                        .arg(r).arg(g).arg(b);
    colorDisplay->setStyleSheet(style);

    // Проверка на выход за границы и предупреждение
    bool outOfRange = false;
    QString warning;

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        outOfRange = true;
        warning = "RGB значения обрезаны до допустимого диапазона";
    }

    double c = cSpin->value(), m = mSpin->value(), y = ySpin->value(), k = kSpin->value();
    if (c < 0 || c > 1 || m < 0 || m > 1 || y < 0 || y > 1 || k < 0 || k > 1) {
        outOfRange = true;
        warning = "CMYK значения обрезаны до допустимого диапазона";
    }

    if (outOfRange && !warning.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", warning);
    }
}
MainWindow::~MainWindow() {}
