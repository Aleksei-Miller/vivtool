#include "viv.h"

#define DEBUG 0

Viv::Viv(const QString &path) : _path(path)
{
    read(path);
}

Viv::~Viv()
{
}

bool Viv::read(const QString &path)
{
    clear();

    if(path.isEmpty())
        return false;

    //Read file
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    //Parse
    //
    QDataStream data_stream(&file);
    data_stream.setByteOrder(QDataStream::BigEndian);

    //Read id
    QByteArray id = data_stream.device()->read(4);
    if (!id.contains("BIG"))
    {
        file.close();

        return false;
    }

    //Seek archive size
    data_stream.device()->seek(data_stream.device()->pos() + 4);

    //Read number of files
    quint32 num_of_files = 0;
    data_stream >> num_of_files;

    //Seek first file offset
    data_stream.device()->seek(data_stream.device()->pos() + 4);

    for (quint32 i = 0; i < num_of_files; i++)
    {
        //Read offset
        quint32 file_offset = 0;
        data_stream >> file_offset;

        //Read size
        quint32 file_size = 0;
        data_stream >> file_size;

        //Check
        if (file_offset > static_cast<quint32>(file.size()) ||
            file_offset + file_size > static_cast<quint32>(file.size()))
        {
            file.close();

            return false;
        }

        //Read file name
        QByteArray file_name;
        const quint8 file_name_max_size = 255;

        for (int i = 0; i < file_name_max_size; i++)
        {
            char c;

            data_stream.device()->read(&c, 1);

            if (c == '\0')
                break;

            file_name += c;
        }

        //Get current offset
        qint64 offset = data_stream.device()->pos();

        //
        data_stream.device()->seek(file_offset);

        //Create chunk
        Chunk *chunk = new Chunk;

        chunk->name = file_name;

        chunk->data = data_stream.device()->read(file_size);

        append(*chunk);

        //Return to old offset
        data_stream.device()->seek(offset);
    }

    file.close();

    return true;
}

bool Viv::write(const QString &path)
{
    if (path.isEmpty())
        return false;

    //Get archive size and first file offset
    // + header = 16
    quint32 archive_size = 16, first_file_offset = 16;

    for (int i = 0; i < count(); i++)
    {
        Chunk *chunk = at(i);

        //name + offset(4) + size(4) + name null byte(1)
        first_file_offset += chunk->name.length() + 8 + 1;
        
        //name + data + offset(4) + size(4) + name null byte(1)
        archive_size += chunk->name.length() + chunk->data.length() + 8 + 1;
    }

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QDataStream data_stream(&file);
    data_stream.setByteOrder(QDataStream::BigEndian);

    //ID
    data_stream.device()->write("BIGF");

    //Write archive size
    data_stream << archive_size;

    //Write number of files
    data_stream << static_cast<quint32>(count());

    //Write first file offset
    data_stream << first_file_offset;

    quint32 file_offset = first_file_offset;
    char c = '\0';

    for (int i = 0; i < count(); i++)
    {
        Chunk *chunk = at(i);

        //Write offset
        data_stream << file_offset;

        //Write size
        data_stream << static_cast<quint32>(chunk->data.size());

        //Write name
        data_stream.device()->write(chunk->name.toStdString().c_str());

        //Write '\0'
        data_stream.device()->write(&c, 1);

        //Get current offset
        qint64 offset = data_stream.device()->pos();

        //
        data_stream.device()->seek(file_offset);

        //Write data
        data_stream.device()->write(chunk->data);

        //Return to old offset
        data_stream.device()->seek(offset);

        //Get next offset
        file_offset += chunk->data.length();
    }

    file.close();

    return true;
}


