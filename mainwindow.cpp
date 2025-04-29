#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this); // Khởi tạo giao diện từ mainwindow.ui trước

    // Tạo menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Tạo menu File
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *openAction = fileMenu->addAction(tr("&Open"));
    QAction *saveAction = fileMenu->addAction(tr("&Save"));
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));

    // Tạo menu Edit
    QMenu *editMenu = menuBar->addMenu(tr("&Edit"));
    QAction *cutAction = editMenu->addAction(tr("Cu&t"));
    QAction *copyAction = editMenu->addAction(tr("&Copy"));
    QAction *pasteAction = editMenu->addAction(tr("&Paste"));

    // Tạo menu Option
    QMenu *optionMenu = menuBar->addMenu(tr("&Option"));
    QAction *settingsAction = optionMenu->addAction(tr("&Settings"));

    // Tạo menu About
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(tr("&About"));

    // Kết nối các hành động từ menu
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cut);
    connect(copyAction, &QAction::triggered, this, &MainWindow::copy);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::paste);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    // Kết nối các hành động từ toolbar
    connect(ui->actionAdd, &QAction::triggered, this, &MainWindow::addFile);
    connect(ui->actionRemove, &QAction::triggered, this, &MainWindow::removeFile);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearList);
    connect(ui->actionPlay, &QAction::triggered, this, &MainWindow::playFile);
    connect(ui->actionPreview, &QAction::triggered, this, &MainWindow::previewFile);
    connect(ui->actionConvert, &QAction::triggered, this, &MainWindow::convertFile);
    connect(ui->actionOptions, &QAction::triggered, this, &MainWindow::openOptions);

    // Kết nối nút Open cho Output Folder
    connect(ui->pushButtonOpen, &QPushButton::clicked, this, &MainWindow::openOutputFolder);

    // Kết nối comboBoxConvertTo với slot updatePresets
    connect(ui->comboBoxConvertTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updatePresets);

    // Khởi tạo preset ban đầu
    updatePresets(0); // Gọi slot để khởi tạo preset cho định dạng mặc định (MP4)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile() {
    QMessageBox::information(this, tr("Open"), tr("Open file action triggered."));
}

void MainWindow::saveFile() {
    QMessageBox::information(this, tr("Save"), tr("Save file action triggered."));
}

void MainWindow::cut() {
    QMessageBox::information(this, tr("Cut"), tr("Cut action triggered."));
}

void MainWindow::copy() {
    QMessageBox::information(this, tr("Copy"), tr("Copy action triggered."));
}

void MainWindow::paste() {
    QMessageBox::information(this, tr("Paste"), tr("Paste action triggered."));
}

void MainWindow::openSettings() {
    QMessageBox::information(this, tr("Settings"), tr("Settings action triggered."));
}

void MainWindow::showAbout() {
    QMessageBox::about(this, tr("About"), tr("This is a sample Qt application.\nVersion 1.0"));
}

void MainWindow::addFile() {
    //QMessageBox::information(this, tr("Add"), tr("Add file action triggered."));
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Add Files"), QString(), tr("Video Files (*.mp4 *.avi *.mkv);;All Files (*)"));
    for (const QString &file : files) {
        ui->listWidgetFiles->addItem(file);
    }
}

void MainWindow::removeFile() {
    QList<QListWidgetItem*> selectedItems = ui->listWidgetFiles->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        delete ui->listWidgetFiles->takeItem(ui->listWidgetFiles->row(item));
    }
}

void MainWindow::clearList() {
    ui->listWidgetFiles->clear();
}

void MainWindow::playFile() {
    QMessageBox::information(this, tr("Play"), tr("Play file action triggered."));
}

void MainWindow::previewFile() {
    QMessageBox::information(this, tr("Preview"), tr("Preview file action triggered."));
}

void MainWindow::convertFile() {
    QMessageBox::information(this, tr("Convert"), tr("Convert file action triggered."));
}

void MainWindow::openOptions() {
    QMessageBox::information(this, tr("Options"), tr("Options action triggered."));
}

void MainWindow::openOutputFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"), QDir::homePath());
    if (!folder.isEmpty()) {
        ui->lineEditOutputFolder->setText(folder);
    }
}

void MainWindow::updatePresets(int index) {
    // Xóa danh sách preset hiện tại
    ui->comboBoxPreset->clear();

    // Lấy định dạng được chọn
    QString format = ui->comboBoxConvertTo->itemText(index);

    // Cập nhật danh sách preset dựa trên định dạng
    if (format == "MP4" || format == "MKV" || format == "MOV") {
        ui->comboBoxPreset->addItem("720p");
        ui->comboBoxPreset->addItem("1080p");
        ui->comboBoxPreset->addItem("480p");
        ui->comboBoxPreset->addItem("4K");
    } else if (format == "AVI" || format == "WMV") {
        ui->comboBoxPreset->addItem("Standard Quality");
        ui->comboBoxPreset->addItem("High Quality");
    } else if (format == "FLV") {
        ui->comboBoxPreset->addItem("Low Quality");
        ui->comboBoxPreset->addItem("Medium Quality");
    } else if (format == "MP3" || format == "AAC" || format == "OGG") {
        ui->comboBoxPreset->addItem("128kbps");
        ui->comboBoxPreset->addItem("192kbps");
        ui->comboBoxPreset->addItem("320kbps");
    }
}
