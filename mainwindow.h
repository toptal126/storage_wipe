#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QString filterString(QString str);
private slots:
    void on_startWipe_clicked();
    void on_exit_clicked();

public:

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
