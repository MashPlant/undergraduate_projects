//
// Created by mashplant on 18-9-3.
//

#ifndef CHESS_CHESSFRAME_HPP
#define CHESS_CHESSFRAME_HPP

#include <vector>
#include <memory>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QTextStream>
#include <cstdint>

struct ChessFrame;

struct Piece
{
    typedef std::unique_ptr<Piece> pointer;
    const static int X = 9, Y = 10;
    const static int red = -1, black = 1;
    /*
     * (0,0)...(8,0)  -- black
     *   .       .
     *   .       .
     *   .       .
     * (0,9)...(8,9)  -- red
     * */
    int color;

    Piece(int color) : color(color)
    {}

    static int xy2int(int x, int y)
    { return x * Y + y; }

    static void int2xy(int i, int &x, int &y)
    { x = i / Y, y = i % Y; }

    static bool valid(int x, int y)
    { return 0 <= x && x < X && 0 <= y && y < Y; }

    static pointer create(int id, int color);

    bool movable(const pointer &p)
    { return !p || p->color == -color; }

    void paint(QPainter &painter, int x, int y, bool selected);

    virtual std::vector<int> next_moves(const ChessFrame &frame, int x, int y) = 0;

    virtual int get_id() = 0;
    // it doesn't need a virtual dtor, because those piece that inherit it won't have any member

protected:
    virtual const QPixmap &get_pic(bool selected) = 0;
};

struct ChessFrame
{
    typedef Piece::pointer pointer;
    const static int X = Piece::X, Y = Piece::Y;
    const static int red = Piece::red, black = Piece::black;

    pointer board[X][Y];
    int selected = -1;
    int player = Piece::red;

    const pointer *operator[](int index) const
    { return board[index]; }

    void paint(QPainter &painter);

    ChessFrame();

    // please pass (event->x() - width() / 2) / factor, (event->y() - height() / 2) / factor to click
    // return -1 if no move, return (f << 16) | t otherwise (client should call move(f, t))
    [[nodiscard]]
    uint32_t click(qreal x, qreal y, int human_player);

    // return the information of the move
    // --8 bit unused--.--8 bit of winning info--.--8 bit of from index--.--8 bit of to index--
    // winner will be: 0 for not end, 0x1 for black, 0xff for red
    // so when black wins, (ret >> 16) & 255 == black, when red wins, (ret >> 16) & 255 != red
    [[nodiscard]]
    uint32_t exec_move(int f, int t);

    int find_checkmate();

    friend QTextStream &operator<<(QTextStream &stream, const ChessFrame &frame);

    friend QTextStream &operator>>(QTextStream &stream, ChessFrame &frame);
};

#endif //CHESS_CHESSFRAME_HPP
