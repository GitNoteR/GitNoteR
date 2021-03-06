#include "messagedialog.h"
#include "ui_messagedialog.h"

MessageDialog::MessageDialog(MainWindow *mainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog),
    mMainWindow(mainWindow)
{
    ui->setupUi(this);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

MessageDialog *MessageDialog::openMessage(MainWindow *parent, const QString &body, const QString &title, const QString &apply, const QString &close)
{
    MessageDialog *messageDialog = new MessageDialog(parent, parent);
    messageDialog->openMessage(body, title, apply, close);

    return messageDialog;
}

void MessageDialog::openMessage(const QString &body, const QString &title, const QString &apply, const QString &close)
{
    setMessageInfo(body, title, apply, close);
    open();
}

void MessageDialog::setMessageInfo(const QString &body, const QString &title, const QString &apply, const QString &close)
{
    ui->label_body->setText(body);

    if (!title.isEmpty()) {
        ui->label_title->setText(title);
    }

    if (!apply.isEmpty()) {
        ui->pushButton_apply->setText(apply);
    }

    if (!close.isEmpty()) {
        ui->pushButton_close->setText(close);
    }
}

void MessageDialog::setActiveButton(MessageDialog::ActiveButton activeButton)
{
    if (MessageDialog::Apply == activeButton) {
        ui->pushButton_apply->setFocus();
    }
    else if (MessageDialog::Close == activeButton) {
        ui->pushButton_close->setFocus();
    }
}

void MessageDialog::on_pushButton_close_clicked()
{
    emit closeClicked();
}

void MessageDialog::on_pushButton_apply_clicked()
{
    emit applyClicked();
}

void MessageDialog::showEvent(QShowEvent *showEvent)
{
    QDialog::showEvent(showEvent);

    mMainWindow->gAnalytics()->sendScreenView(objectName());
}
