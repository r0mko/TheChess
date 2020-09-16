#ifndef PIECE_H
#define PIECE_H

#include <QObject>
#include <QMetaEnum>
#include <QSharedData>
//#include <cxxabi.h>
#include <QDebug>

#define PRINTABLE_ENUM(CLASS, ENUM)                                            \
    template <>                                                                    \
    QByteArray printableEnum< ## CLASS :: ## ENUM>(const CLASS::ENUM &enumType) {  \
    int eid = CLASS::staticMetaObject.indexOfEnumerator(#ENUM);                \
    QMetaEnum e = CLASS::staticMetaObject.enumerator(eid);                     \
    return QByteArray(e.valueToKey(enumType));                                 \
    }

class MyClass
{
    int m_a = 0;
    int m_b = 0;
public:
    MyClass() = default;
    MyClass(int a, int b) :
        m_a(a),
        m_b(b)
    {}
    void setA(int a) { m_a = a; }
    void setB(int b) { m_b = b; }
    int a() const { return m_a; }
    int b() const { return m_b; }
};

template <class T>
QByteArray printableFlags(const QFlags<T> &flags)
{
    //    QMetaEnum f = QMetaEnum::fromType<QFlags<T>>();
    //    return f.valueToKeys(flags.operator int()).constData();
    //    QByteArray tn(typeid(flags).name());

    int r;
//    qDebug() << abi::__cxa_demangle(typeid(flags).name(), 0, 0, &r);
    return QVariant::fromValue<QFlags<T>>(flags).toString().toLocal8Bit();
}
template <class T>
QByteArray printableEnum(const T &enumType)
{
    //    QMetaEnum f = QMetaEnum::fromType<T>();
    //    return f.valueToKey(enumType);
    int r;
//    qDebug() << abi::__cxa_demangle(typeid(enumType).name(), 0, 0, &r);
    return QVariant::fromValue<T>(enumType).toString().toLocal8Bit();
}



QByteArray indexToPos(int index);
int positionToIndex(QByteArray pos);
QPair<int, int> positionToCoords(const QByteArray &pos);
inline int coordsToIndex(int column, int row) { return (row << 3) + column; }

class BoardModel;


class Piece : public QObject
{
    Q_OBJECT

    Q_ENUMS(PieceType)
    Q_ENUMS(Color)
    Q_ENUMS(Status)
    Q_PROPERTY(QByteArray moves READ moves WRITE setMoves) // write only
    Q_PROPERTY(QString sign READ sign CONSTANT)
    Q_PROPERTY(PieceType type READ type WRITE setType) // write once
    Q_PROPERTY(bool inGame READ inGame NOTIFY inGameChanged)

    Q_PROPERTY(int rank READ rank)
    Q_PROPERTY(int file READ file)
    Q_PROPERTY(int boardIndex READ boardIndex)

    Q_PROPERTY(QByteArray position READ position WRITE setPosition)
    Q_PROPERTY(Color color READ color)
    Q_PROPERTY(bool active READ active WRITE setActive)
    Q_PROPERTY(int moveCount READ moveCount)
    Q_PROPERTY(bool isNull READ isNull)

    friend class PiecesModel;

public:

    enum MoveFlag {
        // distance
        OneSquare       = 0x1,
        TwoSquares      = 0x2,
        AnyDistance     = 0x4 | OneSquare | TwoSquares,
        // direction
        Diagonally      = 0x10,
        Orthogonally    = 0x20,
        Forwards        = 0x40,
        Rightward       = 0x80,
        Backwards       = 0x100,
        Leftward        = 0x200,
        Sideways        = Leftward | Rightward,
        SidesMask       = Sideways | Forwards | Backwards,
        // move type
        Leaper          = 0x400,
        Locust          = Leaper | 0x800,
        // condition
        InitialMove     = 0x1000,
        Capture         = 0x2000,
        Move            = 0x4000,
        // grouping
        OrthogonalPair  = 0x10000,
        RepeatedMove    = 0x20000,
    };
    Q_DECLARE_FLAGS(MoveFlags, MoveFlag)
    //    Q_FLAG(MoveFlags)

    enum PieceType {
        Undefined   = ' ',
        King        = 'k',
        Queen       = 'q',
        Rook        = 'r',
        Bishop      = 'b',
        Knight      = 'n',
        Pawn        = 'p',

    };
    //    Q_ENUM(PieceType)

    enum Color {
        Black = 'b',
        White = 'w',
    };
    //    Q_ENUM(Color)

    explicit Piece(bool init = false);
    Piece(PieceType type, QByteArray position);
    Piece(PieceType type, Color color, int index, BoardModel *board = nullptr);

    inline bool operator ==(const Piece &other) const { return d == other.d; }
    inline bool operator !=(const Piece &other) const { return d != other.d; }

    inline bool isEnemy(const Piece *other) const { if (!other) return false; return d->color != other->d->color; }

    inline QByteArray moves() const { return QByteArray(); }
    void setMoves(QByteArray moves);

    QString sign() const;

    inline PieceType type() const { return (PieceType)d->type; }
    void setType(char type);

    inline QList<MoveFlags> capabilities() const { return d->capabilities; }

    inline void incMove() { ++d->move; }

    inline int boardIndex() const { return d->index; }
    void setBoardIndex(int index);

    inline int row() const { return d->index >> 3; }
    inline int column() const { return d->index & 7; }

    inline int rank() const { return 7 - (d->index >> 3); }
    inline int file() const { return column(); }

    QByteArray position() const;
    void setPosition(QByteArray pos);

    inline Color color() const { return (Color)d->color; }

    inline bool active() const { return d->active; }
    void setActive(bool active);

    inline int moveCount() const { return d->move; }

    bool inGame() const;
    void setInGame(bool inGame);

    QByteArray toByteArray() const;

    inline bool isNull() const { return d.operator bool(); }
    inline operator bool() const { return isNull(); }

signals:
    void inGameChanged();

private:
    class PieceData : public QSharedData
    {
    public:
        PieceData(Piece::PieceType type, Piece::Color color, int index, BoardModel *board) :
            type(type),
            color(color),
            index(index),
            board(board)
        {}

        char                type = Undefined;
        char                color = White;
        char                index;
        int                 move = 0;
        bool                active = false;
        QList<MoveFlags>    capabilities;
        bool                inGame = true;
        BoardModel         *board = nullptr;
    };
    Piece(PieceData *data) : d(data) {}

    QExplicitlySharedDataPointer<PieceData> d;

    MoveFlags parseMove(const QByteArray &moveCount);
    bool m_inGame;
};

Q_DECLARE_METATYPE(Piece*)
Q_DECLARE_METATYPE(Piece::PieceType)
Q_DECLARE_METATYPE(Piece::Color)
Q_DECLARE_OPERATORS_FOR_FLAGS(Piece::MoveFlags)


#endif // PIECE_H
