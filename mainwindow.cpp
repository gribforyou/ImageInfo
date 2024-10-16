#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImageReader>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Graphic Files Info");
    setFixedSize(size());
    setFocusPolicy(Qt::StrongFocus);

    model = new QFileSystemModel(this);
    model->setFilter(QDir::QDir::AllEntries);
    model->setFilter(QDir::Dirs);
    model->setRootPath("");

    ui->listView->setModel(model);  
    ui->listView->setRootIndex(model->index(""));
    ui->listView->setFocusPolicy(Qt::NoFocus);

    ui->pathView->setReadOnly(true);
    ui->pathView->setFocusPolicy(Qt::NoFocus);

    QStringList columns;
    columns << "Name" <<"Size" << "X Resolution" << "Y Resolution" << "Color depth" << "Compression";

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setHorizontalHeaderLabels(columns);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileDialogButton_clicked()
{
    QString dir;
    do{
        dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), "/home",
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    while(dir.isEmpty());
    model->setRootPath(dir);
    ui->listView->setRootIndex(model->index(dir));
    ui->pathView->setText(dir);
    updateTableWidget();
}

QStringList MainWindow::getImages()
{
    QDir directory(ui->pathView->text());
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QStringList imageFiles = directory.entryList(filters, QDir::Files);
    return imageFiles;
}

void MainWindow::updateTableWidget()
{
   QStringList images = getImages();
   QFileInfo fileInfo;
   QImage im;
   QString imagePath;
   QString compression;
   QString size;
   QString xResolution;
   QString yResolution;
   QString colorDepth;

   ui->tableWidget->insertRow(ui->tableWidget->rowCount());
   ui->tableWidget->setRowCount(images.size());

   for(int i = 0; i<images.size(); i++){
       imagePath = ui->pathView->text();
       if (!imagePath.endsWith('/')) {
           imagePath += '/';
       }
       imagePath += images[i];

       im = QImage(imagePath);
       fileInfo = QFileInfo(imagePath);
       size = QString::number(im.width()) + "*" + QString::number(im.height());
       xResolution = QString::number(im.physicalDpiX());
       yResolution = QString::number(im.physicalDpiY());
       colorDepth = QString::number(im.depth());
       compression = "N/A";
       if (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "jpeg") {
           compression = "Lossy compression";
       } else if (fileInfo.suffix().toLower() == "png") {
           compression = "Lossless compression";
       }

       ui->tableWidget->setItem(i, 0, new QTableWidgetItem(images[i]));
       ui->tableWidget->setItem(i, 1, new QTableWidgetItem(size));
       ui->tableWidget->setItem(i, 2, new QTableWidgetItem(xResolution));
       ui->tableWidget->setItem(i, 3, new QTableWidgetItem(yResolution));
       ui->tableWidget->setItem(i, 4, new QTableWidgetItem(colorDepth));
       ui->tableWidget->setItem(i, 5, new QTableWidgetItem(compression));
   }

}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QListView* listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);
    if(fileInfo.fileName() == "..")
    {
        QDir dir = fileInfo.dir();
        dir.cdUp();
        model->index(dir.absolutePath());
        listView->setRootIndex(model->index(dir.absolutePath()));
        ui->pathView->setText(dir.absolutePath());
    }
    else if (fileInfo.fileName() == ".")
    {
        listView->setRootIndex(model->index(""));
        ui->pathView->setText("");
    }

    else if(fileInfo.isDir())
    {
        ui->pathView->setText(model->filePath(index));
        listView->setRootIndex(index);
    }
    else if(!fileInfo.isDir())
    {
        ui->pathView->setText(model->filePath(index));
        QDir dir = fileInfo.dir();
        QString fileExt = model->fileName(index);
        QImageWriter a(model->filePath(index));
        QString resolution = "";
        QImage img (model->filePath(index));
        for(int i = fileExt.lastIndexOf('.'); i < fileExt.size(); i++)
        {
            resolution.append(fileExt[i]);
        }
    }
    updateTableWidget();
}

