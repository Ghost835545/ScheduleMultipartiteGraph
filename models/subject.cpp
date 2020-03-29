#include "subject.h"

Subject::Subject(QString name)
{
    this->name = name;
}

QString Subject::getClassName() {
    return QString("Subject");
}

QJsonObject Subject::toJson() {
    QJsonObject items;
    items.insert("id", QString("%1").arg(this->id));
    items.insert("name", QString("%1").arg(this->name));
    return items;
}

void Subject::fromJson(QJsonObject items) {
    this->id = items.value("id").toInt();
    this->name = items.value("name").toInt();
}
