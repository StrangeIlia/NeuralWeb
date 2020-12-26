#ifndef PRDEFINITIONS_H
#define PRDEFINITIONS_H

#include <QHash>
#include <QSharedPointer>
#include <QAbstractItemModel>
#include "neural_networks/Definitions.h"

typedef QSharedPointer<Matrix>                  MatrixPtr;
typedef QSharedPointer<QVariantHash>            QVariantHashPtr;
typedef QSharedPointer<QAbstractItemModel>      QAbstractItemModelPtr;

Q_DECLARE_METATYPE(MatrixPtr);
Q_DECLARE_METATYPE(QVariantHashPtr);
Q_DECLARE_METATYPE(QAbstractItemModelPtr);

#endif // PRDEFINITIONS_H
