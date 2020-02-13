#ifndef ABSTRACTCONTAINER_H
#define ABSTRACTCONTAINER_H

#include <QVector>

template <class T>
class AbstractContainer
{
private:
    QVector<T*> _vector;

public:
    virtual ~AbstractContainer()
    {
        foreach(T *t, _vector)
            delete t;
    }

    T *at(int index)
    {
        return index < 0 || index > _vector.count() || _vector.isEmpty() ? NULL : _vector.at(index);
    }

    bool set(int index, T &t)
    {
        if (index < 0 || index > _vector.count() || _vector.isEmpty())
            return false;
        else
        {
            //Delete old
            delete _vector[index];

            //
            _vector[index] = &t;

            return true;
        }
    }

    void append(T &t)
    {
        _vector.append(&t);
    }

    void remove(int index)
    {
        delete _vector[index];

        _vector.remove(index);
    }

    int count()
    {
        return _vector.count();
    }

    void clear()
    {
        foreach(T *t, _vector)
            delete t;

        _vector.clear();
    }
};

#endif // ABSTRACTCONTAINER_H
