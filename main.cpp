#include <QApplication>
#include <QElapsedTimer>
#include <QQmlApplicationEngine>
#include "qqml.h"
#include "boardmodel.h"
#include "elapsedtimer.h"
template<class... Args> QVector<int> qRegisterMetaTypes() { return { qRegisterMetaType<Args>()... }; }
template<class T> bool registerEnum() {
    return QMetaType::registerConverter<T, int>();
}

template<class... Args> QVector<bool> registerEnums() { return { registerEnum<Args>()... }; }

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // @uri QmlChess
    const char *uri = "QmlChess";
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<Piece>(uri, 1, 0, "Piece", "Piece is not creatable");
    registerEnums<Piece::Color,
            Piece::PieceType>();
    qmlRegisterType<BoardModel>(uri, 1, 0, "BoardModel");
    qmlRegisterType<PiecesModel>(uri, 1, 0, "PiecesModel");
    qmlRegisterType<ElapsedTimer>(uri, 1, 0, "ElapsedTimer");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
