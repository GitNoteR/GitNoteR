#include "configmodel.h"
#include "globals.h"
#include "tools.h"
#include "markdowneditorwidget.h"

ConfigModel::ConfigModel()
{
    mVersion = "version";
    mCutRectKeySequence = "keySequence/cutRect";
    mCutFullKeySequence = "keySequence/cutFull";
    mCutWindowKeySequence = "keySequence/cutWindow";
    mRepoDir = "git/repoDir";
    mRepoUrl = "git/repoUrl";
    mRepoEmail = "git/repoEmail";
    mRepoUsername = "git/repoUsername";
    mRepoPassword = "git/repoPassword";
    mRepoUrlNew = "git/repoUrlNew";
    mRepoEmailNew = "git/repoEmailNew";
    mRepoPasswordNew = "git/repoPasswordNew";
    mRepoStatus = "git/repoStatus";
    mAutoSyncRepoTime = "git/autoSyncRepoTime";
    mOpenMainWindowNoteUuid = "editor/openMainWindowNoteUuid";
    mOpenWindowListNoteUuid = "editor/openWindowListNoteUuid";
    mSideSelectedType = "groupTree/selectedType";
    mSideSelectedName = "groupTree/selectedName";
    mAutoLockTime = "lock/autoLockTime";
    mUnlockPassword = "lock/unlockPassword";
    mEditorFont = "editor/font";
    mEditorSplitterSizes = "editor/splitterSizes";
    mEditorBackgroundSplitterSizes = "editor/backgroundSplitterSizes";
    mEditorWindowRect = "editorWindow/rect";
    mTheme = "window/theme";
    mSidebarWidget = "window/sidebarWidget";
    mListBarWidget = "window/listBarWidget";
    mToolbarWidget = "window/toolbarWidget";
    mNavigationBarWidget = "window/navigationBarWidget";
    mMainWindowSplitterSizes = "mainWindow/splitterSizes";
    mMainWindowRect = "mainWindow/rect";
    mMainWindowFullScreen = "mainWindow/fullScreen";
    mHasLockWindow = "mainWindow/hasLock";
    mNoteSortBasis = "noteList/noteSortBasis";
    mNoteSortType = "noteList/noteSortType";
}

void ConfigModel::init()
{
    Tools::createMkDir(__AppDataPath__);
    settings = new QSettings(__AppConfigPath__, QSettings::IniFormat);
}

template <typename T>
QVariantList ConfigModel::toVariantList(const QList<T> &list)
{
    QVariantList newList;
    for (auto &&item : list) {
        newList << item;
    }

    return newList;
}

template <typename T>
QList<T> ConfigModel::fromVariantList(const QVariantList &list)
{
    QList<T> newList;
    for (auto &&item : list) {
        newList << item.value<T>();
    }

    return newList;
}


void ConfigModel::setRepoDir(const QString &repoDir)
{
    settings->setValue(mRepoDir, repoDir);
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    settings->setValue(mRepoUrl, repoUrl);
}

void ConfigModel::setRepoUrlNew(const QString &repoUrl)
{
    settings->setValue(mRepoUrlNew, repoUrl);
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    settings->setValue(mRepoUsername, repoUsername);
}

void ConfigModel::setRepoEmailNew(const QString &email)
{
    settings->setValue(mRepoEmailNew, email);
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    settings->setValue(mRepoPassword, Tools::encrypt(repoPassword));
}

void ConfigModel::setRepoPasswordNew(const QString &repoPassword)
{
    settings->setValue(mRepoPasswordNew, Tools::encrypt(repoPassword));
}

void ConfigModel::setOpenMainWindowNoteUuid(const QString &openNoteUuid)
{
    settings->setValue(mOpenMainWindowNoteUuid, openNoteUuid);
}

const QString ConfigModel::getVersion() const
{
    return settings->value(mVersion).toString();
}

const QString ConfigModel::getRepoDir() const
{
    return settings->value(mRepoDir, "").toString();
}

const QString ConfigModel::getRepoUrl() const
{
    return settings->value(mRepoUrl, "").toString();
}

QString ConfigModel::getRepoUrlNew()
{
    return settings->value(mRepoUrlNew, "").toString();
}

const QString ConfigModel::getRepoUsername() const
{
    return settings->value(mRepoUsername, "Gitnoter").toString();
}

const QString ConfigModel::getRepoEmailNew() const
{
    return settings->value(mRepoEmailNew, "").toString();
}

const QString ConfigModel::getRepoPassword() const
{
    const QString password = settings->value(mRepoPassword, "").toString();
    return password.isEmpty() ? password : Tools::decrypt(password);
}

const QString ConfigModel::getRepoPasswordNew() const
{
    const QString password = settings->value(mRepoPasswordNew, "").toString();
    return password.isEmpty() ? password : Tools::decrypt(password);
}

