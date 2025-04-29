#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), ffmpegProcess(new QProcess(this))
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

    // Kết nối các hành động với slot
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
    connect(ui->pushButtonOpen, &QPushButton::clicked, this, &MainWindow::openOutputFolder);

    // Kết nối comboBoxConvertTo với slot updatePresets
    connect(ui->comboBoxConvertTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updatePresets);

    // Kết nối checkBoxUseSourceFolder với slot onUseSourceFolderChanged
    connect(ui->checkBoxUseSourceFolder, &QCheckBox::checkStateChanged, this, &MainWindow::onUseSourceFolderChanged);

    // Kết nối listWidgetFiles với slot onFileSelectionChanged
    connect(ui->listWidgetFiles, &QListWidget::itemSelectionChanged, this, &MainWindow::onFileSelectionChanged);

    // Kết nối QProcess để xử lý khi FFmpeg hoàn tất
    connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::onConvertFinished);

    // Khởi tạo preset ban đầu
    updatePresets(0); // Gọi slot để khởi tạo preset cho định dạng mặc định (MP4)
}

MainWindow::~MainWindow()
{
    delete ffmpegProcess;
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
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Add Files"), QString(), tr("Video Files (*.mp4 *.avi *.mkv);;All Files (*)"));
    for (const QString &file : files) {
        ui->listWidgetFiles->addItem(file);
    }
    if (ui->checkBoxUseSourceFolder->isChecked()) {
        onFileSelectionChanged();
    }
}

void MainWindow::removeFile() {
    QList<QListWidgetItem*> selectedItems = ui->listWidgetFiles->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        delete ui->listWidgetFiles->takeItem(ui->listWidgetFiles->row(item));
    }
    if (ui->checkBoxUseSourceFolder->isChecked()) {
        onFileSelectionChanged();
    }
}

void MainWindow::clearList() {
    ui->listWidgetFiles->clear();
    // Cập nhật thư mục nguồn nếu checkbox được tích
    if (ui->checkBoxUseSourceFolder->isChecked()) {
        onFileSelectionChanged();
    }
}

void MainWindow::playFile() {
    QMessageBox::information(this, tr("Play"), tr("Play file action triggered."));
}

void MainWindow::previewFile() {
    QMessageBox::information(this, tr("Preview"), tr("Preview file action triggered."));
}

