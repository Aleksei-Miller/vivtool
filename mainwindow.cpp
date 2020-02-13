#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _viv = new Viv();

    QHeaderView *header = ui->tableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(ui->tableWidget,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(customMenuRequested(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    _path = QFileDialog::getOpenFileName(this,
                                         tr("Open file"),
                                         "",

                                         tr("All supported files") +
                                         "(*.viv *.big);;" +

                                         tr("Viv file") +
                                         "(*.viv);;" +

                                         tr("Big file") +
                                         "(*.big);;" +

                                         tr("All files") +
                                         "(*.*)");

    if (_path.isEmpty())
        return;

    //Clear
    ui->tableWidget->setRowCount(0);

    //Read
    if (!_viv->read(_path))
    {
        showReadErrorMsg(_path);
        return;
    }

    //
    int count = _viv->count();

    ui->tableWidget->setRowCount(count);

    //
    for(int i = 0; i < count; i++)
    {
        Chunk *chunk = _viv->at(i);

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(chunk->name));
    }

    //
    setWindowTitle(QDir::toNativeSeparators(_path));
}

void MainWindow::on_actionSave_triggered()
{
    if (!_path.isEmpty())
    {
        if (!_viv->write(_path))
        {
            showWriteErrorMsg(_path);
            return;
        }
    }
    else
        on_actionSave_As_triggered();
}

void MainWindow::on_actionSave_As_triggered()
{
    QString path = QFileDialog::getSaveFileName(this,
                                         tr("Save file"),
                                         _path,

                                         tr("Viv file") +
                                         "(*.viv);;" +

                                         tr("Big file") +
                                         "(*.big);;" +

                                         tr("All files") +
                                         "(*.*)");

    if (path.isEmpty())
        return;

    _path = path;

    if(!_viv->write(_path))
    {
        showWriteErrorMsg(_path);
        return;
    }

    setWindowTitle(QDir::toNativeSeparators(_path));
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAdd_triggered()
{
    QStringList list_path = QFileDialog::getOpenFileNames(this,
                                                      tr("Add file"),
                                                      "",
                                                      tr("All files") +
                                                      "(*.*)");

    if (list_path.count() < 1)
        return;

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + list_path.count());

    for (int i = 0; i < list_path.count(); i++)
    {
        QString path = list_path.at(i);

        //File name
        QFileInfo file_info(list_path.at(i));
        QString file_name = file_info.fileName();

        Chunk *chunk = new Chunk;
        chunk->name = file_name;

        chunk->data = readFile(path);

        _viv->append(*chunk);

        QTableWidgetItem *item = new QTableWidgetItem(file_name);
        ui->tableWidget->setItem(_viv->count() - 1, 0, item);
    }
}

void MainWindow::on_actionExport_triggered()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.count() == 0)
        return;

    QString export_path;
    QFileInfo file_info(_path);

    if (items.count() == 1)
    {
        QTableWidgetItem *item = items.at(0);

        export_path = QFileDialog::getSaveFileName(this,
                                                   tr("Save file"),
                                                   item->text(),
                                                   tr("All files") +
                                                   "(*.*)");

        if (export_path.isEmpty())
            return;

        if (!writeFile(export_path, _viv->at(item->row())->data))
            showWriteErrorMsg(export_path);
    }
    else
    {
        QString export_dir_path = QFileDialog::getExistingDirectory(this,
                                                                    tr("Save file"),
                                                                    file_info.path());
        if (export_dir_path.isEmpty())
            return;

        for (int i = 0; i < items.count(); i++)
        {
            QTableWidgetItem *item = items.at(i);

            Chunk *chunk = _viv->at(item->row());

            //
            QFileInfo file_info_name(chunk->name);

            export_path = export_dir_path + "/" + file_info_name.fileName();

            if (!writeFile(export_path, _viv->at(item->row())->data))
            {
                showWriteErrorMsg(export_path);
                return;
            }
        }
    }
}

void MainWindow::showReadErrorMsg(const QString &path)
{
    QMessageBox::critical(this,
                          "",
                          tr("Can't read file") +
                          " " +
                          QDir::toNativeSeparators(path));
}

void MainWindow::showWriteErrorMsg(const QString &path)
{
    QMessageBox::critical(this,
                          "",
                          tr("Can't write file") +
                          " " +
                          QDir::toNativeSeparators(path));
}

