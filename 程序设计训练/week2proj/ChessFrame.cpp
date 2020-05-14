//
// Created by mashplant on 18-9-3.
//

#include "ChessFrame.hpp"
#include <algorithm>
#include <QSound>
#include <QUrl>
#include <QtMultimedia/QMediaPlayer>

struct Soldier : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        std::vector<int> ret;
        if (valid(x, y + color) && movable(frame[x][y + color]))
            ret.push_back(xy2int(x, y + color));
        if ((color == red) ^ (y >= 5))
        {
            if (valid(x + 1, y) && movable(frame[x + 1][y]))
                ret.push_back(xy2int(x + 1, y));
            if (valid(x - 1, y) && movable(frame[x + 1][y]))
                ret.push_back(xy2int(x - 1, y));
        }
        return ret;
    }

    int get_id() override
    { return 6; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BP.GIF"), QStringLiteral(":/image/BPS.GIF")},
                                     {QStringLiteral(":/image/RP.GIF"), QStringLiteral(":/image/RPS.GIF")}};
        return pics[color == red][selected];
    }
};

struct Cannon : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {1, -1, 0, 0}, dy[] = {0, 0, 1, -1};
        std::vector<int> ret;
        int o_x = x, o_y = y;
        for (int i = 0; i < 4; ++i)
        {
            x = o_x, y = o_y;
            while (valid(x += dx[i], y += dy[i]) && !frame[x][y])
                ret.push_back(xy2int(x, y));
            while (valid(x += dx[i], y += dy[i]) && !frame[x][y]);
            if (valid(x, y) && frame[x][y] && frame[x][y]->color == -color)
                ret.push_back(xy2int(x, y));
        }
        return ret;
    }

    int get_id() override
    { return 5; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BC.GIF"), QStringLiteral(":/image/BCS.GIF")},
                                     {QStringLiteral(":/image/RC.GIF"), QStringLiteral(":/image/RCS.GIF")}};
        return pics[color == red][selected];
    }
};

struct Rook : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {1, -1, 0, 0}, dy[] = {0, 0, 1, -1};
        std::vector<int> ret;
        int o_x = x, o_y = y;
        for (int i = 0; i < 4; ++i)
        {
            x = o_x, y = o_y;
            while (valid(x += dx[i], y += dy[i]) && !frame[x][y])
                ret.push_back(xy2int(x, y));
            if (valid(x, y) && frame[x][y] && frame[x][y]->color == -color)
                ret.push_back(xy2int(x, y));
        }
        return ret;
    }

    int get_id() override
    { return 4; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BR.GIF"), QStringLiteral(":/image/BRS.GIF")},
                                     {QStringLiteral(":/image/RR.GIF"), QStringLiteral(":/image/RRS.GIF")}};
        return pics[color == red][selected];
    }
};

struct Horse : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {2, 1, -1, -2, -2, -1, 1, 2}, dy[] = {1, 2, 2, 1, -1, -2, -2, -1};
        const int block_x[] = {1, 0, 0, -1, -1, 0, 0, 1}, block_y[] = {0, 1, 1, 0, 0, -1, -1, 0};
        std::vector<int> ret;
        for (int i = 0; i < 8; ++i)
            if (valid(x + dx[i], y + dy[i]) && !frame[x + block_x[i]][y + block_y[i]]
                && movable(frame[x + dx[i]][y + dy[i]]))
                ret.push_back(xy2int(x + dx[i], y + dy[i]));
        return ret;
    }

    int get_id() override
    { return 3; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BN.GIF"), QStringLiteral(":/image/BNS.GIF")},
                                     {QStringLiteral(":/image/RN.GIF"), QStringLiteral(":/image/RNS.GIF")}};
        return pics[color == red][selected];
    }
};

struct Bishop : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {2, -2, -2, 2}, dy[] = {2, 2, -2, -2};
        const int block_x[] = {1, -1, -1, 1}, block_y[] = {1, 1, -1, -1};
        std::vector<int> ret;
        for (int i = 0; i < 4; ++i)
            if (valid(x + dx[i], y + dy[i]) && !frame[x + block_x[i]][y + block_y[i]]
                && ((y + dy[i] <= 4) == (y <= 4)) && movable(frame[x + dx[i]][y + dy[i]]))
                ret.push_back(xy2int(x + dx[i], y + dy[i]));
        return ret;
    }

    int get_id() override
    { return 2; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BB.GIF"), QStringLiteral(":/image/BBS.GIF")},
                                     {QStringLiteral(":/image/RB.GIF"), QStringLiteral(":/image/RBS.GIF")}};
        return pics[color == red][selected];
    }
};

