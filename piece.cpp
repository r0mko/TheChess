#include "piece.h"
#include <QDebug>
#include "boardmodel.h"

Piece::Piece(bool init)
{
    if (init)
        d = new PieceData(Pawn, White, -1, nullptr);
}

Piece::Piece(Piece::PieceType type, QByteArray position) :
    Piece(type, White, positionToIndex(position))
{}

Piece::Piece(Piece::PieceType type, Piece::Color color, int index, BoardModel *board) :
    d(new PieceData(type, color, index, board))
{
    switch (type) {
    case Rook:
        setMoves("n+");
        break;
    case Knight:
        setMoves("~1/2");
        break;
    case Bishop:
        setMoves("nX");
        break;
    case Queen:
        setMoves("n*");
        break;
    case King:
        setMoves("1*");
        break;
    case Pawn:
        setMoves("o1>, c1X>, oi2>");
        break;
    default:
        break;
    }
}

void Piece::setMoves(QByteArray moves)
{
    for (QByteArray m : moves.split(',')) {
        d->capabilities.append(parseMove(m.trimmed()));
    }
}

QString Piece::sign() const
{
    int charCode = d->type >> 8;
    return QString(QChar(charCode + (d->color >> 8)));
}


void Piece::setType(char type)
{
    if (d->type == Undefined)
        d->type = type;
}

void Piece::setBoardIndex(int index)
{
    d->index = index;
}

Piece::MoveFlags Piece::parseMove(const QByteArray &move)
{
    Piece::MoveFlags ret = 0;
    auto it = move.cbegin();
    while (it != move.cend()) {
        switch (*it++) {
        case 'o':
            ret |= Move;
            break;
        case 'i':
            ret |= InitialMove;
            break;
        case 'c':
            ret |= Capture;
            break;
        case '~':
            ret |= Leaper;
            break;
        case '^':
            ret |= Locust;
            break;
        case '1':
            ret |= OneSquare;
            break;
        case '2':
            ret |= TwoSquares;
            break;
        case 'n':
            ret |= AnyDistance;
            break;
        case '*':
            ret |= Orthogonally | Diagonally | Sideways | Forwards | Backwards;
            break;
        case '+':
            ret |= Orthogonally | Sideways | Forwards | Backwards;
            break;
        case '>':
            ret |= Orthogonally | Forwards;
            break;
        case '<':
            ret |= Orthogonally | Backwards;
            break;
        case '=':
            ret |= Orthogonally | Sideways;
            break;
        case 'X': {
            if (it == move.cend())
                ret |= Diagonally | Forwards | Backwards;
            else switch (*it++) {
            case '>':
                ret |= Diagonally | Forwards;
                break;
            case '<':
                ret |= Diagonally | Backwards;
                break;
            default:
                break;
            }
            break;
        }
        case '/':
            ret |= OrthogonalPair;
            break;
        case '&':
            ret |= RepeatedMove;
            break;
        default:
            qWarning() << "Unrecognized token" << *(it - 1);
            return 0;
            break;
        }
    }
    return ret;
}

QByteArray Piece::position() const
{
    QByteArray ret;
    ret.append('a' + file());
    ret.append('1' + rank());
    return ret;
}

void Piece::setPosition(QByteArray pos)
{
    d->index = positionToIndex(pos);
}

void Piece::setActive(bool active) {
    d->active = active;
    if (active)
        d->board->setActivePiece(d->index);
    else
        d->board->setActivePiece(-1);
}

bool Piece::inGame() const
{
    return d->inGame;
}

void Piece::setInGame(bool inGame)
{
    d->inGame = inGame;
}

QByteArray Piece::toByteArray() const
{
    QByteArray ret;

    ret += d->type;
    ret += d->color;

    if (d->inGame)
        ret += "+";
    else
        ret += "-";
    ret += position();
    return ret;
}

int positionToIndex(QByteArray pos)
{
    if (pos.size() != 2)
        return -1;
    QByteArray ba = pos.toLower();
    int file = ba[0] - 97;
    int rank = 56 - ba[1];
    if (qBound(0, file, 7) == file && qBound(0, rank, 7) == rank)
        return coordsToIndex(file, rank);
    return -1;
}
