#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <windows.h>
#include <QStorageInfo>
#include <QSysInfo>
#include <QProcess>

#include "wipe_main.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    bool fRet = false;
    HANDLE hToken = NULL;
    if( OpenProcessToken( GetCurrentProcess( ),TOKEN_QUERY,&hToken ) ) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof( TOKEN_ELEVATION );
        if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) ) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if( hToken ) {
        CloseHandle( hToken );
    }
    qDebug() << fRet;
    if (fRet){
//        this->setWindowTitle("Administrator");
    }
    else{
        QMessageBox::warning(this, "Invalid Previllege for this task!", "Please run this program as administrator.");
//        this->setWindowTitle("Guest");
//        ui->startWipe->setEnabled(false);
    }

    ui->storagePanel->header()->resizeSection(0, 200);


    //get cpu name
    // Get Ram
    /*wmic memorychip get Capacity*/
    // Get HD
    /*wmic diskdrive get model,serialNumber,size*/
    // Get CSPRODUCT Number
    /*WMIC CSPRODUCT get IdentifyingNumber*/

    QString cpucorp = "wmic cpu get name";
    QProcess windowscpu;
    QString windowsOutput;
   /* windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();
    ui->sysInfo->append("[CPU]");
    ui->sysInfo->append(filterString(windowsOutput));

    ui->sysInfo->append("[MEMORY]");
    cpucorp = "wmic memorychip get Capacity";
    windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();
    ui->sysInfo->append(filterString(windowsOutput));
    */

//    ui->sysInfo->append("[DISK]");//,serialNumber,size
    cpucorp = "wmic diskdrive get model";
    windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();

    ui->sysInfo->append("モデル名: " + filterString(windowsOutput));
    cpucorp = "wmic diskdrive get size";
    windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();
    qlonglong size = filterString(windowsOutput).toLongLong();
    ui->sysInfo->append("メディアの容量: " + QString("%1[GB]").arg(size / 1024.0 / 1024 / 1024));
/*
    ui->sysInfo->append("[CSPRODUCT]");//,serialNumber,size
    cpucorp = "WMIC CSPRODUCT get name";
    windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();
    ui->sysInfo->append(filterString(windowsOutput));
    cpucorp = "WMIC CSPRODUCT get IdentifyingNumber";
    windowscpu.start(cpucorp);
    windowscpu.waitForFinished();
    windowsOutput = windowscpu.readAllStandardOutput().toUpper();
    ui->sysInfo->append(filterString(windowsOutput));

*/

    QTreeWidgetItem * hdd= new QTreeWidgetItem(QStringList(QString("Physical HDD")));
    QTreeWidgetItem * usbs= new QTreeWidgetItem(QStringList(QString("Removal Storages")));

    QString debugRes;
    wchar_t wcPath[10];
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {

        QTreeWidgetItem *tItem = new QTreeWidgetItem(QStringList(QString("%1 (%2)").arg(storage.name()).arg(storage.rootPath().left(2))));
        tItem->setToolTip(0, QString("file system: %3\ntotal capacity: %1GB\navailable: %2GB")
                          .arg(storage.bytesTotal()/1024/1024/1024.0)
                          .arg(storage.bytesAvailable()/1024/1024/1024.0)
                          .arg(QString(storage.fileSystemType())));
        hdd->addChild(tItem);

        QStringList tList;
        tList.append(storage.rootPath().left(2));
        tList.append(storage.fileSystemType());
        tList.append("");
        tList.append(QString("%1[GB]").arg(storage.bytesTotal()/1024/1024/1024));
        tItem = new QTreeWidgetItem(tList);
        ui->driveInfo->addTopLevelItem(tItem);


        qDebug() << storage;
        debugRes += storage.rootPath() + '\n';
        if (storage.isReadOnly())
//            qDebug() << "isReadOnly:" << storage.isReadOnly();
            debugRes += "isReadOnly:" + storage.isReadOnly() + '\n';

        qDebug() << "name:" << storage.name();
        debugRes += "name:" + storage.name() + '\n';
        qDebug() << "fileSystemType:" << storage.fileSystemType();
        //QString t = QString("%1: %2\n").arg("fileSystemType:").arg(storage.fileSystemType());
//        debugRes += t;
        qDebug() << "size:" << storage.bytesTotal()/1024/1024/1024 << "GB";
        debugRes += QString("size: %1GB\n").arg(storage.bytesTotal()/1024/1024/1024);
        qDebug() << "availableSize:" << storage.bytesAvailable()/1024/1024/1024 << "GB";
        debugRes += QString("availableSize: %1GB\n").arg(storage.bytesAvailable()/1024/1024/1024);


        storage.rootPath().toWCharArray(wcPath);
        QString temp = QString::fromWCharArray(wcPath);
        qDebug() << temp << GetDriveTypeW(wcPath);

//        ui->storagePanel->
    }
//    ui->textEdit->setText(debugRes);

    ui->storagePanel->clear();
    ui->storagePanel->addTopLevelItem(hdd);
    ui->storagePanel->addTopLevelItem(usbs);


    progressBar = ui->progressBar;

}
QString MainWindow::filterString(QString str){
    QString key = str.split('\r')[0].replace(' ',"");
    QString value;
    QStringList strArr = str.split('\r');
    for (int i = 1 ; i < strArr.length(); i++){
        if (strArr[i].trimmed() == ""){
            continue;
        }
        value += strArr[i].trimmed();
    }
    return /*key + */value;
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startWipe_clicked()
{
    int wipeOption = ui->wipeOption->currentIndex();
    int wipeMethod = ui->wipeMethod->currentIndex();
    qDebug() << wipeOption << wipeMethod;
    if (wipeOption == 0 | wipeMethod == 0){
        QMessageBox::information(this, "Information", "Select above wiping options.");
        return;
    }
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Alert!");
    msgBox.setInformativeText("This will erase entire data of this drive. \nOnce you started wiping, you can't roll back again!");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret == QMessageBox::No)
        return;


   /* char * argv[2];
    argv[0] = new char[5]; strcpy(argv[0], "exe");
    argv[1] = new char[5]; strcpy(argv[1], "-l");
    wipe_main(2, argv);
    */

    QList<QTreeWidgetItem *> list = ui->driveInfo->selectedItems();
    if (list.count() == 0){
        QMessageBox::information(this, "Information", "Select a drive.");
        return;
    }
    QString drive = QString("\\\\.\\") + list.first()->text(0).left(2);
    QByteArray ba = drive.toLocal8Bit();
    const char *c_str2 = ba.data();

    qDebug() << c_str2;
    char * argv[3];
    argv[0] = new char[5]; strcpy(argv[0], "exe");
    argv[1] = new char[10]; strcpy(argv[1], c_str2);
    argv[2] = new char[5]; strcpy(argv[2], "-y");
    wipe_main(3, argv);
//    progressBar->setValue(50);

}
