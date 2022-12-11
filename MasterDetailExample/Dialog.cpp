#include "Dialog.h"

int uniqueAlbumId;
int uniqueArtistId;

Dialog::Dialog(QSqlRelationalTableModel* albums, QDomDocument details,
	QFile* output, QWidget* parent) : QDialog(parent)
{
	model = albums;
	albumDetails = details;
	outputFile = output;

	auto inputWidgetBox = createInputWidgets();
	auto buttonBox = createButtons();

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(inputWidgetBox);
	layout->addWidget(buttonBox);

	setLayout(layout);

	setWindowTitle("Add Album");
}

void Dialog::revert()
{
	artistEditor->clear();
	titleEditor->clear();
	yearEditor->setValue(QDate::currentDate().year());
	tracksEditor->clear();
}

void Dialog::submit()
{
	QString artist = artistEditor->text();
	QString title = titleEditor->text();

	if (artist.isEmpty() || title.isEmpty())
	{
		QString message(tr("Please provide both the name of the artist "
			"and the title of the album."));
		QMessageBox::information(this, tr("Add Album"), message);
	}
	else
	{
		int artistId = findArtistId(artist);
		int albumId = addNewAlbum(title, artistId);

		QStringList tracks;
		tracks = tracksEditor->text().split(QLatin1Char(','), Qt::SkipEmptyParts);
		addTracks(albumId, tracks);

		increaseAlbumCount(indexOfArtist(artist));
		accept();
	}
}

int Dialog::addNewAlbum(const QString& title, int artistId)
{
	int id = generateAlbumId();
	QSqlRecord record;

	QSqlField f1("albumId", QVariant::Type::Int);
	QSqlField f2("title", QVariant::Type::String);
	QSqlField f3("artistId", QVariant::Type::Int);
	QSqlField f4("year", QVariant::Type::Int);

	f1.setValue(id);
	f2.setValue(title);
	f3.setValue(artistId);
	f4.setValue(yearEditor->value());
	record.append(f1);
	record.append(f2);
	record.append(f3);
	record.append(f4);

	model->insertRecord(-1, record);

	return id;
}

int Dialog::addNewArtist(const QString& name)
{
	QSqlTableModel* artistModel = model->relationModel(2);
	QSqlRecord record;

	int id = generateArtistId();

	QSqlField f1(QString("id"), QVariant::Type::Int);
	QSqlField f2(QString("artist"), QVariant::Type::String);
	QSqlField f3(QString("albumcount"), QVariant::Type::Int);

	f1.setValue(QVariant(id));
	f2.setValue(QVariant(name));
	f3.setValue(QVariant(0));

	record.append(f1);
	record.append(f2);
	record.append(f3);

	artistModel->insertRecord(-1, record);
	return id;
}

void Dialog::addTracks(int albumId, const QStringList& tracks)
{
	QDomElement albumNode = albumDetails.createElement("album");
	albumNode.setAttribute("id", albumId);
	
	for (int i = 0; i < tracks.count(); i++)
	{
		QString trackNumber = QString::number(i);
		if (i < 10)
			trackNumber.prepend("0");

			QDomText textNode = albumDetails.createTextNode(tracks.at(i));
			
			QDomElement trackNode = albumDetails.createElement("track");
			trackNode.setAttribute("number", trackNumber);
			trackNode.appendChild(textNode);

			albumNode.appendChild(trackNode);
	}
	auto archive = albumDetails.elementsByTagName("archive");
	archive.item(0).appendChild(albumNode);

	/*
	The following code is commented out since the example uses an in
	memory database, i.e., altering the XML file will bring the data
	out of sync.

	if (!outputFile->open(QIODevice::WriteOnly)) {
		return;
	} else {
		QTextStream stream(outputFile);
		archive.item(0).save(stream, 4);
		outputFile->close();
	}
*/
}

QDialogButtonBox* Dialog::createButtons()
{
	auto closeButton = new QPushButton(tr("&Close"));
	auto revertButton = new QPushButton(tr("&Revert"));
	auto submitButton = new QPushButton(tr("&Submit"));

	closeButton->setDefault(true);

	connect(closeButton, &QPushButton::clicked, this, &Dialog::close);
	connect(revertButton, &QPushButton::clicked, this, &Dialog::revert);
	connect(submitButton, &QPushButton::clicked, this, &Dialog::submit);

	auto buttonBox = new QDialogButtonBox;

	buttonBox->addButton(submitButton, QDialogButtonBox::ResetRole);
	buttonBox->addButton(revertButton, QDialogButtonBox::ResetRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

	return buttonBox;
}

QGroupBox* Dialog::createInputWidgets()
{
	auto box = new QGroupBox(tr("Add Album"));

	auto artistLable = new QLabel(tr("Artist:"));
	auto titleLable = new QLabel(tr("Title:"));
	auto yearLable = new QLabel(tr("Year:"));
	auto tracksLable = new QLabel(tr("Tracks (separated by comma):"));

	artistEditor = new QLineEdit;
	titleEditor = new QLineEdit;

	yearEditor = new QSpinBox;
	yearEditor->setMinimum(1900);
	yearEditor->setMaximum(QDate::currentDate().year());
	yearEditor->setValue(yearEditor->maximum());
	yearEditor->setReadOnly(false);

	tracksEditor = new QLineEdit;

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(artistLable, 0, 0);
	layout->addWidget(artistEditor, 0, 1);
	layout->addWidget(titleLable, 1, 0);
	layout->addWidget(titleEditor, 1, 1);
	layout->addWidget(yearLable, 2, 0);
	layout->addWidget(yearEditor, 2, 1);
	layout->addWidget(tracksLable, 3, 0, 1, 2);
	layout->addWidget(tracksEditor, 4, 0, 1, 2);

	box->setLayout(layout);

	return box;
}

int Dialog::findArtistId(const QString& artist)
{
	QSqlTableModel* artistModel = model->relationModel(2);
	int row = 0;

	while (row < artistModel->rowCount())
	{
		QSqlRecord record = artistModel->record(row);
		if (record.value("artist") == artist)
			return record.value("id").toInt();
		else
			row++;
	}

	return addNewArtist(artist);
}

int Dialog::generateAlbumId()
{
	uniqueAlbumId++;
	return uniqueAlbumId;
}

int Dialog::generateArtistId()
{
	uniqueArtistId++;
	return uniqueArtistId;;
}

void Dialog::increaseAlbumCount(QModelIndex artistindex)
{
	QSqlTableModel* artistModel = model->relationModel(2);

	QModelIndex albumCountIndex = artistindex.sibling(artistindex.row(), 2);

	int albumCount = albumCountIndex.data().toInt();
	artistModel->setData(albumCountIndex, QVariant(albumCount + 1));
}

QModelIndex Dialog::indexOfArtist(const QString& artist)
{
	QSqlTableModel* artistModel = model->relationModel(2);

	for (int i = 0; i < artistModel->rowCount(); i++)
	{
		QSqlRecord record = artistModel->record(i);
		if (record.value("artist") == artist)
			return artistModel->index(i, 1);
	}

	return QModelIndex();
}
