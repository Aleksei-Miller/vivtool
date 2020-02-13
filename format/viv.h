#ifndef VIV_H
#define VIV_H

#include <QtGlobal>

#include <QString>
#include <QVector>

#include <QFile>
#include <QDataStream>

#include <QDebug>

#include "abstract/abstractcontainer.h"

#include "interface.h"

struct Chunk{
    QString name;
    QByteArray data;
};

class Viv : public AbstractContainer<Chunk>, public Interface
{
private:
    QString _path;

public:
    Viv(const QString &path = "");
    ~Viv();

    bool read(const QString &path) override;
    bool write(const QString &path) override;
};

#endif // VIV_H
