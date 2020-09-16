#ifndef BOARD_H
#define BOARD_H
#include <QAbstractListModel>
#include <QObject>
#include <QTime>
#include <QVector>
#include "piece.h"
#include "qqml.h"
#include "piecesmodel.h"

class PiecesModel;

class BoardModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(PiecesModel* pieces READ pieces CONSTANT)
    Q_PROPERTY(int turn READ turn NOTIFY turnChanged)

public:
    enum BoardRole {
        SquareColorRole = Qt::UserRole + 1,
        CellCoordRole,
        BoardIndexRole,
        PieceRole,
        RankRole,
        FileRole,
        MoveRole,
        CaptureRole
    };
//    Q_ENUM(BoardRole)

    BoardModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void setActivePiece(int index);
    void toggleTurn();

    inline PiecesModel* pieces() const { return m_pieces; }
    inline int turn() const { return m_turn; }

public slots:
    void addPiece(QByteArray p);
    void initFromFEN(QByteArray fen);
    void makeMove(int sourceIndex, int destiantionIndex, QTime timestamp);
    void appendHistoryState(QByteArray sourcePos, QByteArray destinationPos, int capturedIndex, QTime timestamp, int type);
    void clear();

signals:
    void turnChanged();

private:
    PiecesModel* m_pieces;
    QHash<int, PiecesModel::SquareStatus> m_board;
    int m_turn = Piece::White;
};



#endif // BOARD_H
