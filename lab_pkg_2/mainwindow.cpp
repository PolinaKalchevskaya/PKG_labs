#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QFileInfo>
#include <QThread>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    ui->setupUi(this);
    setWindowTitle("Анализатор графических файлов");
    setMinimumSize(1000, 600);

    // Создание виджетов
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Панель управления
    QHBoxLayout *controlLayout = new QHBoxLayout();

    QLabel *folderLabel = new QLabel("Папка с изображениями:", this);
    folderPathEdit = new QLineEdit(this);
    folderPathEdit->setReadOnly(true);

    btnSelectFolder = new QPushButton("Выбрать папку", this);
    btnProcess = new QPushButton("Обработать файлы", this);
    btnClear = new QPushButton("Очистить", this);

    controlLayout->addWidget(folderLabel);
    controlLayout->addWidget(folderPathEdit, 1);
    controlLayout->addWidget(btnSelectFolder);
    controlLayout->addWidget(btnProcess);
    controlLayout->addWidget(btnClear);

    // Таблица для результатов
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(8);
    tableWidget->setHorizontalHeaderLabels(QStringList()
                                           << "Имя файла" << "Размер (пиксели)" << "Разрешение (DPI)"
                                           << "Глубина цвета" << "Сжатие" << "Формат" << "Размер файла" << "Доп. информация");

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Прогресс бар и статус
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);

    statusLabel = new QLabel("Готов к работе", this);

    // Компоновка
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(tableWidget, 1);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(statusLabel);

    setCentralWidget(centralWidget);

    // Подключение сигналов
    connect(btnSelectFolder, &QPushButton::clicked, this, &MainWindow::onSelectFolder);
    connect(btnProcess, &QPushButton::clicked, this, &MainWindow::onProcessFiles);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClearTable);

    // Стилизация интерфейса
    QString styleSheet = R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border-radius: 5px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QLineEdit {
            background-color: #ecf0f1;
            border: 1px solid #bdc3c7;
            padding: 4px;
            border-radius: 4px;
        }
        QLabel {
            font-size: 14px;
            color: #2c3e50;
        }
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f2f2f2;
            gridline-color: #bdc3c7;
        }
        QHeaderView::section {
            background-color: #34495e;
            color: white;
            padding: 4px;
            border: none;
        }
        QProgressBar {
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #27ae60;
            width: 20px;
        }
    )";
    this->setStyleSheet(styleSheet);

}

void MainWindow::onSelectFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Выберите папку с изображениями",
                                                       QDir::homePath());
    if (!folder.isEmpty()) {
        folderPathEdit->setText(folder);
    }
}

void MainWindow::onProcessFiles()
{
    QString folderPath = folderPathEdit->text();
    if (folderPath.isEmpty() || !QDir(folderPath).exists()) {
        QMessageBox::warning(this, "Ошибка", "Выберите корректную папку!");
        return;
    }

    // Поддерживаемые форматы
    QStringList supportedFormats = {"*.jpg", "*.jpeg", "*.png", "*.bmp",
                                    "*.gif", "*.tif", "*.tiff", "*.pcx"};

    // Поиск файлов
    QDirIterator it(folderPath, supportedFormats, QDir::Files, QDirIterator::Subdirectories);
    QStringList files;

    while (it.hasNext() && files.size() < 100000) {
        files.append(it.next());
    }

    if (files.isEmpty()) {
        QMessageBox::information(this, "Информация", "В выбранной папке не найдено поддерживаемых изображений!");
        return;
    }

    // Настройка интерфейса
    tableWidget->setRowCount(0);
    progressBar->setVisible(true);
    progressBar->setRange(0, files.size());
    progressBar->setValue(0);
    btnProcess->setEnabled(false);
    btnSelectFolder->setEnabled(false);

    QElapsedTimer timer;
    timer.start();

    // Обработка файлов
    int processed = 0;
    for (const QString &filePath : files) {
        ImageInfo info = getImageInfo(filePath);

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        tableWidget->setItem(row, 0, new QTableWidgetItem(info.fileName));
        tableWidget->setItem(row, 1, new QTableWidgetItem(info.size));
        tableWidget->setItem(row, 2, new QTableWidgetItem(info.resolution));
        tableWidget->setItem(row, 3, new QTableWidgetItem(info.colorDepth));
        tableWidget->setItem(row, 4, new QTableWidgetItem(info.compression));
        tableWidget->setItem(row, 5, new QTableWidgetItem(info.format));
        tableWidget->setItem(row, 6, new QTableWidgetItem(info.fileSize));
        tableWidget->setItem(row, 7, new QTableWidgetItem(info.additionalInfo));

        processed++;
        progressBar->setValue(processed);

        // Обновление интерфейса каждые 10 файлов
        if (processed % 10 == 0) {
            QApplication::processEvents();
        }
    }

    // Завершение обработки
    progressBar->setVisible(false);
    btnProcess->setEnabled(true);
    btnSelectFolder->setEnabled(true);

    qint64 elapsed = timer.elapsed();
    statusLabel->setText(QString("Обработано %1 файлов за %2 мс").arg(files.size()).arg(elapsed));

    // Автоматическое подгонка размера колонок
    tableWidget->resizeColumnsToContents();
}

