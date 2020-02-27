#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "editdicom.h"
#include "pubfun.h"

#include <thread>

MainWindow* g_pMainWnd = nullptr;
void startEditThread(EditDicom* pEditor, bool bOld, QString sDstDir);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    g_pMainWnd = this;

    ui->setupUi(this);

    //test somthing
    if(0)
    {
        DcmFileFormat fm;
        fm.loadFile("E:\\0-dicomFile\\errorDicom\\46160666.dcm");
        EditDicom::saveDcmFile(fm, "E:\\0-dicomFile\\errorDicom\\46160666-decode2.dcm");
    }

    //hide toolbar
    {
        ui->mainToolBar->hide();
    }

    //icon
    {
        //this->setWindowIcon(QIcon(":/icon/escope.png"));
        this->setWindowTitle("DICOM Anonymizer");
    }

    {
        setProcessInfo(-1,-1,0,0);

        //ui->editFilter->setText("*.*");

        setFixedSize(this->width(),this->height());

        ui->comboBox->addItem("*");
        ui->comboBox->addItem("*.dcm");
        ui->comboBox->addItem("*.dic");

        ui->checkSubDir->setChecked(true);
    }

    //set skin style defined in Resources psblack.css
    {
        //initStyle();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initStyle()
{
    //load css style
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly))
    {
        QString qss = file.readAll();//QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void MainWindow::setProcessInfo(int nCur, int nAll, int nError, int nWarnning)
{
    if(nCur < 0)
    {
        ui->progressBar->setRange(0,100);
        ui->progressBar->setValue(0);
        statusBar()->showMessage("");
        ui->labelFileCount->setText("/");
        ui->labelResult->setText("");
        return;
    }

    ui->progressBar->setRange(0,nAll);
    ui->progressBar->setValue(nCur);
    ui->progressBar->update();

    char cc[200] = {0};
    sprintf(cc, "失败: %d; 警告: %d", nError, nWarnning);
    ui->labelResult->setText(QString::fromLocal8Bit(cc));

    sprintf(cc, "%d/%d", nCur, nAll);
    ui->labelFileCount->setText(cc);
}

void MainWindow::setStatusBarMsg(QString sMsg)
{
    statusBar()->showMessage(sMsg);
}

void MainWindow::endEdit(bool bEnd)
{
    ui->btnSelectDir->setEnabled(bEnd);
    ui->btnSelectDstDir->setEnabled(bEnd);
    ui->btnStart->setEnabled(bEnd);
    ui->checkCoverOld->setEnabled(bEnd);
    ui->checkSubDir->setEnabled(bEnd);
    ui->comboBox->setEnabled(bEnd);
    //ui->editDestDir->setEnabled(bEnd);
    ui->editSourceDir->setEnabled(bEnd);

    if(bEnd == true)
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        QString sTxt = QString::fromLocal8Bit("修改完成 !");
        msgBox.setText(sTxt);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::on_btnStart_clicked()
{
    QString srcDir = ui->editSourceDir->text();
    QDir dir1(srcDir);
    if(srcDir.isEmpty() || dir1.exists() == false)
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        msgBox.setText(QString::fromLocal8Bit("无效的DICOM文件夹"));//("Source folder is invalid !    ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        return;
    }

    QString dstDir = ui->editDestDir->text();

    if(ui->checkCoverOld->isChecked() == true)
    {
        dstDir = ui->editSourceDir->text();
    }
    else
    {
        if(dstDir.compare(srcDir) == 0)
        {
            QMessageBox msgBox(nullptr);
            msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
            msgBox.setText(QString::fromLocal8Bit("保存文件夹与源文件夹不能相同！"));//("Destination folder is the same as source folder !");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();

            return;
        }
    }


    if(dstDir.isEmpty())
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        msgBox.setText(QString::fromLocal8Bit("保存文件夹不能为空！"));//("Destination folder is empty !    ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        return;
    }

    QDir dir2(dstDir);
    if(dir2.exists() == false)
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        msgBox.setText(QString::fromLocal8Bit("保存文件夹不存在，自动创建吗？"));//("The destination folder does not exist, do you want to create it ?");
        msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        if(msgBox.exec() == QMessageBox::Cancel)
            return;
    }

    //go
    QString sIniName = QApplication::applicationDirPath();
    sIniName += "/DicomAnonymizer.ini";