struct Guard : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {1, -1, -1, 1}, dy[] = {1, 1, -1, -1};
        std::vector<int> ret;
        int o_x = x, o_y = y;
        for (int i = 0; i < 4; ++i)
        {
            x = o_x + dx[i], y = o_y + dy[i];
            if (valid(x, y) && movable(frame[x][y]) && 3 <= x && x <= 5
                && (o_y <= 2 ? y <= 2 : 7 <= y))
                ret.push_back(xy2int(x, y));
        }
        return ret;
    }

    int get_id() override
    { return 1; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BA.GIF"), QStringLiteral(":/image/BAS.GIF")},
                                     {QStringLiteral(":/image/RA.GIF"), QStringLiteral(":/image/RAS.GIF")}};
        return pics[color == red][selected];
    }
};

struct King : Piece
{
    using Piece::Piece;

    std::vector<int> next_moves(const ChessFrame &frame, int x, int y) override
    {
        const int dx[] = {1, -1, 0, 0}, dy[] = {0, 0, 1, -1};
        std::vector<int> ret;
        int o_x = x, o_y = y;
        for (int i = 0; i < 4; ++i)
        {
            x = o_x + dx[i], y = o_y + dy[i];
            if (valid(x, y) && movable(frame[x][y]) && 3 <= x && x <= 5
                && (o_y <= 2 ? y <= 2 : 7 <= y))
                ret.push_back(xy2int(x, y));
        }
        x = o_x, y = o_y;
        int d = y <= 2 ? 1 : -1;
        while (valid(x, y += d) && !frame[x][y]);
        if (valid(x, y) && dynamic_cast<King *>(frame[x][y].get()))
            ret.push_back(xy2int(x, y));
        return ret;
    }

    int get_id() override
    { return 0; }

protected:
    const QPixmap &get_pic(bool selected) override
    {
        static QPixmap pics[2][2] = {{QStringLiteral(":/image/BK.GIF"), QStringLiteral(":/image/BKS.GIF")},
                                     {QStringLiteral(":/image/RK.GIF"), QStringLiteral(":/image/RKS.GIF")}};
        return pics[color == red][selected];
    }
};

void Piece::paint(QPainter &painter, int x, int y, bool selected)
{
    const QPixmap &pic = get_pic(selected);
    painter.drawPixmap(QRectF(x - X / 2.0, y - Y / 2.0, 1, 1),
                       pic, pic.rect());
}

Piece::pointer Piece::create(int id, int color)
{
    switch (id)
    {
        case 0:
            return pointer(new King(color));
        case 1:
            return pointer(new Guard(color));
        case 2:
            return pointer(new Bishop(color));
        case 3:
            return pointer(new Horse(color));
        case 4:
            return pointer(new Rook(color));
        case 5:
            return pointer(new Cannon(color));
        case 6:
            return pointer(new Soldier(color));
        default:
            qDebug() << "unknown piece id";
            return nullptr;
    }
}

ChessFrame::ChessFrame()
{
    board[0][0].reset(new Rook(black));
    board[1][0].reset(new Horse(black));
    board[2][0].reset(new Bishop(black));
    board[3][0].reset(new Guard(black));
    board[4][0].reset(new King(black));
    board[5][0].reset(new Guard(black));
    board[6][0].reset(new Bishop(black));
    board[7][0].reset(new Horse(black));
    board[8][0].reset(new Rook(black));

    board[1][2].reset(new Cannon(black));
    board[7][2].reset(new Cannon(black));
    board[0][3].reset(new Soldier(black));
    board[2][3].reset(new Soldier(black));
    board[4][3].reset(new Soldier(black));
    board[6][3].reset(new Soldier(black));
    board[8][3].reset(new Soldier(black));

    board[0][9].reset(new Rook(red));
    board[1][9].reset(new Horse(red));
    board[2][9].reset(new Bishop(red));
    board[3][9].reset(new Guard(red));
    board[4][9].reset(new King(red));
    board[5][9].reset(new Guard(red));
    board[6][9].reset(new Bishop(red));
    board[7][9].reset(new Horse(red));
    board[8][9].reset(new Rook(red));

    board[1][7].reset(new Cannon(red));
    board[7][7].reset(new Cannon(red));
    board[0][6].reset(new Soldier(red));
    board[2][6].reset(new Soldier(red));
    board[4][6].reset(new Soldier(red));
    board[6][6].reset(new Soldier(red));
    board[8][6].reset(new Soldier(red));
}

void ChessFrame::paint(QPainter &painter)
{
    static QPixmap background(":/image/BOARD.GIF");
    static QPixmap selector(":/image/OOS.GIF");
    painter.drawPixmap(QRectF(-X / 2.0 - 0.25, -Y / 2.0 - 0.25, X + 0.5, Y + 0.5), background, background.rect());
//    for (double j = -Y / 2.0; j <= Y / 2.0; ++j)
//        painter.drawLine(QPointF(-X / 2.0, j), QPointF(X / 2.0, j));
//    for (double i = -X / 2.0; i <= X / 2.0; ++i)
//        painter.drawLine(QPointF(i, -Y / 2.0), QPointF(i, Y / 2.0));
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            if (board[x][y])
                board[x][y]->paint(painter, x, y, selected == Piece::xy2int(x, y));
    if (selected != -1)
    {
        int x, y;
        Piece::int2xy(selected, x, y);
        for (auto move : board[x][y]->next_moves(*this, x, y))
        {
            Piece::int2xy(move, x, y);
            painter.drawPixmap(QRectF(x - X / 2.0, y - Y / 2.0, 1, 1),
                               selector, selector.rect());
        }
    }
}

