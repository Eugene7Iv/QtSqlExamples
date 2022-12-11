#include "WidgetMapperExample.h"
#include <QtSql>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QDataWidgetMapper>
#include <QGridLayout>

WidgetMapperExample::WidgetMapperExample(QWidget *parent)
    : QWidget(parent)
{
    setupModel();

    nameLabel = new QLabel(tr("Na&me:"));
    nameEdit = new QLineEdit;
    nameLabel->setBuddy(nameEdit);

    addressLabel = new QLabel(tr("&Address:"));
    addressEdit = new QTextEdit;
    addressLabel->setBuddy(addressEdit);

    typeLabel = new QLabel(tr("&Type:"));
    typeComboBox = new QComboBox;
    typeLabel->setBuddy(typeComboBox);

    nextButton = new QPushButton(tr("&Next"));
    prevButton = new QPushButton(tr("&Previous"));

    QSqlTableModel* addressTypeModel = model->relationModel(typeIndex);
    typeComboBox->setModel(addressTypeModel);
    typeComboBox->setModelColumn(addressTypeModel->fieldIndex("description"));

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(nameEdit, model->fieldIndex("name"));
    mapper->addMapping(addressEdit, model->fieldIndex("address"));
    mapper->addMapping(typeComboBox, typeIndex);

    connect(prevButton, &QPushButton::clicked,
        mapper, &QDataWidgetMapper::toPrevious);
    connect(nextButton, &QPushButton::clicked,
        mapper, &QDataWidgetMapper::toNext);
    connect(mapper, &QDataWidgetMapper::currentIndexChanged,
        this, &WidgetMapperExample::updateButtons);

    QGridLayout* layout = new QGridLayout;
    

    layout->addWidget(nameLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(nameEdit, 0, 1, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(prevButton, 0, 2, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(addressLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(addressEdit, 1, 1, 2, 1, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(nextButton, 1, 2, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(typeLabel, 3, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(typeComboBox, 3, 1, Qt::AlignLeft | Qt::AlignTop);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    layout->setColumnMinimumWidth(1, 200);
    
    setLayout(layout);

    setWindowTitle(tr("SQL Widget Mapper"));
    mapper->toFirst();
}

void WidgetMapperExample::updateButtons(int row)
{
    prevButton->setEnabled(row != 0);
    nextButton->setEnabled(row != model->rowCount() - 1);
}

void WidgetMapperExample::setupModel()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open())
    {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to establish a database connection.\n"
                "This example needs SQLite support. Please read "
                "the Qt SQL driver documentation for information how "
                "to build it."), QMessageBox::Cancel);
        return;
    }

    QSqlQuery query;

    query.exec("create table person (id int primary key, "
        "name varchar(20), address varchar(200), typeid int)");
    query.exec("insert into person values(1, 'Alice', "
        "'<qt>123 Main Street<br/>Market Town</qt>', 101)");
    query.exec("insert into person values(2, 'Bob', "
        "'<qt>PO Box 32<br/>Mail Handling Service"
        "<br/>Service City</qt>', 102)");
    query.exec("insert into person values(3, 'Carol', "
        "'<qt>The Lighthouse<br/>Remote Island</qt>', 103)");
    query.exec("insert into person values(4, 'Donald', "
        "'<qt>47338 Park Avenue<br/>Big City</qt>', 101)");
    query.exec("insert into person values(5, 'Emma', "
        "'<qt>Research Station<br/>Base Camp<br/>"
        "Big Mountain</qt>', 103)");

    query.exec("create table addresstype (id int, description varchar(20))");
    query.exec("insert into addresstype values(101, 'Home')");
    query.exec("insert into addresstype values(102, 'Work')");
    query.exec("insert into addresstype values(103, 'Other')");

    model = new QSqlRelationalTableModel(this);
    model->setTable("person");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    typeIndex = model->fieldIndex("typeid");
    model->setRelation(typeIndex, QSqlRelation("addresstype", "id", "description"));

    model->select();
}