//    QFile fe(sIniName);
//    if(fe.exists() == false)
//    {
//        QMessageBox msgBox(nullptr);
//        msgBox.setWindowTitle("Error");
//        QString sTxt = "Can not find config file : ";
//        sTxt += sIniName;
//        msgBox.setText(sTxt);
//        msgBox.setStandardButtons(QMessageBox::Ok);
//        msgBox.exec();

//        return;
//    }


    EditDicom* pEditor = new EditDicom;
    if(false == pEditor->setTagList(sIniName, "taglist"))
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle("Error");
        QString sTxt = "Invalid config file : ";
        sTxt += sIniName;
        msgBox.setText(sTxt);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        delete pEditor;
        return;
    }

    bool bFind = pEditor->setSourceDir(srcDir,ui->comboBox->currentText(), ui->checkSubDir->isChecked());
    if(bFind == false)
    {
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        QString sTxt = QString::fromLocal8Bit("没有找到文件");//"No file found in the folder!";
        msgBox.setText(sTxt);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        delete pEditor;
        return;
    }

    //qApp->processEvents();


    connect(pEditor, SIGNAL(setProcessInfo(int , int , int , int )), this, SLOT(setProcessInfo(int , int , int , int )));
    connect(pEditor, SIGNAL(setStatusBarMsg(QString)), this, SLOT(setProcessInfo(QString)));
    connect(pEditor, SIGNAL(endEdit(bool)), this, SLOT(endEdit(bool)));
    if(ui->checkCoverOld->isChecked() == true)
    {
        std::thread t1(startEditThread, pEditor, true,  "");
        //t1.join();
        t1.detach();
        //ed.editToOldDir();
    }
    else
    {
        std::thread t1(startEditThread, pEditor, false, dstDir);
        //t1.join();
        t1.detach();
        //ed.editToNewDir(dstDir);
    }

    return;
}

void MainWindow::on_btnSelectDir_clicked()
{
    setProcessInfo(-1,-1,0,0);

    QFileDialog *fileDialog = new QFileDialog(this);

    fileDialog->setWindowTitle(QStringLiteral("Select Folde"));

    fileDialog->setDirectory(".");

    fileDialog->setNameFilter(tr("File(*.*)"));

    fileDialog->setFileMode(QFileDialog::DirectoryOnly);

    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        ui->editSourceDir->setText(fileNames[0]);
    }

}

void MainWindow::on_btnSelectDstDir_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);

    fileDialog->setWindowTitle(QStringLiteral("Select Folder"));

    fileDialog->setDirectory(".");

    fileDialog->setNameFilter(tr("File(*.*)"));

    fileDialog->setFileMode(QFileDialog::DirectoryOnly);

    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        ui->editDestDir->setText(fileNames[0]);
    }
}

void MainWindow::on_checkCoverOld_stateChanged(int arg1)
{
    if(ui->checkCoverOld->isChecked() == true)
    {
        ui->editDestDir->setEnabled(false);
        ui->btnSelectDstDir->setEnabled(false);
    }
    else
    {
        ui->editDestDir->setEnabled(true);
        ui->btnSelectDstDir->setEnabled(true);
    }
}

void startEditThread(EditDicom* pEditor, bool bOld, QString sDstDir)
{
//    for(int i=0;i<3;i++)
//    {
//        //std::cout << a <<std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    }

    if(bOld)
        pEditor->editToOldDir();
    else
        pEditor->editToNewDir(sDstDir);

    delete pEditor;
}
