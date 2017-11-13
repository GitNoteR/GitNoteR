#include "ui_markdowneditorwidget.h"

#include "markdowneditorwidget.h"
#include "mainwindow.h"

#include <QScrollBar>
#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget),
    categoryListWidget(new CategoryListWidget(this))
{
    ui->setupUi(this);
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_tag->installEventFilter(this);
    ui->markdownEditor->installEventFilter(this);

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(markdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(markdownPreviewSliderValueChanged(int)));
    connect(categoryListWidget, SIGNAL(categoryChanged(const QString &)),
            this, SLOT(onCategoryChanged(const QString &)));
    connect(categoryListWidget, SIGNAL(categoryAppend(const QString &)),
            this, SLOT(onCategoryAppend(const QString &)));
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
}

void MarkdownEditorWidget::init(NoteModel *noteModel, GroupTreeWidget *groupTreeWidget, QListWidget *listWidget)
{
    mNoteModel = noteModel;
    mGroupTreeWidget = groupTreeWidget;
    mListWidget = listWidget;

    ui->markdownEditor->setText(noteModel->getNoteText());
    ui->markdownPreview->setText(noteModel->getMarkdownHtml());
    ui->pushButton_category->setText(noteModel->getCategory().isEmpty() ? tr("所有笔记") : noteModel->getCategory());
    ui->label_createTime->setText(tr("创建时间: %1").arg(noteModel->getCreateDateString()));
    ui->label_updateTime->setText(tr("更新时间: %1").arg(noteModel->getUpdateDateString()));

    QStringList tagModelList = noteModel->getTagList();
    for (int i = 1; i < ui->horizontalLayout->count() - 1; ++i) {
        QLayoutItem *layoutItem = ui->horizontalLayout->itemAt(i);
        ui->horizontalLayout->removeItem(layoutItem);
        layoutItem->widget()->close();
    }
    for (auto &&item : tagModelList) {
        TagCellWidget *tagCellWidget = new TagCellWidget(item, this);
        ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
    }

    if (mNoteModel->getIsDelete()) {
        ui->pushButton_category->setDisabled(true);
        ui->lineEdit_tag->setDisabled(true);
        ui->label_tagIcon->setDisabled(true);
        ui->markdownEditor->setReadOnly(true);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else {
        ui->pushButton_category->setDisabled(false);
        ui->lineEdit_tag->setDisabled(false);
        ui->label_tagIcon->setDisabled(false);
        ui->markdownEditor->setReadOnly(false);
    }
}

void MarkdownEditorWidget::on_splitter_editor_splitterMoved(int pos, int)
{
    setSplitterHandleDisable(0 == pos || ui->splitter_editor->size().width() == pos);
}

void MarkdownEditorWidget::setSplitterHandleDisable(bool b)
{
    ui->splitter_editor->setStyleSheet(b ? "QSplitter#splitter_editor::handle {image: none;}" : "");
    ui->splitter_editor->handle(1)->setDisabled(b);
}

void MarkdownEditorWidget::removeTag(const QString &tagName)
{
    if (tagName.isEmpty()) {
        QLayoutItem *layoutItem = ui->horizontalLayout->itemAt(ui->horizontalLayout->count() - 2);
        layoutItem->widget()->close();
        ui->horizontalLayout->removeItem(layoutItem);
    }
    else {
        for (int i = 1; i < ui->horizontalLayout->count() - 1; ++i) {
            QLayoutItem *layoutItem = ui->horizontalLayout->itemAt(i);
            if (((TagCellWidget *) layoutItem->widget())->getTagName() == tagName) {
                ui->horizontalLayout->removeItem(layoutItem);
                layoutItem->widget()->close();
            }
        }
    }

    setTagList();
    emit groupModelListDeleted(Gitnoter::Category, tagName);
}

bool MarkdownEditorWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lineEdit_tag) {
        if (event->type() == QEvent::FocusIn) {
            ui->frame->setStyleSheet("QFrame#frame{border: 0.5px solid rgb(191, 191, 191);}");
            ui->pushButton_markdownPeview->setHidden(true);
            ui->pushButton_splitterPreview->setHidden(true);
            ui->stackedWidget->setCurrentIndex(1);
            return false;
        }
        else if (event->type() == QEvent::FocusOut) {
            ui->frame->setStyleSheet("");
            ui->pushButton_markdownPeview->setHidden(false);
            ui->pushButton_splitterPreview->setHidden(false);
            ui->stackedWidget->setCurrentIndex(0);
            return false;
        }
        // if lintEdit_tag is empty and press Backspace key, delete tag
        else if (event->type() == QEvent::KeyPress && ((QKeyEvent *) event)->key() == Qt::Key_Backspace) {
            if (ui->lineEdit_tag->text().isEmpty() && ui->horizontalLayout->count() > 2) {
                removeTag();
                return false;
            }
        }
    }
    else if (object == ui->markdownEditor) {
        if (mNoteModel && !mNoteModel->getIsDelete()) {
            if (event->type() == QEvent::FocusIn) {
                ui->stackedWidget->setCurrentIndex(0);
                return false;
            }
            else if (event->type() == QEvent::FocusOut) {
                ui->stackedWidget->setCurrentIndex(1);
                return false;
            }
        }
    }
    return QWidget::eventFilter(object, event);
}

