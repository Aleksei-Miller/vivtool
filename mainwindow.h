#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include <QMenu>

#include "format/viv.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    QByteArray readFile(const QString &path);
    bool writeFile(const QString &path, const QByteArray &data);

    void showReadErrorMsg(const QString &path);
    void showWriteErrorMsg(const QString &path);

    QString getStringFileSize(uint size);

    void customMenuRequested(const QPoint &pos);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionExit_triggered();

    void on_actionAdd_triggered();

    void on_actionExport_triggered();

    void on_actionDelete_triggered();

    void on_actionImport_triggered();

    void on_actionRename_triggered();

    void on_actionFind_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;

    QString _path;

    Viv *_viv;

    const QString product_version = "0.1";
    const QString product_site = "https://github.com/Aleksei-Miller/vivtool";
};
#endif // MAINWINDOW_H
