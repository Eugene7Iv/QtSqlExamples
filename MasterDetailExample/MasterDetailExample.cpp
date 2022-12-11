#include "MasterDetailExample.h"
#include <QModelIndex>
#include <QFile>
#include <QtSql>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QTableView>
#include <QHeaderView>

#include "Dialog.h"

extern int uniqueAlbumId;
extern int uniqueArtistId;

MasterDetailExample::MasterDetailExample(const QString& artistTable, const QString& albumTable,
	QFile* albumDetails, QWidget* parent) : QMainWindow(parent)
{
	file = albumDetails;
	readAlbumData();

	model = new QSqlRelationalTableModel(this);
	model->setTable(albumTable);
	model->setRelation(2, QSqlRelation(artistTable, "id", "artist"));
	model->select();

	auto artists = createArtistGroupBox();
	auto albums = createAlbumGroupBox();
	auto details = createDetailsGroupBox();

	artistView->setCurrentIndex(0);
	uniqueAlbumId = model->rowCount();
	uniqueArtistId = artistView->count();

	connect(model, &QSqlRelationalTableModel::rowsInserted,
		this, &MasterDetailExample::updateHeader);
	connect(model, &QSqlRelationalTableModel::rowsRemoved,
		this, &MasterDetailExample::updateHeader);

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(artists, 0, 0);
	layout->addWidget(albums, 1, 0);
	layout->addWidget(details, 0, 1, 2, 1);
	layout->setColumnStretch(1, 1);
	layout->setColumnMinimumWidth(0, 500);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);
	setCentralWidget(widget);
	createMenuBar();

	showImageLabel();
	resize(850, 400);
	setWindowTitle(tr("Music Archive"));
}

void MasterDetailExample::about()
{
	QMessageBox::about(this, tr("About Music Archive"),
		tr("<p>The <b>Music Archive</b> example shows how to present "
			"data from different data sources in the same application. "
			"The album titles, and the corresponding artists and release dates, "
			"are kept in a database, while each album's tracks are stored "
			"in an XML file. </p><p>The example also shows how to add as "
			"well as remove data from both the database and the "
			"associated XML file using the API provided by the Qt SQL and "
			"Qt XML modules, respectively.</p>"));
}


void MasterDetailExample::addAlbum()
{
	Dialog* dialog = new Dialog(model, albumData, file, this);

	if (dialog->exec() == QDialog::Accepted)
	{
		int lastRow = model->rowCount() - 1;
		albumView->selectRow(lastRow);
		albumView->scrollToBottom();
		showAlbumDetails(model->index(lastRow, 0));
	}
}

void MasterDetailExample::changeArtist(int row)
{
	if (row > 0)
	{
		auto index = model->relationModel(2)->index(row, 1);
		model->setFilter("artist = '" + index.data().toString() + '\'');
		showArtistProfile(index);
	}
	else if (row == 0)
	{
		model->setFilter(QString());
		showImageLabel();
	}
	else
	{
		return;
	}
}

void MasterDetailExample::deleteAlbum()
{
	QModelIndexList selection = albumView->selectionModel()->selectedRows(0);
	
	if (!selection.empty())
	{
		QModelIndex idIndex = selection.at(0);
		int id = idIndex.data().toInt();
		QString title = idIndex.sibling(idIndex.row(), 1).data().toString();
		QString artist = idIndex.sibling(idIndex.row(), 2).data().toString();

		QMessageBox::StandardButton button;
		button = (QMessageBox::StandardButton)QMessageBox::question(this, tr("Delete Album"),
								tr("Are you sure you want to "
									"delete '%1' by '%2'?")
								.arg(title, artist), QMessageBox::Yes, QMessageBox::No
			);

		if (button == QMessageBox::Yes)
		{
			removeAlbumFromFile(id);
			removeAlbumFromDatabase(idIndex);
			decreaseAlbumCount(indexOfArtist(artist));

			showImageLabel();
		}
	}
	else
	{
		QMessageBox::information(this, tr("Delete Album"),
			tr("Select the album you want to delete."));
	}
}

