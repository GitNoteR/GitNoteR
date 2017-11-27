/*
 * Copyright (c) 2014-2017 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui {
class QTextEditSearchWidget;
}

class QTextEditSearchWidget : public QWidget
{
    Q_OBJECT

public:
    enum SearchMode {
        PlainTextMode,
        WholeWordsMode,
        RegularExpressionMode
    };

    explicit QTextEditSearchWidget(QPlainTextEdit *parent = 0);
    bool doSearch(bool searchDown = true, bool allowRestartAtTop = true);
    void setDarkMode(bool enabled);
    ~QTextEditSearchWidget();

private:
    Ui::QTextEditSearchWidget *ui;

protected:
    QPlainTextEdit *_textEdit;
    bool _darkMode;
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void activate();
    void deactivate();
    void doSearchDown();
    void doSearchUp();
    void setReplaceMode(bool enabled);
    void activateReplace();
    bool doReplace(bool forAll = false);
    void doReplaceAll();

protected slots:
    void searchLineEditTextChanged(const QString &arg1);
};
