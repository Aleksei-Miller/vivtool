#ifndef INTERFACE_H
#define INTERFACE_H

#include <QString>

class Interface{
public:
    virtual bool read(const QString &){ return false; }
    virtual bool write(const QString &){ return false; }
};

#endif // INTERFACE_H
