#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QColorDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFromRGB();
    void updateFromCMYK();
    void updateFromHSV();
    void updateFromManualInput();
    void openColorDialog();
    void updateColorDisplay();

private:
    // RGB компоненты
    QSlider *rSlider, *gSlider, *bSlider;
    QSpinBox *rSpin, *gSpin, *bSpin;

    // CMYK компоненты
    QSlider *cSlider, *mSlider, *ySlider, *kSlider;
    QDoubleSpinBox *cSpin, *mSpin, *ySpin, *kSpin;

    // HSV компоненты
    QSlider *hSlider, *sSlider, *vSlider;
    QSpinBox *hSpin, *sSpin, *vSpin;

    // Ручной ввод для тестирования
    QLineEdit *manualR, *manualG, *manualB;
    QLineEdit *manualC, *manualM, *manualY, *manualK;
    QLineEdit *manualH, *manualS, *manualV;
    QPushButton *applyManualButton;
    QTextEdit *logText;

    // Элементы интерфейса
    QLabel *colorDisplay;
    QPushButton *colorDialogButton;

    // Флаг для предотвращения рекурсивных обновлений
    bool updating;

    // Методы преобразования
    void rgbToCmyk(int r, int g, int b, double &c, double &m, double &y, double &k);
    void cmykToRgb(double c, double m, double y, double k, int &r, int &g, int &b);
    void rgbToHsv(int r, int g, int b, int &h, int &s, int &v);
    void hsvToRgb(int h, int s, int v, int &r, int &g, int &b);

    // Вспомогательные методы
    double clamp(double value, double min, double max);
    int clamp(int value, int min, int max);
    QLayout* createSliderSpinLayout(QSlider *slider, QWidget *spin);
    void addLogMessage(const QString &message);
};

#endif // MAINWINDOW_H
