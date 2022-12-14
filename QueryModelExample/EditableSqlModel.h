#pragma once
#include <QSqlQueryModel>
#include <Qt>

class EditableSqlModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	EditableSqlModel(QObject* parent = nullptr);

	Qt::ItemFlags flags(const QModelIndex& index) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
	bool setFirstName(int personId, const QString& firstName);
	bool setLastName(int personId, const QString& lastName);
	void refresh();
};

