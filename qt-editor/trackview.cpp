#include "trackview.h"
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QHeaderView>

TrackModel::TrackModel(QObject *parent)
 : QAbstractTableModel(parent)
{
}

int TrackModel::rowCount(const QModelIndex & /* parent */) const
{
	return 128;
}

int TrackModel::columnCount(const QModelIndex & /* parent */) const
{
	return 32;
}

Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QString str;
	switch (role) {
	case Qt::DisplayRole:
		if (datacontent.contains(index))
			return datacontent[index];
		return QVariant();

	case Qt::BackgroundRole:
		if (index.row() % 4 == 0 && index.column() % 2 == 0)
			return QBrush(QColor(255, 0, 0, 32));
		if (index.row() % 8 == 0)
			return QBrush(QColor(0, 0, 0, 16));
		return QVariant();

	case Qt::EditRole:
		if (index.row() % 4 != 0)
			return QVariant();
		return str.setNum(float(index.row()) / 16);
	}

	return QVariant();
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool ok;
	value.toFloat(&ok);

	if (!ok)
		datacontent.remove(index);
	else
		datacontent[index] = value.toFloat();

	dataChanged(index, index);
	return true;
}


QVariant TrackModel::headerData(int section,
    Qt::Orientation orientation,
    int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal)
			return QVariant(QString("header %1").arg(section));
		else
			return QVariant(QString("%1").arg(section, 4, 16, QLatin1Char('0')));
	}
	return QVariant();
}

TrackView::TrackView(QWidget *parent) : QTableView(parent)
{
	setModel(new TrackModel());

	QFontMetrics fm(font());

	/* The padding of 4 in "lineSpacing() + 4" has been found by
	 * trial-and-error. Do not lower it, but increasing it is fine.
	 */
	verticalHeader()->setDefaultSectionSize(fm.lineSpacing() + 4);
	horizontalHeader()->setDefaultSectionSize(fm.averageCharWidth() * 16);

	verticalHeader()->setResizeMode(QHeaderView::Fixed);
}
