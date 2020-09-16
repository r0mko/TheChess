#include "piecesmodel.h"
#include "boardmodel.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlFile>

template <>
QByteArray printableEnum(const Piece::PieceType &enumType)
{
    int eid = Piece::staticMetaObject.indexOfEnumerator("PieceType");
    QMetaEnum e = Piece::staticMetaObject.enumerator(eid);
    return QByteArray(e.valueToKey(enumType));
}

//template <>
//QByteArray printableEnum(const Piece::PieceType &enumType)
//{
//    int eid = Piece::staticMetaObject.indexOfEnumerator("PieceType");
//    QMetaEnum e = Piece::staticMetaObject.enumerator(eid);
//    return QByteArray(e.valueToKey(enumType));
//}

PiecesModel::PiecesModel(QObject *parent) :
    QAbstractListModel(parent),
    m_model(qobject_cast<BoardModel*>(parent))
{
    clear();
}

int PiecesModel::rowCount(const QModelIndex &parent) const
{
    return m_pieces.size();
}

QVariant PiecesModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "data query" << index<< role;
    int i = index.row();

    if (i >= m_pieces.size())
        return QVariant();
    Piece *p = m_pieces.at(i);

    switch (role) {
    case RankRole:
        return p->rank();
    case BoardIndexRole:
        return p->boardIndex();
    case FileRole:
        return p->file();
    case RowRole:
        return p->row();
    case ColumnRole:
        return p->column();
    case PosRole:
        return p->position();
    case ColorRole:
        return p->color();
    case PieceRole:
        return QVariant::fromValue(p);
    case SymbolRole: {
        QByteArray iconName;
        iconName += (char)p->type();
        iconName += (char)p->color();
        return iconName;
    }
    case InGameRole:
        return (int)p->inGame();
    default:
        break;
    }
    return QVariant();
}

Piece *PiecesModel::pieceAtSquare(int index) const
{
    if (qBound(0, index, 63) != index)
        return nullptr;
    return layout[index].data();
}


Piece *PiecesModel::pieceAt(int index) const
{
    return m_pieces.value(index, nullptr);
}

int PiecesModel::indexOfPiece(int pieceIndex) const
{
    return indexOfPiece(pieceAtSquare(pieceIndex));
}

int PiecesModel::indexOfPiece(Piece *piece) const
{
    if (!piece)
        return -1;
    return m_pieces.indexOf(piece);
}

bool PiecesModel::contains(int index) const
{
    if (index > 63 || index < 0)
        return false;
    return layout[index];
}

QHash<int, QByteArray> PiecesModel::roleNames() const
{
    QHash<int, QByteArray> r;
    r[RankRole] = "rank";
    r[FileRole] = "file";
    r[RowRole] = "row";
    r[ColumnRole] = "column";
    r[PosRole] = "position";
    r[SymbolRole] = "iconSrc";
    r[InGameRole] = "inGame";
    r[PieceRole] = "piece";
    r[ColorRole] = "color";
    r[BoardIndexRole] = "boardIndex";
    return r;
}

QHash<int, PiecesModel::SquareStatus> PiecesModel::availableMoves(int pieceIndex) const
{
    QHash<int, PiecesModel::SquareStatus> r;
    if (pieceIndex < 0)
        return r;
    iterator it = begin(pieceIndex);
    Piece *src = it.piece();
    if (!it.piece())
        return r;
    for (Piece::MoveFlags cap : src->capabilities()) {
        if ((cap & Piece::InitialMove) && it.piece()->moveCount() > 0)
            continue;
        QList<Piece::MoveFlags> dirs = extractDirections(cap);
        for (Piece::MoveFlags f : dirs) {
            iterator i(this, pieceIndex);
            int s = cap & Piece::AnyDistance;
            if (cap & Piece::Leaper)
                s = 1;
            while (s--) {
                if (!i.move(f, src->color()))
                    break;
                if (it.piece()->isEnemy(i.piece()) && !(cap & Piece::Move)) {
                    r[i.i] = CanCapture;
                    break;
                } else if (i.piece()) {
                        break;
                } else if (cap & Piece::Capture)
                    break;
                r[i.i] = CanMove;
                // TODO: en passant
            }
        }
    }
    return r;
}

QJsonArray PiecesModel::history() const
{
    return m_history;
}

