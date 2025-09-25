#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>
#include <QHeaderView>
#include <QElapsedTimer>
#include<QVBoxLayout>
#include<QHBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct ImageInfo {
    QString fileName;
    QString size;
    QString resolution;
    QString colorDepth;
    QString compression;
    QString format;
    QString fileSize;
    QString additionalInfo;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFolder();
    void onProcessFiles();
    void onClearTable();

private:
    Ui::MainWindow *ui;
    QTableWidget *tableWidget;
    QPushButton *btnSelectFolder;
    QPushButton *btnProcess;
    QPushButton *btnClear;
    QLineEdit *folderPathEdit;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    ImageInfo getImageInfo(const QString &filePath);
    QString getCompressionInfo(const QString &format, const QString &filePath);
    QString getAdditionalInfo(const QString &format, const QString &filePath);
    void setupUI();
};
#endif // MAINWINDOW_H
