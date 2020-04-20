
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "src/videoplayer.h"
#include <qfiledialog.h>
#include <qdebug.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) ,videoPlayer(nullptr), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::openActionClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openActionClicked(){


    if(this->videoPlayer != nullptr){

        qDebug() << "openActionClicked already videoPlayer opened";
        return;
    }


    QFileDialog dialog;
    dialog.setNameFilter(tr("Videos (*.mp4 *.avi *.xmv)"));
    dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

    if(dialog.exec()==QFileDialog::Accepted){

        if(this->videoPlayer == nullptr){

            this->videoPlayer = new VideoPlayer(dialog.selectedFiles().at(0));
            this->videoPlayer->setVideoRenderWidget(ui->videoRenderWidget);
            this->videoPlayer->play();
        }
    }
}