void PiecesModel::appendHistoryState(int sourceIndex, int destinationIndex, int capturedIndex, qint64 timestamp, Piece::PieceType type)
{
    qDebug() << "Appending state" << sourceIndex << destinationIndex << indexToPos(sourceIndex) << indexToPos(destinationIndex) << type;
    QJsonObject m;
    m["source"] = QString(indexToPos(sourceIndex));
    m["sign"] = QString(QChar((char)type));
    m["destination"] = QString(indexToPos(destinationIndex));
    if (capturedIndex >= 0)
        m["captured"] = capturedIndex;
    m["timestamp"] = timestamp;
    m_history.append(m);
    emit historyChanged();
    emit logSizeChanged();
}

bool PiecesModel::addPiece(const QByteArray &p)
{
    if (p.size() < 5)
        return false;
    auto i = p.cbegin();
    char pType = *i++;
    char color = *i++;
    bool inGame = (*i++ == '+');
    int pos = positionToIndex(QByteArray(i, 2));
    return addPiece(pos, (Piece::PieceType)pType, (Piece::Color)color, inGame);
}

bool PiecesModel::addPiece(int index, Piece::PieceType type, Piece::Color color, bool inGame)
{
//    qDebug() << "Adding piece at idx" << index << printableEnum(type) << printableEnum(color) << (status == Piece::InGame);
    if (contains(index) && inGame) {
        qWarning() << "piece at index" << index << "already exists!";
        return false;
    }
    int row = m_pieces.size();
    beginInsertRows(QModelIndex(), row, row);
    Piece *p = new Piece(type, color, index, m_model);
    p->setInGame(inGame);
    if (inGame)
        layout[index] = p;
    m_pieces.append(p);
    endInsertRows();
    emit countChanged();
    return true;

}

void PiecesModel::makeMove(int sourceIndex, int destinationIndex, qint64 timestamp)
{
    if (!contains(sourceIndex)) {
        qWarning() << "No piece at position" << sourceIndex << indexToPos(sourceIndex);
        return;
    }
    Piece *pSrc = pieceAtSquare(sourceIndex);
    Piece *pDst = pieceAtSquare(destinationIndex);
    int dstIndex = -1;
    if (pDst) {
        qDebug() << "capture piece" << pDst->type() << pDst->position() << pDst->color();
        dstIndex = indexOfPiece(pDst);
        pDst->setInGame(false);
        QModelIndex idx = createIndex(dstIndex, 0);
        emit dataChanged(idx, idx, { InGameRole });
        layout[destinationIndex] = nullptr;
    }
    pSrc->setBoardIndex(destinationIndex);
    pSrc->incMove();
    int srcIndex = indexOfPiece(pSrc);
    qSwap(layout[destinationIndex], layout[sourceIndex]);
    QModelIndex idx = createIndex(srcIndex, 0);
    appendHistoryState(sourceIndex, destinationIndex, dstIndex, timestamp, pSrc->type());
    emit dataChanged(idx, idx, { BoardIndexRole, RowRole, ColorRole, FileRole, RankRole, PosRole });
    m_model->toggleTurn();
}

void PiecesModel::undoLastMove()
{
    if (m_history.isEmpty())
        return;

    QJsonObject move = m_history.last().toObject();
    revertMove(move);
    m_history.removeLast();
    emit historyChanged();
    emit logSizeChanged();
}

void PiecesModel::exportGame(const QUrl &filename) const
{
    QString file = QQmlFile::urlToLocalFileOrQrc(filename);
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create file" << file;
        return;
    }
    QJsonObject o;
    o["date"] = QJsonValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    o["version"] = "1";
    o["pieces"] = piecesToJson();
    o["moves"] = m_history;
    o["game"] = m_game;
    QJsonDocument doc;
    doc.setObject(o);
    f.write(doc.toJson());
    f.flush();
    f.close();
}

