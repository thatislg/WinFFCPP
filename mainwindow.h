#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QProcess *ffmpegProcess;

private slots:
    void openFile();
    void saveFile();
    void cut();
    void copy();
    void paste();
    void openSettings();
    void showAbout();
    void addFile();
    void removeFile();
    void clearList();
    void playFile();
    void previewFile();
    void convertFile();
    void openOptions();
    void openOutputFolder();
    void updatePresets(int index); // Slot mới để cập nhật preset
    void onUseSourceFolderChanged(int state); // Slot mới cho checkbox
    void onFileSelectionChanged(); // Slot mới cho sự kiện chọn tệp
    void onConvertFinished(int exitCode, QProcess::ExitStatus exitStatus); // Slot xử lý khi FFmpeg hoàn tất
};
#endif // MAINWINDOW_H
