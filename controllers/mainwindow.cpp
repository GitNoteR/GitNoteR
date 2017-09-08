#include "mainwindow.h"
#include "previewpage.h"
#include "tools.h"
#include "globals.h"
#include "categorieslistcell.h"

#include "ui_mainwindow.h"

#include <QWebChannel>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 不接受拖放 ui编辑器编辑无用必须代码, 可能是bug
    ui->listWidget_categories->setAcceptDrops(false);

//    QFile defaultTextFile(":/marked/default.md");
//    defaultTextFile.open(QIODevice::ReadOnly);
//    QString defaultText = QString(defaultTextFile.readAll());
//
//    noteModel = new NoteModel(defaultText);
//
//
//    return;

//    qDebug() << Qt::DescendingOrder;

    this->initNotesToDatabases();

    g_configTableModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde5");
    g_configTableModel->setSidebarSortKey(1);
    g_configTableModel->setSidebarSortValue("DESC");
    g_configTableModel->setCategoriesId(1);

    this->setDefaultNote();
    this->setSidebarTable();

    // 初始化菜单快捷键
    ui->action_newNote->setShortcut(Qt::CTRL | Qt::Key_N);
    ui->action_newCategories->setShortcut(Qt::META | Qt::CTRL | Qt::Key_C);
    ui->action_newTags->setShortcut(Qt::META | Qt::CTRL | Qt::Key_T);

    ui->action_saveNote->setShortcut(Qt::CTRL | Qt::Key_S);
    ui->action_quitNote->setShortcut(Qt::CTRL | Qt::Key_W);

    ui->action_synch->setShortcut(Qt::META | Qt::CTRL | Qt::Key_S);

    ui->action_repeal->setShortcut(Qt::CTRL | Qt::Key_Z);
    ui->action_reform->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_Z);

    ui->action_cut->setShortcut(Qt::CTRL | Qt::Key_X);
    ui->action_copy->setShortcut(Qt::CTRL | Qt::Key_C);
    ui->action_paste->setShortcut(Qt::CTRL | Qt::Key_V);
    ui->action_selectAll->setShortcut(Qt::CTRL | Qt::Key_A);

    ui->action_notes->setShortcut(Qt::ALT | Qt::CTRL | Qt::Key_F);
    ui->action_noteText->setShortcut(Qt::CTRL | Qt::Key_F);

//    ui->action_toUppercase->setShortcut(Qt::CTRL | Qt::Key_N);
//    ui->action_toLowercase->setShortcut(Qt::CTRL | Qt::Key_N);
//    ui->action_toUppercaseAtFirst->setShortcut(Qt::CTRL | Qt::Key_N);

    connect(ui->tableWidget_list->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MainWindow::on_headerView_sortIndicatorChanged);

    // 左侧菜单栏
    this->menuPushButtons.insert("pushButton_folder", ui->pushButton_folder);
    this->menuPushButtons.insert("pushButton_notes", ui->pushButton_notes);
    this->menuPushButtons.insert("pushButton_search", ui->pushButton_search);
    this->menuPushButtons.insert("pushButton_sync", ui->pushButton_sync);
    this->menuPushButtons.insert("pushButton_tags", ui->pushButton_tags);
    this->menuPushButtons.insert("pushButton_Trash", ui->pushButton_Trash);

    for (auto itr = menuPushButtons.begin(); itr != menuPushButtons.end(); ++itr) {
        connect(itr.value(), &QPushButton::clicked, this, &MainWindow::menuPushButtonClicked);
    }

    // 屏蔽选中时的边框颜色
    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_categories->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // 初始化编辑器
    ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->webEngineView_preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->webEngineView_preview->setUrl(QUrl("qrc:/marked/index.html"));

    this->setMainWindowData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    g_noteModel->noteTableModel->setTitle(ui->lineEdit_title->displayText().isEmpty()
                                        ? ui->lineEdit_title->placeholderText()
                                        : ui->lineEdit_title->displayText());
    g_noteModel->noteTableModel->setBody(ui->plainTextEdit_editor->toPlainText());
    g_noteModel->noteTableModel->setUpdateDate(0);
    this->updatePreview();
}