void PiecesModel::importGame(const QUrl &filename)
{
    m_model->clear();
    QString file = QQmlFile::urlToLocalFileOrQrc(filename);
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << Q_FUNC_INFO << "Failed to open file" << file;
        return;
    }
    QByteArray ba = f.readAll();
    f.close();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(ba, &error);
    if (error.error) {
        qWarning() << Q_FUNC_INFO << file << "is not valid JSON";
        return;
    }
    if (!doc.isObject()) {
        qWarning() << Q_FUNC_INFO << file << "contains no JSON object";
        return;
    }
    QJsonObject o = doc.object();
    QJsonValueRef v_pcs = o["pieces"];
    QJsonValueRef v_moves = o["moves"];
    if (!v_pcs.isArray() || !v_moves.isArray()) {
        qWarning() << Q_FUNC_INFO << file << "is not valid Chess JSON";
        return;
    }
    m_history = v_moves.toArray();

    m_currentMove = m_history.constEnd();

    m_game = o;
    for (QJsonValue v : v_pcs.toArray()) {
        m_model->addPiece(v.toString().toLatin1());
    }

    emit historyChanged();
    emit logSizeChanged();
    emit gameLoaded();
//    QJsonArray pcs = v_pcs.toArray();
//    for (QJsonValue v : pcs) {
//        QJsonObject p = v.toObject();
//        if (!p.contains("position") || !p.contains("inGame") || !p.contains("color") || !p.contains("type")) {
//            qWarning() << Q_FUNC_INFO << file << "contains corrupted Chess JSON";
//            return;
//        }
//        QString pos = o["position"].toString();
//    }


}

void PiecesModel::toNextMove()
{
    if (m_currentMove != m_history.constEnd()) {
        replayMove((*m_currentMove).toObject());
        m_currentMove++;
        emit currentMoveChanged();
    }
}

void PiecesModel::toPrevMove()
{
    if (m_currentMove == m_history.constBegin())
        return;
    --m_currentMove;
    revertMove((*m_currentMove).toObject());
    emit currentMoveChanged();
}

void PiecesModel::toFirstMove()
{
    while (m_currentMove != m_history.constBegin())
        toPrevMove();
}

void PiecesModel::toLastMove()
{
    while (m_currentMove != m_history.constEnd())
        toNextMove();
}

void PiecesModel::clear()
{
    beginResetModel();
    qDeleteAll(m_pieces);
    m_pieces.clear();
    memset(layout, 0, 64 * sizeof(void*));
    m_history = QJsonArray();
    m_game["history"] = m_history;
    m_currentMove = m_history.constEnd();
    endResetModel();
}

void PiecesModel::setGame(QJsonObject game)
{
    if (m_game == game)
        return;
    m_game = game;
    m_history = game["moves"].toArray();
    m_game["history"] = m_history;
    emit historyChanged();
    emit gameChanged();
}

int PiecesModel::currentMove() const
{
    return m_currentMove - m_history.begin();
}


QList<Piece::MoveFlags> PiecesModel::extractDirections(Piece::MoveFlags flags) const
{
    QList<Piece::MoveFlags> r;
    Piece::MoveFlags op = (flags & Piece::OrthogonalPair);
    if (op) {
        flags |= (Piece::Diagonally | Piece::Orthogonally | Piece::Sideways | Piece::Forwards | Piece::Backwards);
    }

    if (flags & (Piece::Diagonally | Piece::OrthogonalPair)) {
        if (flags & Piece::Forwards)
            r << (Piece::Forwards | Piece::Leftward | op) << (Piece::Forwards | Piece::Rightward | op);
        if (flags & Piece::Backwards)
            r << (Piece::Backwards | Piece::Leftward | op) << (Piece::Backwards | Piece::Rightward | op);
    }
    if (flags & (Piece::Orthogonally | Piece::OrthogonalPair)) {
        if (flags & Piece::Forwards)
            r << (Piece::Forwards | op);
        if (flags & Piece::Backwards)
            r << (Piece::Backwards | op);
        if (flags & Piece::Sideways)
            r << (Piece::Rightward | op) << (Piece::Leftward | op);
    }
    return r;
}

void PiecesModel::revertMove(QJsonObject m)
{
    QString srcPos = m["source"].toString();
    QString dstPos = m["destination"].toString();
    int capturedPiece = -1;
    if (m.contains("captured"))
        capturedPiece = m["captured"].toInt();
    int source = positionToIndex(srcPos.toLatin1());
    if (source < 0) {
        qWarning() << "Illegal position" << srcPos;
        return;
    }
    int destination = positionToIndex(dstPos.toLatin1());
    if (destination < 0) {
        qWarning() << "Illegal position" << dstPos;
        return;
    }
    Piece *cap;
    if (capturedPiece >= 0) {
        cap = m_pieces[capturedPiece];
        cap->setInGame(true);
        QModelIndex ci = createIndex(capturedPiece, 0);
        emit dataChanged(ci, ci, { InGameRole });
        layout[source] = cap;
    }
    Piece *pSrc = pieceAtSquare(destination);
    pSrc->setBoardIndex(source);
    qSwap(layout[destination], layout[source]);
    int srcIndex = indexOfPiece(pSrc);
    QModelIndex idx = createIndex(srcIndex, 0);
    emit dataChanged(idx, idx);
}