const QString ConfigModel::openMainWindowNoteUuid() const
{
    return settings->value(mOpenMainWindowNoteUuid, "").toString();
}

QString ConfigModel::getSideSelectedName() const
{
    return settings->value(mSideSelectedName, "").toString();
}

void ConfigModel::setSideSelected(Gitnoter::GroupType type, const QString &name)
{
    settings->setValue(mSideSelectedType, type);
    settings->setValue(mSideSelectedName, name);
}

void ConfigModel::setSideSelected(GroupModel *groupModel)
{
    settings->setValue(mSideSelectedType, groupModel->getType());
    settings->setValue(mSideSelectedName, groupModel->getName());
}

Gitnoter::RepoStatus ConfigModel::getLocalRepoStatus() const
{
    return (Gitnoter::RepoStatus) settings->value(mRepoStatus, Gitnoter::NoneRepo).toInt();
}

void ConfigModel::setLocalRepoStatus(Gitnoter::RepoStatus status)
{
    settings->setValue(mRepoStatus, status);
}

Gitnoter::GroupType ConfigModel::getSideSelectedType() const
{
    return (Gitnoter::GroupType) settings->value(mSideSelectedType, Gitnoter::All).toInt();
}

const QString ConfigModel::getRepoEmail() const
{
    return settings->value(mRepoEmail, "").toString();
}

void ConfigModel::setRepoEmail(const QString &repoEmail)
{
    settings->setValue(mRepoEmail, repoEmail);
}

int ConfigModel::getAutoSyncRepoTime() const
{
    return settings->value(mAutoSyncRepoTime, 15 * 60 * 1000).toInt();
}

void ConfigModel::setAutoSyncRepoTime(int autoSyncRepoTime)
{
    settings->setValue(mAutoSyncRepoTime, autoSyncRepoTime);
}

int ConfigModel::getAutoLockTime() const
{
    return settings->value(mAutoLockTime, 15 * 60 * 1000).toInt();
}

void ConfigModel::setAutoLockTime(int autoLockTime)
{
    settings->setValue(mAutoLockTime, autoLockTime);
}

const QString ConfigModel::getUnlockPassword() const
{
    return settings->value(mUnlockPassword, "").toString();
}

void ConfigModel::setUnlockPassword(const QString &unlockPassword)
{
    settings->setValue(mUnlockPassword, unlockPassword);
}

QFont ConfigModel::getEditorFont() const
{
    QFont font;
    font.fromString(settings->value(mEditorFont, QFont().defaultFamily()).toString());

    return font;
}

void ConfigModel::setEditorFont(const QFont &font)
{
    settings->setValue(mEditorFont, font.toString());
}

const QString ConfigModel::getCutRectKeySequence() const
{
    const QString keySequence = settings->value(mCutRectKeySequence, __CutRectKeySequence__).toString();
    return keySequence.isEmpty() ? __CutRectKeySequence__ : keySequence;
}

void ConfigModel::setCutRectKeySequence(const QString &keySequence)
{
    settings->setValue(mCutRectKeySequence, keySequence);
}

const QString ConfigModel::getCutFullKeySequence() const
{
    const QString keySequence = settings->value(mCutFullKeySequence, __CutFullKeySequence__).toString();
    return keySequence.isEmpty() ? __CutFullKeySequence__ : keySequence;
}

void ConfigModel::setCutFullKeySequence(const QString &keySequence)
{
    settings->setValue(mCutFullKeySequence, keySequence);
}

const QString ConfigModel::getCutWindowKeySequence() const
{
    const QString keySequence = settings->value(mCutWindowKeySequence, __CutWindowKeySequence__).toString();
    return keySequence.isEmpty() ? __CutWindowKeySequence__ : keySequence;
}

void ConfigModel::setCutWindowKeySequence(const QString &keySequence)
{
    settings->setValue(mCutWindowKeySequence, keySequence);
}

ThemeManager::ThemeFlag ConfigModel::getTheme() const
{
    return (ThemeManager::ThemeFlag) settings->value(mTheme, ThemeManager::Default).toInt();
}

void ConfigModel::setTheme(const ThemeManager::ThemeFlag &theme)
{
    settings->setValue(mTheme, theme);
}

QList<int> ConfigModel::getMainWindowSplitterSizes()
{
    return fromVariantList<int>(settings->value(mMainWindowSplitterSizes, {0, 0, 0}).toList());
}

void ConfigModel::setMainWindowSplitterSizes(const QList<int> &splitterSizes)
{
    settings->setValue(mMainWindowSplitterSizes, toVariantList(splitterSizes));
}

QRect ConfigModel::getMainWindowRect()
{
    return settings->value(mMainWindowRect, QRect()).toRect();
}

void ConfigModel::setMainWindowRect(const QRect &rect)
{
    settings->setValue(mMainWindowRect, rect);
}