QString MainWindow::getStringFileSize(uint size)
{
    double d = size / 1024.0;

    if (d > 1024.0)
    {
        d /= 1024.0;

        return QString::number(d, 'f', 2)  + " Mb";
    }

    return QString::number(d, 'f', 2)  + " Kb";
}

QByteArray MainWindow::readFile(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return 0;

    QByteArray data = file.readAll();

    file.close();

    return data;
}

bool MainWindow::writeFile(const QString &path, const QByteArray &data)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    if ((file.write(data)) == -1)
        return false;

    file.close();

    return true;
}

void MainWindow::customMenuRequested(const QPoint &pos)
{
    if (ui->tableWidget->rowCount() < 1)
        return;

    QMenu * menu = new QMenu(this);

    QAction *actionExport = new QAction(tr("Export"), this);
    connect(actionExport, SIGNAL(triggered()), this, SLOT(on_actionExport_triggered()));
    menu->addAction(actionExport);

    QAction *actionImport= new QAction(tr("Import"), this);
    connect(actionImport, SIGNAL(triggered()), this, SLOT(on_actionImport_triggered()));
    menu->addAction(actionImport);

    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionDelete_triggered()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.count() < 1)
        return;

    int ret = QMessageBox::information(this,
                                       "",
                                       tr("Delete file?"),
                                       QMessageBox::Yes | QMessageBox::No);

    if (ret != QMessageBox::Yes)
        return;

    //Delete from viv container
    for (int i = 0; i < items.count(); i++)
        _viv->remove(items.at(i)->row() - i);

    //
    for (int i = 0; i < items.count(); i++)
        ui->tableWidget->removeRow(items.at(i)->row());
}

void MainWindow::on_actionImport_triggered()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.count() < 1)
        return;

    QString import_path;
    QFileInfo file_info(_path);

    Chunk *chunk;

    if (items.count() == 1)
    {
        QTableWidgetItem *item = items.at(0);

        import_path = QFileDialog::getOpenFileName(this,
                                                   tr("Save file"),
                                                   item->text(),
                                                   tr("All files") +
                                                   "(*.*)");

        if (import_path.isEmpty())
            return;

        QByteArray data = readFile(import_path);

        if (data.isEmpty())
            return;

        chunk = new Chunk;

        chunk->name = item->text();
        chunk->data = data;

        _viv->set(item->row(), *chunk);
    }
    else
    {
        QString import_dir_path = QFileDialog::getExistingDirectory(this,
                                                                    tr("Save file"),
                                                                    file_info.path());
        if (import_dir_path.isEmpty())
            return;

        for (int i = 0; i < items.count(); i++)
        {
            QTableWidgetItem *item = items.at(i);

            //
            QFileInfo file_info(item->text());
            import_path = import_dir_path + "/" + file_info.fileName();

            QByteArray data = readFile(import_path);

            if (data.isEmpty())
                continue;

            //
            chunk = new Chunk;

            chunk->name = item->text();
            chunk->data = data;

            _viv->set(item->row(), *chunk);
        }
    }
}

void MainWindow::on_actionRename_triggered()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.count() < 1)
        return;

    QString name = QInputDialog::getText(this,
                                         tr("Rename"),
                                         "",
                                         QLineEdit::Normal,
                                         items.at(0)->text());

    if (name.isEmpty())
        return;

    QTableWidgetItem *item = items.at(0);
    item->setText(name);

    _viv->at(item->row())->name = name;
}

void MainWindow::on_actionFind_triggered()
{
    QString name = QInputDialog::getText(this,
                                         tr("Find"),
                                         tr("Find what:"));
    if (name.isEmpty())
        return;

    ui->tableWidget->clearSelection();
    QList<QTableWidgetItem*> items = ui->tableWidget->findItems(name, Qt::MatchContains);

    if (items.count() < 1)
        return;

    for (int i = 0; i < items.count(); i++)
        items.at(i)->setSelected(true);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,
                             tr("About VivTool"),
                             tr("Version") + ": " + product_version + "<br/>" +
                             tr("Site") + ": " + "<a href=" + product_site + ">" + product_site + "</a>");

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "");
}