void MasterDetailExample::showAlbumDetails(const QModelIndex& index)
{
	auto record = model->record(index.row());

	auto artist = record.value("artist").toString();
	auto title = record.value("title").toString();
	auto year = record.value("year").toString();
	auto albumId = record.value("albumId").toString();

	showArtistProfile(indexOfArtist(artist));
	titleLabel->setText(tr("Title : %1 (%2)").arg(title).arg(year));
	titleLabel->show();

	auto albums = albumData.elementsByTagName("album");
	for (int i = 0; albums.count(); i++)
	{
		auto album = albums.item(i);
		if (album.toElement().attribute("id") == albumId)
		{
			getTrackList(album.toElement());
			break;
		}
	}

	if (trackList->count() != 0)
		trackList->show();
}

void MasterDetailExample::showArtistProfile(const QModelIndex& index)
{
	auto record = model->relationModel(2)->record(index.row());

	auto name = record.value("artist").toString();
	auto count = record.value("albumcount").toString();
	profileLabel->setText(tr("Artist : %1 \n" \
							 "Number of Albums : %2").arg(name).arg(count));

	profileLabel->show();
	iconLabel->show();

	titleLabel->hide();
	trackList->hide();
	imageLabel->hide();
}

void MasterDetailExample::updateHeader(const QModelIndex&, int, int)
{
	adjustHeader();
}

void MasterDetailExample::adjustHeader()
{
	albumView->hideColumn(0);
	albumView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	albumView->resizeColumnToContents(2);
	albumView->resizeColumnToContents(3);
}

QGroupBox* MasterDetailExample::createAlbumGroupBox()
{
	QGroupBox* box = new QGroupBox(tr("Album"));

	albumView = new QTableView;
	albumView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	albumView->setSortingEnabled(true);
	albumView->setSelectionBehavior(QAbstractItemView::SelectRows);
	albumView->setSelectionMode(QAbstractItemView::SingleSelection);
	albumView->setShowGrid(false);
	albumView->verticalHeader()->hide();
	albumView->horizontalHeader()->show();
	albumView->setAlternatingRowColors(true);
	albumView->setModel(model);
	adjustHeader();

	QLocale locale = albumView->locale();
	locale.setNumberOptions(QLocale::OmitGroupSeparator);
	albumView->setLocale(locale);

	connect(albumView, &QTableView::clicked, this, &MasterDetailExample::showAlbumDetails);
	connect(albumView, &QTableView::activated, this, &MasterDetailExample::showAlbumDetails);

	auto layout = new QVBoxLayout;
	layout->addWidget(albumView, 0, {});
	box->setLayout(layout);

	return box;
}

QGroupBox* MasterDetailExample::createArtistGroupBox()
{
	artistView = new QComboBox;
	artistView->setModel(model->relationModel(2));
	artistView->setModelColumn(1);

	connect(artistView, qOverload<int>(&QComboBox::currentIndexChanged), this, &MasterDetailExample::changeArtist);

	auto box = new QGroupBox(tr("Artist"));
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(artistView, 0, 0);
	box->setLayout(layout);

	return box;
}

QGroupBox* MasterDetailExample::createDetailsGroupBox()
{
	auto box = new QGroupBox(tr("Details"));

	profileLabel = new QLabel;
	profileLabel->setWordWrap(true);
	profileLabel->setAlignment(Qt::AlignBottom);

	titleLabel = new QLabel;
	titleLabel->setWordWrap(true);
	titleLabel->setAlignment(Qt::AlignBottom);

	iconLabel = new QLabel;
	iconLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	iconLabel->setPixmap(QPixmap(":/images/icon.png"));

	imageLabel = new QLabel;
	imageLabel->setWordWrap(true);
	imageLabel->setAlignment(Qt::AlignCenter);
	imageLabel->setPixmap(QPixmap(":/images/image.png"));

	trackList = new QListWidget;

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(imageLabel, 0, 0, 3, 2);
	layout->addWidget(profileLabel, 0, 0);
	layout->addWidget(iconLabel, 0, 1);
	layout->addWidget(titleLabel, 1, 0, 1, 2);
	layout->addWidget(trackList, 2, 0, 1, 2);
	layout->setRowStretch(2, 1);
	box->setLayout(layout);

	return box;
}

