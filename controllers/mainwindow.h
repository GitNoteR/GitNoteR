#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "database.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *size);

private slots:
    void on_pushButton_clicked();

signals:
    void resizeChildWindow(QSize size);

private:
    Ui::MainWindow *ui;
    Document m_content;
    QMap<QString, QPushButton *> menuPushButtons;
    CategoriesWidget *categoriesWidget;
    NoteModel *noteModel;
    Database *database;
    QString openNotesUuid;

    void textChanged();
    void updatePreview();
    void menuPushButtonClicked();
};

#endif // MAINWINDOW_H