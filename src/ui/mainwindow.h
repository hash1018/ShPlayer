#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class VideoPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    VideoPlayer *videoPlayer;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void openActionClicked();
};
#endif // MAINWINDOW_H