void MainWindow::onClearTable()
{
    tableWidget->setRowCount(0);
    statusLabel->setText("Таблица очищена");
}

ImageInfo MainWindow::getImageInfo(const QString &filePath)
{
    ImageInfo info;
    QFileInfo fileInfo(filePath);
    QImageReader reader(filePath);

    info.fileName = fileInfo.fileName();
    info.fileSize = QString("%1 KB").arg(fileInfo.size() / 1024.0, 0, 'f', 1);
    info.format = reader.format().toUpper();

    // Получение размеров изображения
    QSize size = reader.size();
    if (size.isValid()) {
        info.size = QString("%1 x %2").arg(size.width()).arg(size.height());
    } else {
        info.size = "Некорректный размер";
    }

    // Разрешение DPI - используем QImage вместо QImageReader
    QImage image;
    if (reader.read(&image)) {
        int dpiX = image.dotsPerMeterX() * 0.0254; // преобразование в DPI
        int dpiY = image.dotsPerMeterY() * 0.0254;
        info.resolution = QString("%1 x %2").arg(dpiX).arg(dpiY);

        // Глубина цвета
        info.colorDepth = QString("%1 бит").arg(image.depth());
    } else {
        info.resolution = "Неизвестно";
        info.colorDepth = "Неизвестно";
    }

    // Информация о сжатии
    info.compression = getCompressionInfo(info.format, filePath);

    // Дополнительная информация
    info.additionalInfo = getAdditionalInfo(info.format, filePath);

    return info;
}

QString MainWindow::getCompressionInfo(const QString &format, const QString &filePath)
{
    if (format == "JPG" || format == "JPEG") {
        return "JPEG сжатие (с потерями)";
    } else if (format == "PNG") {
        return "Deflate (без потерь)";
    } else if (format == "GIF") {
        return "LZW (без потерь)";
    } else if (format == "BMP") {
        return "Без сжатия или RLE";
    } else if (format == "TIF" || format == "TIFF") {
        return "LZW/Deflate/JPEG (вариативно)";
    } else if (format == "PCX") {
        return "RLE";
    }

    return "Неизвестно";
}

QString MainWindow::getAdditionalInfo(const QString &format, const QString &filePath)
{
    QString additional;

    if (format == "JPG" || format == "JPEG") {
        additional = "Цветовое пространство: YCbCr";
    } else if (format == "PNG") {
        QImage image(filePath);
        if (!image.isNull()) {
            additional = QString("Каналов: %1").arg(image.isGrayscale() ? "1" : "3-4");
        }
    } else if (format == "GIF") {
        additional = "Палитровое изображение (256 цветов)";
    } else if (format == "BMP") {
        additional = "Прямое кодирование цветов";
    }

    return additional;
}