void MainWindow::convertFile() {
    // Kiểm tra xem có tệp nào trong danh sách không
    if (ui->listWidgetFiles->count() == 0) {
        QMessageBox::warning(this, tr("Warning"), tr("Please add at least one file to convert."));
        return;
    }

    // Lấy thư mục đầu ra
    QString outputDir = ui->lineEditOutputFolder->text();
    if (outputDir.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please specify an output folder."));
        return;
    }

    // Kiểm tra xem thư mục đầu ra có tồn tại không, nếu không thì tạo
    QDir dir(outputDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Lấy định dạng và preset
    QString format = ui->comboBoxConvertTo->currentText().toLower();
    QString preset = ui->comboBoxPreset->currentText();

    // Xây dựng tham số FFmpeg dựa trên định dạng và preset
    QStringList presetArgs;
    if (format == "mp4" || format == "mkv" || format == "mov") {
        if (preset == "720p") {
            presetArgs << "-s" << "1280x720";
        } else if (preset == "1080p") {
            presetArgs << "-s" << "1920x1080";
        } else if (preset == "480p") {
            presetArgs << "-s" << "854x480";
        } else if (preset == "4K") {
            presetArgs << "-s" << "3840x2160";
        }
        presetArgs << "-c:v" << "libx264" << "-preset" << "medium" << "-crf" << "23";
    } else if (format == "avi" || format == "wmv") {
        if (preset == "Standard Quality") {
            presetArgs << "-q:v" << "10";
        } else if (preset == "High Quality") {
            presetArgs << "-q:v" << "2";
        }
        if (format == "avi") {
            presetArgs << "-c:v" << "mpeg4";
        } else {
            presetArgs << "-c:v" << "wmv2";
        }
    } else if (format == "flv") {
        if (preset == "Low Quality") {
            presetArgs << "-b:v" << "500k";
        } else if (preset == "Medium Quality") {
            presetArgs << "-b:v" << "1000k";
        }
        presetArgs << "-c:v" << "flv1";
    } else if (format == "mp3" || format == "aac" || format == "ogg") {
        if (preset == "128kbps") {
            presetArgs << "-b:a" << "128k";
        } else if (preset == "192kbps") {
            presetArgs << "-b:a" << "192k";
        } else if (preset == "320kbps") {
            presetArgs << "-b:a" << "320k";
        }
        if (format == "mp3") {
            presetArgs << "-c:a" << "mp3";
        } else if (format == "aac") {
            presetArgs << "-c:a" << "aac";
        } else {
            presetArgs << "-c:a" << "vorbis";
        }
    }

    // Chuyển đổi từng tệp trong danh sách
    for (int i = 0; i < ui->listWidgetFiles->count(); ++i) {
        QString inputFile = ui->listWidgetFiles->item(i)->text();
        QFileInfo fileInfo(inputFile);
        QString outputFile = outputDir + QDir::separator() + fileInfo.baseName() + "." + format;

        // Xây dựng lệnh FFmpeg
        QStringList arguments;
        arguments << "-i" << inputFile; // Tệp đầu vào
        arguments << presetArgs; // Tham số preset
        arguments << "-y"; // Ghi đè tệp đầu ra nếu đã tồn tại
        arguments << outputFile; // Tệp đầu ra

        // Chạy FFmpeg
        ffmpegProcess->start("ffmpeg", arguments);
        if (!ffmpegProcess->waitForStarted()) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to start FFmpeg. Please ensure FFmpeg is installed and accessible."));
            return;
        }

        // Chờ FFmpeg hoàn tất (đồng bộ để xử lý từng tệp một)
        ffmpegProcess->waitForFinished(-1);
    }
}

void MainWindow::openOptions() {
    QMessageBox::information(this, tr("Options"), tr("Options action triggered."));
}

void MainWindow::openOutputFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"), QString());
    if (!dir.isEmpty()) {
        ui->lineEditOutputFolder->setText(dir);
        // Bỏ tích checkbox nếu người dùng chọn thư mục thủ công
        ui->checkBoxUseSourceFolder->setChecked(false);
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

void MainWindow::onUseSourceFolderChanged(int state) {
    if (state == Qt::Checked) {
        onFileSelectionChanged(); // Cập nhật dựa trên tệp được chọn hoặc tệp đầu tiên
    } else {
        // Nếu checkbox không được tích, để trống trường output folder
        ui->lineEditOutputFolder->clear();
    }
}

void MainWindow::onFileSelectionChanged() {
    if (!ui->checkBoxUseSourceFolder->isChecked()) {
        return; // Không làm gì nếu checkbox không được tích
    }

    // Kiểm tra xem có tệp nào được chọn không
    QList<QListWidgetItem*> selectedItems = ui->listWidgetFiles->selectedItems();
    if (!selectedItems.isEmpty()) {
        // Lấy thư mục của tệp được chọn
        QString selectedFilePath = selectedItems.first()->text();
        QFileInfo fileInfo(selectedFilePath);
        QString sourceDir = fileInfo.absolutePath();
        ui->lineEditOutputFolder->setText(sourceDir);
    } else if (ui->listWidgetFiles->count() > 0) {
        // Nếu không có tệp nào được chọn, dùng tệp đầu tiên trong danh sách
        QString firstFilePath = ui->listWidgetFiles->item(0)->text();
        QFileInfo fileInfo(firstFilePath);
        QString sourceDir = fileInfo.absolutePath();
        ui->lineEditOutputFolder->setText(sourceDir);
    } else {
        // Nếu danh sách trống, để trống trường output folder
        ui->lineEditOutputFolder->clear();
    }
}

void MainWindow::onConvertFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QMessageBox::information(this, tr("Success"), tr("Conversion completed successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Conversion failed. FFmpeg exited with code: %1").arg(exitCode));
    }
}