void MainWindow::updatePreview()
{
    if (g_noteModel->noteTableModel->getTitle().isEmpty()) {
        m_content.setText(g_noteModel->noteTableModel->getBody());
    }
    else {
        m_content.setText("# " + g_noteModel->noteTableModel->getTitle() + "\n\n" + g_noteModel->noteTableModel->getBody());
    }
}

void MainWindow::menuPushButtonClicked()
{
    QString objectName = this->sender()->objectName();

    // TODO: 选择这里需要优化, 重复点击同一个按钮会出现闪烁, 之后改成, 点击替换按钮图片, 避免闪烁
    QMap<QString, QPushButton *>::Iterator itr;
    for (itr = this->menuPushButtons.begin(); itr != this->menuPushButtons.end(); ++itr) {
        itr.value()->setChecked(itr.key() == objectName);
    }
}

void MainWindow::on_pushButton_categories_clicked()
{
    categoriesWidget = new CategoriesWidget(this);
    categoriesWidget->show();

    connect(this, &MainWindow::resizeChildWindow, categoriesWidget, &CategoriesWidget::resizeWindow);
    connect(categoriesWidget, SIGNAL(changeCategories()), this, SLOT(onChangeCategories()));
}

void MainWindow::resizeEvent(QResizeEvent *size)
{
    emit resizeChildWindow(size->size());
}

void MainWindow::initNotesToDatabases()
{
    QString notesPath = QDir(g_repoPath).filePath(g_noteFolderName);
    QFileInfoList fileInfoList = Tools::getFilesPath(notesPath);
    for (auto &&fileInfo : fileInfoList) {
        NoteModel *noteModel = new NoteModel(Tools::readerFile(fileInfo.absoluteFilePath()), fileInfo.absoluteFilePath());
        g_database->addNoteText(noteModel);
    }
}

void MainWindow::setDefaultNote()
{
    if (g_configTableModel->getOpenNotesUuid().isEmpty()) {
        if (m_sidebarNoteList->length() != 0) {
            g_configTableModel->setOpenNotesUuid(m_sidebarNoteList->at(0)->getUuid());
            this->setDefaultNote();
            return;
        }
        g_noteModel = new NoteModel(Tools::readerFile(":/marked/default.md"));
    }
    else {
        g_noteModel = g_database->getNoteByUuid(g_configTableModel->getOpenNotesUuid());
    }
}

void MainWindow::setSidebarTable()
{
    ui->tableWidget_list->clearContents();
    m_sidebarNoteList = g_database->getSidebarNotes();
    auto *sidebarNoteList = new QList<NoteTableModel *>();
    if (g_configTableModel->getCategoriesId() == 0) {
        sidebarNoteList = m_sidebarNoteList;
    }
    else {
        auto categoriesList = g_database->selectNJCTableByCategoriesId(g_configTableModel->getCategoriesId());
        for (auto &&item : categoriesList) {
            for (auto &&list : *m_sidebarNoteList) {
                if (item->getNotesUuid() == list->getUuid()) {
                    sidebarNoteList->append(list);
                }
            }
        }
    }

    ui->tableWidget_list->setRowCount(sidebarNoteList->length());
    for (int i = 0; i < sidebarNoteList->length(); ++i) {
        auto *tableWidgetItem0 = new QTableWidgetItem(sidebarNoteList->at(i)->getTitle());
        tableWidgetItem0->setData(Qt::UserRole, i);
        auto *tableWidgetItem1 = new QTableWidgetItem(
                    Tools::timestampToDateTime(sidebarNoteList->at(i)->getUpdateDate()));
        tableWidgetItem1->setData(Qt::UserRole, i);
        ui->tableWidget_list->setItem(i, 0, tableWidgetItem0);
        ui->tableWidget_list->setItem(i, 1, tableWidgetItem1);
        if (sidebarNoteList->at(i)->getUuid() == g_configTableModel->getOpenNotesUuid()) {
            ui->tableWidget_list->selectRow(i);
        }
    }

    int sidebarSortKey = g_configTableModel->getSidebarSortKey();
    Qt::SortOrder sidebarSortValue = g_configTableModel->getSidebarSortValue() == "DESC" ? Qt::DescendingOrder
                                                                                      : Qt::AscendingOrder;
    ui->tableWidget_list->horizontalHeader()->sortIndicatorChanged(sidebarSortKey, sidebarSortValue);
    ui->tableWidget_list->horizontalHeader()->setSortIndicator(sidebarSortKey, sidebarSortValue);
}

