#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MasterDetailExample.h"
#include <QDomDocument>

class QGroupBox;
class QTableView;
class QListWidget;
class QLabel;
class QFile;
class QSqlRelationalTableModel;

class MasterDetailExample : public QMainWindow
{
    Q_OBJECT

public:
    MasterDetailExample(const QString& artistTable, const QString& albumTable,
        QFile* albumDetails, QWidget * parent = nullptr);

private slots:
    void about();
    void addAlbum();
    void changeArtist(int row);
    void deleteAlbum();
    void showAlbumDetails(const QModelIndex& index);
    void showArtistProfile(const QModelIndex& index);
    void updateHeader(const QModelIndex&, int, int);

private:
    void adjustHeader();
    QGroupBox* createAlbumGroupBox();
    QGroupBox* createArtistGroupBox();
    QGroupBox* createDetailsGroupBox();
    void createMenuBar();
    void decreaseAlbumCount(QModelIndex artistIndex);
    void getTrackList(QDomNode album);
    QModelIndex indexOfArtist(const QString& artist);
    void readAlbumData();
    void removeAlbumFromDatabase(QModelIndex index);
    void removeAlbumFromFile(int id);
    void showImageLabel();

    QTableView* albumView;
    QComboBox* artistView;
    QListWidget* trackList;

    QLabel* iconLabel;
    QLabel* imageLabel;
    QLabel* profileLabel;
    QLabel* titleLabel;

    QDomDocument albumData;
    QFile* file;
    QSqlRelationalTableModel* model;
};