void MarkdownEditorWidget::markdownEditorSliderValueChanged(int value, bool force) {
    // don't react if note text edit doesn't have the focus
    if (!ui->markdownEditor->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = ui->markdownEditor->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->markdownPreview->verticalScrollBar();

    float editScrollFactor = static_cast<float>(value) / editScrollBar->maximum();
    int viewPosition =
            static_cast<int>(viewScrollBar->maximum() * editScrollFactor);

    // set the scroll position in the note text view
    viewScrollBar->setSliderPosition(viewPosition);
}

void MarkdownEditorWidget::markdownPreviewSliderValueChanged(int value, bool force) {
    // don't react if note text view doesn't have the focus
    if (!ui->markdownPreview->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = ui->markdownEditor->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->markdownPreview->verticalScrollBar();

    float editScrollFactor =
            static_cast<float>(value) / viewScrollBar->maximum();

    int editPosition =
            static_cast<int>(editScrollBar->maximum() * editScrollFactor);

    // for some reason we get some int-min value here sometimes
    if (editPosition < 0) {
        return;
    }

    // set the scroll position in the note text edit
    editScrollBar->setSliderPosition(editPosition);
}

void MarkdownEditorWidget::onTagCellWidgetClicked(const QString tagName)
{
    removeTag(tagName);
}

void MarkdownEditorWidget::on_lineEdit_tag_returnPressed()
{
    if (!ui->lineEdit_tag->text().isEmpty()) {
        if (mNoteModel->getTagList().indexOf(ui->lineEdit_tag->text()) == -1) {
            TagCellWidget *tagCellWidget = new TagCellWidget(ui->lineEdit_tag->text(), this);
            ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
            setTagList();
            emit groupModelListAppend(Gitnoter::Tag, ui->lineEdit_tag->text());
        }
        ui->lineEdit_tag->clear();
    }
}

void MarkdownEditorWidget::on_markdownEditor_textChanged()
{
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    mNoteModel->saveNoteToLocal();
    emit noteModelChanged(mNoteModel);
}

void MarkdownEditorWidget::setTagList()
{
    QStringList tagList = {};
    for (int i = 1; i < ui->horizontalLayout->count() - 1; ++i) {
        tagList.append(((TagCellWidget *) ui->horizontalLayout->itemAt(i)->widget())->getTagName());
    }
    mNoteModel->setTagList(tagList);
    mNoteModel->saveNoteDataToLocal();
    emit noteModelChanged(mNoteModel);
}

void MarkdownEditorWidget::on_pushButton_category_clicked()
{
    if (categoryListWidget->isHidden()) {
        categoryListWidget->init(mGroupTreeWidget, mNoteModel->getCategory());
        categoryListWidget->open();
    }
}

void MarkdownEditorWidget::onCategoryChanged(const QString &category)
{
    ui->pushButton_category->setText(category);

    mNoteModel->setCategory(category);
    mNoteModel->saveNoteDataToLocal();
    emit noteModelChanged(mNoteModel);
}

void MarkdownEditorWidget::onCategoryAppend(const QString &category)
{
    emit categoryAppend(category);
}