QRect ConfigModel::getEditorWindowRect()
{
    return settings->value(mEditorWindowRect, QRect()).toRect();
}

void ConfigModel::setEditorWindowRect(const QRect &rect)
{
    settings->setValue(mEditorWindowRect, rect);
}

Gitnoter::SortBasis ConfigModel::getNoteSortBasis() const
{
    return (Gitnoter::SortBasis) settings->value(mNoteSortBasis, Gitnoter::Title).toInt();
}

void ConfigModel::setNoteSortBasis(Gitnoter::SortBasis noteSortBasis)
{
    settings->setValue(mNoteSortBasis, noteSortBasis);
}

Qt::SortOrder ConfigModel::getNoteSortType() const
{
    return (Qt::SortOrder) settings->value(mNoteSortType, Qt::AscendingOrder).toInt();
}

void ConfigModel::setNoteSortType(Qt::SortOrder noteSortType)
{
    settings->setValue(mNoteSortType, noteSortType);
}

void ConfigModel::setNoteSort(Gitnoter::SortBasis noteSortBasis, Qt::SortOrder noteSortType)
{
    settings->setValue(mNoteSortBasis, noteSortBasis);
    settings->setValue(mNoteSortType, noteSortType);
}

void ConfigModel::setOpenWindowListNoteUuid(QWidgetList widgetList)
{
    QStringList uuidList = {};
    for (auto &&widget : widgetList) {
        if (MarkdownEditorWidget *markdownEditorWidget
                = reinterpret_cast<MarkdownEditorWidget *>(qobject_cast<MarkdownEditorWidget *>(widget))) {
            uuidList.append(markdownEditorWidget->noteModel()->getUuid());
        }
    }

    settings->setValue(mOpenWindowListNoteUuid, uuidList);
}

void ConfigModel::appendOpenWindowListNoteUuid(const QString &noteUuid)
{
    QStringList uuidList = openWindowListNoteUuid();
    uuidList.append(noteUuid);
    settings->setValue(mOpenWindowListNoteUuid, uuidList);
}

void ConfigModel::removeOpenWindowListNoteUuid(const QString &noteUuid)
{
    QStringList uuidList = openWindowListNoteUuid();
    uuidList.removeOne(noteUuid);
    settings->setValue(mOpenWindowListNoteUuid, uuidList);
}

void ConfigModel::clearOpenWindowListNoteUuid()
{
    settings->setValue(mOpenWindowListNoteUuid, {});
}

QStringList ConfigModel::openWindowListNoteUuid() const
{
    return settings->value(mOpenWindowListNoteUuid, {}).toStringList();
}

QList<int> ConfigModel::getEditorSplitterSizes()
{
    return fromVariantList<int>(settings->value(mEditorSplitterSizes, {0, 0}).toList());
}

void ConfigModel::setEditorSplitterSizes(const QList<int> &splitterSizes)
{
    settings->setValue(mEditorSplitterSizes, toVariantList(splitterSizes));
}

bool ConfigModel::getMainWindowFullScreen() const
{
    return settings->value(mMainWindowFullScreen, false).toBool();
}

void ConfigModel::setMainWindowFullScreen(bool fullScreen)
{
    settings->setValue(mMainWindowFullScreen, fullScreen);
}

bool ConfigModel::getSidebarWidget() const
{
    return settings->value(mSidebarWidget, true).toBool();
}

void ConfigModel::setSidebarWidget(bool b)
{
    settings->setValue(mSidebarWidget, b);
}

bool ConfigModel::getListBarWidget() const
{
    return settings->value(mListBarWidget, true).toBool();
}

void ConfigModel::setListBarWidget(bool b)
{
    settings->setValue(mListBarWidget, b);
}

bool ConfigModel::getToolbarWidget() const
{
    return settings->value(mToolbarWidget, true).toBool();
}

void ConfigModel::setToolbarWidget(bool b)
{
    settings->setValue(mToolbarWidget, b);
}

bool ConfigModel::getNavigationBarWidget() const
{
    return settings->value(mNavigationBarWidget, true).toBool();
}

void ConfigModel::setNavigationBarWidget(bool b)
{
    settings->setValue(mNavigationBarWidget, b);
}

QList<int> ConfigModel::getEditorBackgroundSplitterSizes()
{
    return fromVariantList<int>(settings->value(mEditorBackgroundSplitterSizes, {0, 0}).toList());
}

void ConfigModel::setEditorBackgroundSplitterSizes(const QList<int> &sizes)
{
    settings->setValue(mEditorBackgroundSplitterSizes, toVariantList(sizes));
}

bool ConfigModel::getHasLockWindow()
{
    return settings->value(mHasLockWindow, false).toBool();
}

void ConfigModel::setHasLockWindow(bool hasLock)
{
    settings->setValue(mHasLockWindow, hasLock);
}
