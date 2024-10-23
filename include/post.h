#ifndef POST_H
#define POST_H

#include <QString>
#include <QList>
#include "comment.h"

struct Post {
    int postNumber;
    QString subject;
    QString writer;
    QString description;
    QList<Comment> comments;
};

#endif // POST_H