uint32_t ChessFrame::click(qreal x, qreal y, int human_player)
{
    int clk_x = x + X / 2.0, clk_y = y + Y / 2.0;
    if (Piece::valid(clk_x, clk_y))
    {
        int tmp = Piece::xy2int(clk_x, clk_y);
        if (selected == -1)
        {
            if (board[clk_x][clk_y] && board[clk_x][clk_y]->color == player && player == human_player)
                selected = tmp;
        }
        else
        {
            int x, y;
            Piece::int2xy(selected, x, y);
            auto moves = board[x][y]->next_moves(*this, x, y);
            if (std::find(moves.begin(), moves.end(), tmp) != moves.end())
            {
                int f = selected;
                selected = -1;
                return (f << 16) | tmp;
            }
            else if (board[clk_x][clk_y] && board[clk_x][clk_y]->color == player && player == human_player)
                selected = tmp;
        }
    }
    return -1u;
}

uint32_t ChessFrame::exec_move(int f, int t)
{
    int xf, yf, xt, yt, winner = 0;
    Piece::int2xy(f, xf, yf), Piece::int2xy(t, xt, yt);
    if (dynamic_cast<King *>(board[xt][yt].get()))
        winner = player;
    board[xt][yt] = std::move(board[xf][yf]);
    if (find_checkmate())
    {
        QMediaPlayer *media = new QMediaPlayer;
        media->setMedia(QUrl("qrc:///sound/checkmate.mp3"));
        media->setVolume(100);
        media->play();
    }
    else
    {
        QMediaPlayer *media = new QMediaPlayer;
        media->setMedia(QUrl("qrc:///sound/down.mp3"));
        media->setVolume(100);
        media->play();
    }
    player = -player;
    return (winner << 16) | (f << 8) | t;
}

int ChessFrame::find_checkmate()
{
    for (int x = 0; x < ChessFrame::X; ++x)
        for (int y = 0; y < ChessFrame::Y; ++y)
            if (board[x][y])
                for (auto move : board[x][y]->next_moves(*this, x, y))
                {
                    int mx, my;
                    Piece::int2xy(move, mx, my);
                    if (dynamic_cast<King *>(board[mx][my].get()) && board[mx][my]->color == -board[x][y]->color)
                        return board[x][y]->color;
                }
    return 0;
}

QTextStream &operator<<(QTextStream &stream, const ChessFrame &frame)
{
    const char *names[] = {"red", "", "black"}, **p_name = names + 1;
    int colors[] = {ChessFrame::red, ChessFrame::black};
    if (frame.player == ChessFrame::black)
        std::swap(colors[0], colors[1]);
    for (auto color : colors)
    {
        int count[7]{};
        stream << p_name[color] << '\n';
        for (int x = 0; x < ChessFrame::X; ++x)
            for (int y = 0; y < ChessFrame::Y; ++y)
                if (frame[x][y] && frame[x][y]->color == color)
                    ++count[frame[x][y]->get_id()];
        for (int i = 0; i < 7; ++i)
        {
            stream << count[i] << ' ';
            for (int x = 0; x < ChessFrame::X; ++x)
                for (int y = 0; y < ChessFrame::Y; ++y)
                    if (frame[x][y] && frame[x][y]->color == color && frame[x][y]->get_id() == i)
                        stream << '<' << x << ',' << ChessFrame::Y - 1 - y << '>';
            stream << '\n';
        }
    }
}

QTextStream &operator>>(QTextStream &stream, ChessFrame &frame)
{
    for (int x = 0; x < ChessFrame::X; ++x)
        for (int y = 0; y < ChessFrame::Y; ++y)
            frame.board[x][y].reset();
    frame.player = ChessFrame::red;
    int colors[] = {ChessFrame::red, ChessFrame::black};
    auto next_char = [&stream]
    {
        char ch;
        while (stream >> ch, isspace(ch));
        return ch;
    };
    QString type;
    stream >> type;
    if (type == "black")
        std::swap(colors[0], colors[1]), frame.player = ChessFrame::black;
    for (auto color : colors)
    {
        for (int i = 0, count, x, y; i < 7; ++i)
        {
            stream >> count;
            for (int j = 0; j < count; ++j)
            {
                next_char(), x = next_char() - '0', next_char(), y = next_char() - '0', next_char();
                frame.board[x][ChessFrame::Y - 1 - y] = Piece::create(i, color);
            }
        }
        stream >> type;
    }
    return stream;
}