void MainWindow::on_tableWidget_list_itemClicked(QTableWidgetItem *item)
{
    qDebug() << "on_tableWidget_list_itemClicked: " << item->data(Qt::UserRole);
    QString uuid = m_sidebarNoteList->at(item->data(Qt::UserRole).toInt())->getUuid();
    if (g_configTableModel->getOpenNotesUuid() == uuid) {
        return;
    }
    g_configTableModel->setOpenNotesUuid(uuid);
    this->setDefaultNote();
    this->setMainWindowData();
}

void MainWindow::on_tableWidget_list_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::setEditText()
{
    disconnect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    disconnect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);

    ui->lineEdit_title->setText(g_noteModel->noteTableModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(g_noteModel->noteTableModel->getBody());

    this->setModified(true);
    this->updatePreview();

    // 监听编辑器中文本是否有更改
    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);
}

void MainWindow::on_action_saveNote_triggered()
{
    if (!this->isModified()) {
        return;
    }

    Tools::writerFile(g_noteModel->noteTableModel->getFilePath(), g_noteModel->getNote());
    g_database->addNoteText(g_noteModel);
    this->setSidebarTable();
}

void MainWindow::on_action_newNote_triggered()
{
    this->on_action_saveNote_triggered();

    g_noteModel->clear();
    this->setMainWindowData();
}

bool MainWindow::isModified()
{
    return ui->lineEdit_title->isModified() || ui->plainTextEdit_editor->document()->isModified();
}

void MainWindow::setModified(bool m)
{
    ui->lineEdit_title->setModified(m);
    ui->plainTextEdit_editor->document()->setModified(m);
}

void MainWindow::on_pushButton_deleteNote_clicked()
{
    this->on_action_deleteNote_triggered();
}

void MainWindow::on_action_deleteNote_triggered()
{
    g_database->deleteNoteByUuid(g_noteModel->noteTableModel->getUuid());
    QFile::remove(g_noteModel->noteTableModel->getFilePath());
    g_configTableModel->setOpenNotesUuid("");
    g_noteModel->clear();

    this->setSidebarTable();
    this->setDefaultNote();
    this->setMainWindowData();
}

void MainWindow::on_headerView_sortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    g_configTableModel->setSidebarSortKey(logicalIndex);
    g_configTableModel->setSidebarSortValue(order == Qt::DescendingOrder ? "DESC" : "ASC");

    qDebug() << "on_headerView_sortIndicatorChanged";
}

void MainWindow::setTagsData()
{
    QString tagsString;
    for (auto &&item : *g_noteModel->tagTableList) {
        tagsString += item->getName() + g_tagSplit;
    }
    tagsString.chop(g_tagSplit.length());
    ui->pushButton_changeTags->setText(tagsString);
}

void MainWindow::setMainWindowData()
{
    this->setEditText();
    this->setModified(false);

    this->setTagsData();
    ui->pushButton_categories->setText(g_noteModel->categoriesTableModel->getName());
}

void MainWindow::onChangeCategories()
{
    ui->pushButton_categories->setText(g_noteModel->categoriesTableModel->getName());
    this->setModified(true);
    this->on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_folder_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    if (ui->listWidget_categories->count() > 0) {
        return;
    }

    setCategoriesList();
}

