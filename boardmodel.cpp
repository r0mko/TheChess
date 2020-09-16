#include "boardmodel.h"
#include "piecesmodel.h"
#include <QDebug>

BoardModel::BoardModel(QObject *parent) :
    QAbstractListModel(parent),
    m_pieces(new PiecesModel(this))
{

}

void BoardModel::initFromFEN(QByteArray fen)
{
    auto i = fen.cbegin();
    int idx = 0;
    while (i != fen.cend()) {
        switch (*i) {
        case 'R':
        case 'r':
            m_pieces->addPiece(idx++, Piece::Rook, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case 'N':
        case 'n':
            m_pieces->addPiece(idx++, Piece::Knight, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case 'B':
        case 'b':
            m_pieces->addPiece(idx++, Piece::Bishop, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case 'P':
        case 'p':
            m_pieces->addPiece(idx++, Piece::Pawn, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case 'Q':
        case 'q':
            m_pieces->addPiece(idx++, Piece::Queen, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case 'K':
        case 'k':
            m_pieces->addPiece(idx++, Piece::King, isupper(*i) ? Piece::White : Piece::Black);
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            idx += *i - '0';
            break;
        case '/':
            break;

        default:
            break;
        }
        ++i;
    }
    m_pieces->setInitial(fen);

}

void BoardModel::makeMove(int sourceIndex, int destiantionIndex, QTime timestamp)
{
    m_pieces->makeMove(sourceIndex, destiantionIndex, timestamp.msecsSinceStartOfDay());
}

void BoardModel::appendHistoryState(QByteArray sourcePos, QByteArray destinationPos, int capturedIndex, QTime timestamp, int type)
{
    m_pieces->appendHistoryState(positionToIndex(sourcePos), positionToIndex(destinationPos), capturedIndex, timestamp.msecsSinceStartOfDay(), (Piece::PieceType)type);
}

void BoardModel::clear()
{
    m_board.clear();
    m_pieces->clear();
}

QVariant BoardModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int i = index.row();
    Piece *p = m_pieces->pieceAtSquare(i);
    int file = i % 8;
    int rank = 7 - (i >> 3);
    switch (role) {
    case SquareColorRole:
        return ((i + rank) % 2) == 1;
    case CellCoordRole:
        return indexToPos(i);
    case BoardIndexRole:
        return i;
    case PieceRole:
        return QVariant::fromValue(p);
    case FileRole:
        return file;
    case RankRole:
        return rank;
    case MoveRole:
        return m_board.contains(i) && (m_board[i] & PiecesModel::CanMove);
    case CaptureRole:
        return m_board.contains(i) && (m_board[i] & PiecesModel::CanCapture);
    default:
        break;
    }
    return QVariant();
}


QHash<int, QByteArray> BoardModel::roleNames() const
{
    QHash<int, QByteArray> r;
    r[SquareColorRole] = "isDark";
    r[CellCoordRole] = "coords";
    r[BoardIndexRole] = "boardIndex";
    r[PieceRole] = "piece";
    r[RankRole] = "rank";
    r[FileRole] = "file";
    r[MoveRole] = "move";
    r[CaptureRole] = "capture";
    return r;
}

void BoardModel::setActivePiece(int index)
{
    QModelIndex i1 = createIndex(0, 0);
    QModelIndex i2 = createIndex(63, 0);
    if (!m_pieces->contains(index)) {
        if (index >= 0)
            qWarning() << "piece" << index << indexToPos(index) << "not found";
        m_board.clear();
        emit dataChanged(i1, i2, { MoveRole, CaptureRole, PieceRole });
        return;
    }
    m_board = m_pieces->availableMoves(index);
    emit dataChanged(i1, i2, { MoveRole, CaptureRole, PieceRole });
}

void BoardModel::toggleTurn()
{
    m_turn ^= (Piece::Black ^ Piece::White);
    qDebug() << Q_FUNC_INFO << "turn" << m_turn;
    emit turnChanged();
}

void BoardModel::addPiece(QByteArray p)
{
    qDebug() << "Adding piece" << p;
    m_pieces->addPiece(p);

}

int BoardModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 64;
    return 0;
}

QByteArray indexToPos(int index)
{
    int file = index % 8;
    int rank = index >> 3;
    QByteArray ret;
    ret.append('a' + file);
    ret.append('8' - rank);
    return ret;
}


QPair<int, int> positionToCoords(const QByteArray &pos)
{
    if (pos.size() != 2)
        return qMakePair(-1, -1);
    QByteArray ba = pos.toLower();
    int column = ba[0] - 'a';
    int row = ba[1] - '1';
    if (qBound(0, column, 7) == column && qBound(0, row, 7) == row)
        return qMakePair(column, row);
    return qMakePair(-1, -1);
}

