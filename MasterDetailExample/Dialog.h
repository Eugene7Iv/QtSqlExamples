#pragma once
#include <QtWidgets>
#include <QtSql>
#include <QtXml>;

class Dialog : public QDialog
{
public:
	Dialog(QSqlRelationalTableModel* albums, QDomDocument details,
		QFile* output, QWidget* parent = nullptr);

private slots:
	void revert();
	void submit();

private:
	int addNewAlbum(const QString& title, int artistId);
	int addNewArtist(const QString& name);
	void addTracks(int albumId, const QStringList& tracks);
	QDialogButtonBox* createButtons();
	QGroupBox* createInputWidgets();
	int findArtistId(const QString& artist);
	static int generateAlbumId();
	static int generateArtistId();
	void increaseAlbumCount(QModelIndex artistindex);
	QModelIndex indexOfArtist(const QString& artist);

	QSqlRelationalTableModel* model;
	QDomDocument albumDetails;
	QFile* outputFile;

	QLineEdit* artistEditor;
	QLineEdit* titleEditor;
	QSpinBox* yearEditor;
	QLineEdit* tracksEditor;
};

