#pragma once

#include <QtWidgets/QWidget>
#include "ui_CachedTableExample.h"

class QSqlTableModel;
class QPushButton;
class QDialogButtonBox;

class CachedTableExample : public QWidget
{
    Q_OBJECT

public:
    CachedTableExample(const QString& tableName, QWidget *parent = nullptr);
    ~CachedTableExample();

private slots:
    void submit();

private:
    QPushButton* submitButton;
    QPushButton* revertButton;
    QPushButton* quitButton;
    QDialogButtonBox* buttonBox;

    QSqlTableModel* model;
};