void MainWindow::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_categories->itemWidget(item)->findChild<QWidget *>("widget_3");

    if (item->data(Qt::UserRole).isNull()) {
        resetListWidgetCategoriesBorder();
        item->setData(Qt::UserRole, 1);
        Tools::changeWidgetBorder(widget, "#7EBFF5", 2);
    }
}

void MainWindow::on_pushButton_notes_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

QMenu* MainWindow::createListWidgetCategoriesMenu()
{
    QMenu* popMenu = new QMenu(this);

    QAction *action_createCategories;
    QAction *action_renameCategories;
    QAction *action_removeCategories;
    QAction *action_nameSort;
    QAction *action_countSort;
    QAction *action_timeSort;

    action_createCategories = new QAction("新建文件夹", this);
    action_renameCategories = new QAction("重命名笔记本", this);
    action_removeCategories = new QAction("删除笔记本", this);
    action_nameSort = new QAction("按笔记本名称排序", this);
    action_countSort = new QAction("按笔记数量排序", this);
    action_timeSort = new QAction("按笔记更新时间排序", this);

    action_createCategories->setObjectName(QStringLiteral("action_createCategories"));
    action_renameCategories->setObjectName(QStringLiteral("action_renameCategories"));
    action_removeCategories->setObjectName(QStringLiteral("action_removeCategories"));
    action_nameSort->setObjectName(QStringLiteral("action_nameSort"));
    action_countSort->setObjectName(QStringLiteral("action_countSort"));
    action_timeSort->setObjectName(QStringLiteral("action_timeSort"));

    popMenu->addAction(action_createCategories);
    popMenu->addAction(action_renameCategories);
    popMenu->addAction(action_removeCategories);
    popMenu->addSeparator();
    popMenu->addAction(action_nameSort);
    popMenu->addAction(action_countSort);
    popMenu->addAction(action_timeSort);

    connect(action_createCategories, &QAction::triggered, this, &MainWindow::on_pushButton_addCategories_clicked);
    connect(action_renameCategories, &QAction::triggered, this, &MainWindow::onActionRenameCategoriesTriggered);
    connect(action_removeCategories, &QAction::triggered, this, &MainWindow::on_pushButton_removeCategories_clicked);
    connect(action_nameSort, &QAction::triggered, this, &MainWindow::onActionNameSortTriggered);
    connect(action_countSort, &QAction::triggered, this, &MainWindow::onActionCountSortTriggered);
    connect(action_timeSort, &QAction::triggered, this, &MainWindow::onActionTimeSortTriggered);

    return popMenu;
}

void MainWindow::onActionNameSortTriggered()
{
    qDebug() << "on_action_nameSort_triggered";
}

void MainWindow::onActionCountSortTriggered()
{

}

void MainWindow::onActionTimeSortTriggered()
{
    qDebug() << "onActionTimeSortTriggered";
}