void PiecesModel::replayMove(QJsonObject m)
{
    QString srcPos = m["source"].toString();
    QString dstPos = m["destination"].toString();
    int capturedPiece = -1;
    if (m.contains("captured"))
        capturedPiece = m["captured"].toInt();
    int source = positionToIndex(srcPos.toLatin1());
    if (source < 0) {
        qWarning() << "Illegal position" << srcPos;
        return;
    }
    int destination = positionToIndex(dstPos.toLatin1());
    if (destination < 0) {
        qWarning() << "Illegal position" << dstPos;
        return;
    }
    if (contains(destination) && capturedPiece < 0) {
        qWarning() << "Illegal move" << srcPos << dstPos;
        return;
    }
    Piece *pSrc = pieceAtSquare(source);
    pSrc->setBoardIndex(destination);
    qSwap(layout[destination], layout[source]);
    int srcIndex = indexOfPiece(pSrc);
    QModelIndex idx = createIndex(srcIndex, 0);
    emit dataChanged(idx, idx);
    if (capturedPiece >= 0) {
        m_pieces[capturedPiece]->setInGame(false);
        layout[source] = nullptr;
        QModelIndex idx = createIndex(capturedPiece, 0);
        emit dataChanged(idx, idx, { InGameRole });
    }
}

QJsonArray PiecesModel::piecesToJson() const
{
    QJsonArray pieces;
    for (const Piece *p : m_pieces) {
        pieces.append(QJsonValue(QString(p->toByteArray())));
    }
    return pieces;
}

void PiecesModel::setInitial(const QByteArray &fenSetup)
{
    m_game["FEN"] = QString(fenSetup);
}

bool PiecesModel::iterator::move(Piece::MoveFlags dir, Piece::Color color)
{
    if (!dir & (Piece::OneSquare | Piece::TwoSquares))
        return false;
    quint32 dirIdx = (quint32)(dir & (Piece::SidesMask));
    int p = qPopulationCount(dirIdx);
    if (p > 2)
        return false;

//    qDebug() << "Dir:" << printableFlags((Piece::MoveFlags)dirIdx) << printableFlags(dir) << "index" << i << "pos" << indexToPos(i);
    int ni = i;
    int dx = 0;
    int dy = 0;
    if (dir & Piece::Forwards)
        --dy;
    if (dir & Piece::Backwards)
        ++dy;
    if (dir & Piece::Rightward)
        ++dx;
    if (dir & Piece::Leftward)
        --dx;
    if (dx == 0 && dy == 0)
        return false;

    if (color == Piece::Black)
        dy *= -1;

    if (dir & Piece::OrthogonalPair) {
        dir ^= Piece::OrthogonalPair;
        dir &= ~Piece::TwoSquares;
        quint32 dirIdx2 = ((dirIdx << 1) & Piece::SidesMask) | ((dirIdx & Piece::Leftward) >> 3);
        Piece::MoveFlags dir2 = Piece::OneSquare;
        if (p == 2)
            dir2 |= (Piece::MoveFlags)(dirIdx2 & dirIdx);
        else
            dir2 |= (Piece::MoveFlags)(dirIdx2 | dirIdx);
        if (!move(dir2, color)) {
            i = ni;
            return false;
        }
    }
    int nx = column() + dx;
    int ny = row() + dy;
    if (qBound(0, nx, 7) != nx)
        return false;
    if (qBound(0, ny, 7) != ny)
        return false;
//    qDebug() << "x:" << column() << "new x:" << nx;
//    qDebug() << "y:" << row() << "new y:" << ny;
    i = coordsToIndex(nx, ny);
//    qDebug() << "New index" << i;
    if (dir & Piece::TwoSquares) {
        dir &= ~Piece::TwoSquares;
        dir |= Piece::OneSquare;
        if (move(dir, color)) {
            i = ni;
            return false;
        }
    }
    return true;

}
