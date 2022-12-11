#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WidgetMapperExample.h"

class QLabel;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QSqlRelationalTableModel;
class QItemSelectionModel;
class QDataWidgetMapper;

class WidgetMapperExample : public QWidget
{
    Q_OBJECT

public:
    WidgetMapperExample(QWidget *parent = nullptr);
    
private slots: 
    void updateButtons(int row);
    
private:
    void setupModel();

    QLabel* nameLabel;
    QLabel* addressLabel;
    QLabel* typeLabel;
    QLineEdit* nameEdit;
    QTextEdit* addressEdit;
    QComboBox* typeComboBox;
    QPushButton* nextButton;
    QPushButton* prevButton;

    QSqlRelationalTableModel* model;
    QItemSelectionModel* selectionModel;
    QDataWidgetMapper* mapper;

    int typeIndex;
    
};
