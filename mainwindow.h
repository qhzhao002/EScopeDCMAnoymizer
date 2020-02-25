#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    Ui::MainWindow *ui;
    //QLabel* m_pStatusBar;
    void initStyle();
public:

private slots:
    void on_btnStart_clicked();
    void on_btnSelectDir_clicked();
    void on_btnSelectDstDir_clicked();
    void on_checkCoverOld_stateChanged(int arg1);
    void setProcessInfo(int nCur, int nAll, int nError, int nWarnning);
    void setStatusBarMsg(QString sMsg);
    void endEdit(bool bEnd);
    //static void startEditThread(MainWindow* pMainWnd);
};

#endif // MAINWINDOW_H
