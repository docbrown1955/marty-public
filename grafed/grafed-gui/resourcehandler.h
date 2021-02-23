#ifndef RESSOURCEHANDLER_H
#define RESSOURCEHANDLER_H

#include <iostream>
#include <functional>
#include <QFuture>
#include <QFutureWatcher>
#include <QMap>
#include <QString>
#include <QObject>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>
#include <cassert>

class AutoMutex {
public:
    AutoMutex()  { mut.lock();   }
    ~AutoMutex() { mut.unlock(); }
private:
    QMutex mut;
};


class ResourceHandler: public QObject {

    Q_OBJECT

public:
    using Key = std::string;
    using Value = QPixmap;

    using future_iterator
        = typename QMap<Key, QFuture<Value>>::iterator;
    using future_const_iterator
        = typename QMap<Key, QFuture<Value>>::const_iterator;

    using iterator
        = typename QMap<Key, Value>::iterator;
    using const_iterator
        = typename QMap<Key, Value>::const_iterator;

public:
    ResourceHandler(QObject *_parent)
        :QObject(_parent)
    {}

    ~ResourceHandler() override {}

    ResourceHandler(std::function<Value(Key const&)> const &t_builder)
    {
        setBuilder(t_builder);
    }

    void setBuilder(std::function<Value(Key const&)> const &t_builder)
    {
        builder = t_builder;
    }

    Value buildInMainThread(Key const &key) {
        AutoMutex m;
        if (auto pos = finalMap.find(key); pos != finalMap.end())
            return pos.value();
        finalMap[key] = builder(key);
        return finalMap[key];
    }

    bool isReady() const {
        AutoMutex M;
        return futureMap.empty();
    }

    iterator find(Key const &key) {
        auto res = finalMap.find(key);
        return res;
    }

    const_iterator find(Key const &key) const {
        auto res = finalMap.find(key);
        return res;
    }

    future_iterator isFuture(Key const &key) {
        AutoMutex M;
        auto res = futureMap.find(key);
        return res;
    }

    future_const_iterator isFuture(Key const &key) const {
        AutoMutex M;
        auto res = futureMap.find(key);
        return res;
    }

    void sendKey(Key const &key) {
        if (find(key) != finalMap.end())
            return;
        AutoMutex m;
        if (isFuture(key) != futureMap.end())
            return;
        assert(builder);
        QFutureWatcher<Value> *watcher = new QFutureWatcher<Value>;
        futureMap[key] = QtConcurrent::run(builder, key);
        auto endTask = [=]() { emit this->valueReady(key); };
        connect(
             watcher,
             &QFutureWatcher<Value>::finished,
             endTask
        );
        watcher->setFuture(futureMap[key]);
    }

signals:

    void valueReady(const std::string&);

private:

    std::function<Value(Key const&)> builder;
    QMap<Key, QFuture<Value>>        futureMap;
    QMap<Key, Value>                 finalMap;
};


#endif // RESSOURCEHANDLER_H
