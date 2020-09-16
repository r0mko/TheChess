#ifndef PIECESMODEL_H
#define PIECESMODEL_H

#include "piece.h"

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>
#include <QStack>
#include <QUrl>

class BoardModel;

class PiecesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int logSize READ logSize NOTIFY logSizeChanged)
    Q_PROPERTY(int currentMove READ currentMove NOTIFY currentMoveChanged)

    Q_PROPERTY(QJsonArray history READ history NOTIFY historyChanged)
    Q_PROPERTY(QJsonObject game READ game WRITE setGame NOTIFY gameChanged)

    QVector<Piece*>     m_pieces;
    BoardModel          *m_model;
    QPointer<Piece>     layout[64];
    QJsonObject         m_game;

    QJsonArray          m_history;
    QJsonArray::const_iterator m_currentMove;

    QList<Piece::MoveFlags> extractDirections(Piece::MoveFlags flags) const;

    void revertMove(QJsonObject m);
    void replayMove(QJsonObject m);

    QJsonArray piecesToJson() const;


public:
    enum Role {
        RankRole = Qt::UserRole + 1,
        FileRole,
        RowRole,
        ColumnRole,
        PosRole,
        BoardIndexRole,
        SymbolRole,
        InGameRole,
        PieceRole,
        ColorRole
    };

    enum SquareStatus {
        CanMove = 1,
        CanCapture = 2,
        EnPassant = 3
    };

    PiecesModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    Piece *pieceAtSquare(int index) const;
    Piece *pieceAt(int index) const;
    int indexOfPiece(int pieceIndex) const;
    int indexOfPiece(Piece* piece) const;

    bool contains(int index) const;
    QHash<int, QByteArray> roleNames() const;

    QHash<int, SquareStatus> availableMoves(int pieceIndex) const;

    class iterator
    {
        const PiecesModel *p;
    public:
        int i;
        iterator(const PiecesModel *model, int index = -1) : p(model), i(index) {}
        inline bool isValid() const { return qBound(0, i, 63) == i; }
        bool move(Piece::MoveFlags dir, Piece::Color color = Piece::White);

        inline int row() const { return i >> 3; }
        inline int column() const { return i & 7; }

        inline int rank() const { return 7 - row(); }
        inline int file() const { return column(); }
        inline Piece *piece() const { return p->pieceAtSquare(i); }
    };


    inline iterator begin(int pos) const { return iterator(this, pos); }
    inline int count() const { return m_pieces.size(); }
    inline int logSize() const { return m_history.size(); }
    QJsonArray history() const;

    void appendHistoryState(int sourcePos, int destinationPos, int capturedIndex, qint64 timestamp, Piece::PieceType type);
    void setInitial(const QByteArray &fenSetup);

    inline QJsonObject game() const { return m_game; }
    void setGame(QJsonObject game);

    int currentMove() const;

public slots:
    bool addPiece(const QByteArray &p);
    bool addPiece(int index, Piece::PieceType type, Piece::Color color, bool status = true);
    void makeMove(int sourceIndex, int destinationIndex, qint64 timestamp);

    void undoLastMove();
    void exportGame(const QUrl &filename) const;
    void importGame(const QUrl &filename);

    void toNextMove();
    void toPrevMove();
    void toFirstMove();
    void toLastMove();

    void clear();

signals:
    void countChanged();
    void logSizeChanged();
    void historyChanged();
    void gameChanged();
    void gameLoaded();
    void currentMoveChanged();
};

#endif // PIECESMODEL_H