void MainWindow::onLineEditNameEditingFinished()
{
    int index = ui->listWidget_categories->selectionModel()->selectedIndexes()[0].row();
    QWidget *widget = ui->listWidget_categories->itemWidget(ui->listWidget_categories->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget_3")->findChild<QLineEdit *>("lineEdit_name");

    if(lineEdit_name->displayText().isEmpty()) {
        lineEdit_name->setText(m_categoriesList[index]->getName());
    }
    else {
        if (g_database->updateCategoriesTableByName(lineEdit_name->displayText(), m_categoriesList[index]->getName())) {
            auto categoriesList = g_database->selectNJCTableByCategoriesId(m_categoriesList[index]->getCategoriesId());
            for (auto &&item : categoriesList) {
                auto *note = g_database->getNoteByUuid(item->getNotesUuid());
                Tools::writerFile(note->noteTableModel->getFilePath(), note->getNote());
            }

            m_categoriesList[index]->setName(lineEdit_name->displayText());
        }
        else {
            lineEdit_name->setText(m_categoriesList[index]->getName());
        }
    }
    lineEdit_name->setEnabled(false);
}

void MainWindow::on_pushButton_addCategories_clicked()
{
    for (int i = 0; i < 100; ++i) {
        if (g_database->insertCategoriesTable(tr("新建笔记本%1").arg(i == 0 ? "" : QString::number(i))) != 0) {
            ui->listWidget_categories->clear();
            setCategoriesList();
            return;
        }
    }
}

void MainWindow::onActionRenameCategoriesTriggered()
{
    QWidget *widget = ui->listWidget_categories->itemWidget(ui->listWidget_categories->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget_3")->findChild<QLineEdit *>("lineEdit_name");
    lineEdit_name->setEnabled(true);
    lineEdit_name->setFocus();
    lineEdit_name->selectAll();
    connect(lineEdit_name, SIGNAL(editingFinished()), this, SLOT(onLineEditNameEditingFinished()));
}

void MainWindow::on_pushButton_removeCategories_clicked()
{
    auto selectedIndexes = ui->listWidget_categories->selectionModel()->selectedIndexes();
    if (selectedIndexes.length() != 0) {
        int index = selectedIndexes[0].row();
        if (m_categoriesList[index]->getCount() == 0) {
            g_database->deleteCategoriesTableByName(m_categoriesList[index]->getName());
            setCategoriesList();
        }
        else {
            QMessageBox::about(this, tr("消息提醒"), tr("该笔记本存在笔记, 请将笔记移出笔记本"));
        }
    }
}

void MainWindow::on_listWidget_categories_customContextMenuRequested(const QPoint &pos)
{
    if(ui->listWidget_categories->itemAt(pos) != NULL) {
        on_listWidget_categories_itemClicked(ui->listWidget_categories->itemAt(pos));
        createListWidgetCategoriesMenu()->exec(ui->listWidget_categories->mapToGlobal(pos));
    }
}

void MainWindow::resetListWidgetCategoriesBorder()
{
    for (int i = 0; i < ui->listWidget_categories->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
        QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget_3");
        Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
        listWidgetItem->setData(Qt::UserRole, QVariant());
    }
}

void MainWindow::setCategoriesList(bool reread, const QString &string)
{
    if (!string.isEmpty()) {
        m_categoriesSearchList.clear();
        for (int i = 0; i < m_categoriesList.length(); ++i) {
            int searchIndex = m_categoriesList[i]->getName().indexOf(string);
            if (searchIndex != -1) {
                m_categoriesSearchList.append(m_categoriesList[i]);
            }
        }
    }
    else {
        if (reread) {
            m_categoriesList = g_database->selectCategoriesTable();
        }
        m_categoriesSearchList = m_categoriesList;
    }

    ui->listWidget_categories->clear();
    for (auto &&datum : m_categoriesSearchList) {
        datum->setCount(g_database->selectNJCTableByCategoriesId(datum->getCategoriesId()).length());
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_categories);
        ui->listWidget_categories->addItem(item);
        CategoriesListCell *categoriesListCell = new CategoriesListCell(datum);
        item->setSizeHint(categoriesListCell->sizeHint());
        ui->listWidget_categories->setItemWidget(item, categoriesListCell);
    }
}

void MainWindow::on_listWidget_categories_doubleClicked(const QModelIndex &index)
{
    g_configTableModel->setCategoriesId(m_categoriesList[index.row()]->getCategoriesId());
}

void MainWindow::on_lineEdit_searchCategories_textChanged(const QString &arg1)
{
    arg1.isEmpty() ? setCategoriesList() : setCategoriesList(false, arg1);
}

void MainWindow::on_pushButton_changeTags_clicked()
{
    tagWidget = new TagsWidget(this);
    tagWidget->show();

    connect(this, &MainWindow::resizeChildWindow, tagWidget, &TagsWidget::resizeWindow);
    connect(tagWidget, SIGNAL(changeCategories()), this, SLOT(onChangeCategories()));
}
