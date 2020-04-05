#ifndef REPOSITORYGENERAL_H
#define REPOSITORYGENERAL_H

#include <QDebug>
#include <QList>
#include "../abstractnoderepository.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QFile>
#include <QDir>
#include <ctime>

template <class T>
class RepositoryGeneral
{
private:
    // Список всех нод.
    QList<T> elements;

    // Порядковый номер ноды (идентификатор ноды).
    int increment = -1;

    // Наименование класса, с которым работает репозиторий.
    // Нужен для именования файлов и папок, в которых храняться соотвествующие данные.
    QString tname;

    // Наименование папки, в которой храняться данные.
    QString dirStorager = "storage";

    // Наименование папки, в которой храняться данные по данном виду данных.
    QString dir;

    // Инициализация репозитория.
    void init();

    // Проверка ноды.
    void checkNode();

    // Инициализация внутренних переменных.
    void initEnvironment();

    // Инициализация хранилища.
    void initStorage();

    // Получение итератора по идентификатору.
    class QList<T>::iterator getIteratorById(int id);

public:
    RepositoryGeneral();
    ~RepositoryGeneral();

    // Добавить объект.
    int add(T data);

    // Удалить объект.
    bool remove(int id);

    // Обновить объект.
    bool update(int id, T data);

    // Получить объект.
    T getById(int id);

    // Получить объекты по параметрам.
    QList<T> getByParameters(T searchObject);

    // Получить все объекты.
    QList<T> getAll();

    // Начало итератора
    class QList<T>::iterator begin();

    // Конец итератора
    class QList<T>::iterator end();

    // Сохранить все объекты на диск.
    void save();

    // Загрузить всё с диска.
    void load();
};

// Начало инициализации

template <class T>
RepositoryGeneral<T>::RepositoryGeneral() {
    this->init();
}

template <class T>
void RepositoryGeneral<T>::init() {
    this->checkNode();
    this->initEnvironment();
    this->initStorage();
    this->load();
}

template <class T>
void RepositoryGeneral<T>::checkNode() {
    // Проверка на наследование ноды от abstractNodeRepository
    abstractNodeRepository* test = dynamic_cast<abstractNodeRepository*> (new T);
    if (!test) {
        throw std::runtime_error("The type of template does not satisfy the required. The type must be a descendant of the abstractNodeRepository class.");
    }
}

template <class T>
void RepositoryGeneral<T>::initEnvironment() {
    this->tname = T().getClassName();
    this->dir = QString("%1/%2").arg(this->dirStorager).arg(this->tname);
}

template <class T>
void RepositoryGeneral<T>::initStorage() {
    QDir dirStorage(this->dirStorager);
    QDir dir(this->dir);

    bool ok = dirStorage.exists();
    if (!ok) {
        QDir().mkdir(this->dirStorager);
    }
    ok = dir.exists();
    if (!ok) {
        QDir().mkdir(this->dir);
    }
}

template <class T>
void RepositoryGeneral<T>::load() {
    // Достаем список файлов для сущности, с которой работает репозиторий
    QDir dir(this->dir);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();

    // Если файлы существуют, то считываем их
    if (!list.empty()){
        // Берем файл последний файл
        QString jsonName = QString("%1/%2").arg(this->dir).arg(list.at(list.size()-1).fileName());

        // Десериализуем
        QFile jsonFile(jsonName);
        jsonFile.open(QFile::ReadOnly);
        QJsonDocument json = QJsonDocument().fromJson(jsonFile.readAll());
        QJsonObject object = json.object();
        QJsonArray data = object["data"].toArray();
        QJsonObject metadata = object["metadata"].toObject();

        // Считываемпредыдущее положение счетчика
        this->increment = metadata["increment"].toInt();

        // Добавлем в репозиторий данные
        for (auto element : data) {
            T t;
            t.fromJson(element.toObject());
            this->elements.append(t);
        }
    }
}

// Конец инициализации

template <class T>
class QList<T>::iterator RepositoryGeneral<T>::getIteratorById(int id) {
    class QList<T>::iterator it = std::find_if(this->elements.begin(), this->elements.end(), [id](T element){ return element.id == id; });
    return it;
}

template <class T>
int RepositoryGeneral<T>::add(T data) {
    this->increment++;
    data.id = this->increment;
    this->elements.append(data);
    return this->increment;
}

template <class T>
bool RepositoryGeneral<T>::remove(int id) {
    class QList<T>::iterator it = this->getIteratorById(id);
    if (it != this->elements.end()){
        this->elements.erase(it);
        return true;
    }
    return false;
}

template <class T>
bool RepositoryGeneral<T>::update(int id, T data) {
    class QList<T>::iterator it = this->getIteratorById(id);
    if (it != this->elements.end()){
        data.id = id;
        this->elements.replace(std::distance(this->elements.begin(), it), data);
        return true;
    }
    return false;
}

template <class T>
T RepositoryGeneral<T>::getById(int id) {
    class QList<T>::iterator it = this->getIteratorById(id);
    if(it !=this->elements.end()) {
        return *it;
    }
    return T();
}

template <class T>
QList<T> RepositoryGeneral<T>::getByParameters(T searchObject) {
    QList<T> elements;
    for (T element : this->elements) {
        if (element == searchObject) {
            elements.append(element);
        }
    }
    return elements;
}

template <class T>
QList<T> RepositoryGeneral<T>::getAll() {
    return this->elements;
}

template <class T>
class QList<T>::iterator RepositoryGeneral<T>::begin()
{
    return this->elements.begin();
}

template <class T>
class QList<T>::iterator RepositoryGeneral<T>::end()
{
    return this->elements.end();
}

template <class T>
void RepositoryGeneral<T>::save() {
    QJsonDocument json;

    QJsonArray data;
    QJsonObject metadata;
    QJsonObject object;

    object = json.object();

    for (T element : this->elements) {
        data.append(element.toJson());
    }
    metadata["increment"] = this->increment;

    object["data"] = data;
    object["metadata"] = metadata;
    json.setObject(object);

    QString jsonName = QString("storage/%1/%2_%3.json").arg(this->tname).arg(this->tname).arg(time(NULL));
    QFile jsonFine(jsonName);
    jsonFine.open(QFile::WriteOnly);
    jsonFine.write(json.toJson());
}

template <class T>
RepositoryGeneral<T>::~RepositoryGeneral() {
//    this->save();
}
#endif // REPOSITORYGENERAL_H