void MasterDetailExample::createMenuBar()
{
	auto addAction = new QAction(tr("&Add album..."), this);
	auto deleteAction = new QAction(tr("&Delete album..."), this);
	auto quitAction = new QAction(tr("&Quit"), this);
	auto aboutAction = new QAction(tr("&About"), this);
	auto aboutQtAction = new QAction(tr("About &Qt"), this);

	addAction->setShortcut(tr("Ctrl+A"));
	deleteAction->setShortcut(tr("Ctrl+D"));
	quitAction->setShortcuts(QKeySequence::Quit);

	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(addAction);
	fileMenu->addAction(deleteAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(aboutQtAction);

	connect(addAction, &QAction::triggered, this, &MasterDetailExample::addAlbum);
	connect(deleteAction, &QAction::triggered, this, &MasterDetailExample::deleteAlbum);
	connect(quitAction, &QAction::triggered, this, &MasterDetailExample::close);
	connect(aboutAction, &QAction::triggered, this, &MasterDetailExample::about);
	connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MasterDetailExample::decreaseAlbumCount(QModelIndex artistIndex)
{
	const int row = artistIndex.row();
	auto countIndex = artistIndex.sibling(row, 2);
	int albumCount = countIndex.data().toInt();

	QSqlTableModel* artistsModel = model->relationModel(2);

	if (albumCount == 1)
	{
		artistsModel->removeRow(row);
		showImageLabel();
	}
	else
	{
		artistsModel->setData(countIndex, QVariant(albumCount - 1));
	}
}

void MasterDetailExample::getTrackList(QDomNode album)
{
	trackList->clear();

	auto tracks = album.childNodes();
	QDomNode track;
	QString trackNumber;

	for (int i = 0; i < tracks.count(); i++)
	{
		track = tracks.item(i);
		trackNumber = track.toElement().attribute("number");

		QListWidgetItem* item = new QListWidgetItem(trackList);
		item->setText(trackNumber + " : " + track.toElement().text());
	}
}

QModelIndex MasterDetailExample::indexOfArtist(const QString& artist)
{
	auto artistModel = model->relationModel(2);

	for (int i = 0; i < artistModel->rowCount(); i++)
	{
		auto record = artistModel->record(i);
		if (record.value("artist").toString() == artist)
			return artistModel->index(i, 1);
	}

	return QModelIndex();
}

void MasterDetailExample::readAlbumData()
{
	if (!file->open(QIODevice::ReadOnly))
		return;

	if (!albumData.setContent(file))
	{
		file->close();
		return;
	}

	file->close();
}

void MasterDetailExample::removeAlbumFromDatabase(QModelIndex index)
{
	model->removeRow(index.row());
}

void MasterDetailExample::removeAlbumFromFile(int id)
{
	QDomNodeList albums = albumData.elementsByTagName("album");

	for (int i = 0; i < albums.count(); i++)
	{
		QDomNode album = albums.item(i);
		if (album.toElement().attribute("id").toInt() == id)
		{
			albumData.elementsByTagName("archive").item(0).removeChild(album);
			break;
		}
	}

	/*
	The following code is commented out since the example uses an in
	memory database, i.e., altering the XML file will bring the data
	out of sync.

	if (!file->open(QIODevice::WriteOnly)) {
		return;
	} else {
		QTextStream stream(file);
		albumData.elementsByTagName("archive").item(0).save(stream, 4);
		file->close();
	}
*/
}

void MasterDetailExample::showImageLabel()
{
	profileLabel->hide();
	titleLabel->hide();
	iconLabel->hide();
	trackList->hide();

	imageLabel->show();
}
