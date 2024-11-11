#ifndef _OTHELLO_ENV_H_
#define _OTHELLO_ENV_H_

#include <vector>
#include <tuple>
#include <deque>

#include "player.h"
#include "action.h"
#include "bitboard.h"

class Env {
public:
    Env()
        : player{ Player::BLACK }
        , b_board{ 0x0000'0000'0000'0000 }
        , w_board{ 0x0000'0000'0000'0000 }
        , board_history{}
        , action_history{} {
    }

    Env(const Env &env)
        : Env() {
        this->operator=(env);
    }

    Env &operator=(const Env &other) {
        this->player = other.player;
        this->b_board = other.b_board;
        this->w_board = other.w_board;
        this->board_history = other.board_history;
        this->action_history = other.action_history;
        return *this;
    }

    Env copy() const {
        Env env = *this;
        return env;
    }

    void reset() {
        player = Player(Player::BLACK);
        b_board = 0x0000'0008'1000'0000;
        w_board = 0x0000'0010'0800'0000;
        action_history.clear();
        board_history.clear();
    }

    std::array<int, NUM_CELLS> board() const {
        auto B = bitsToBoard(b_board);
        auto W = bitsToBoard(w_board);
        std::transform(B.begin(), B.end(), W.begin(), B.begin(), [](int b, int w) -> int { return b - w; });
        return B;
    }

    int count(Player player) const {
        if (player.isBlack()) {
            return bitCount(b_board);
        } else {
            return bitCount(w_board);
        }
    }

    bool isDone() const {
        return ::isDone(b_board, w_board);
    }

    bool isWin(Player player) const {
        return this->count(player) > this->count(-player);
    }

    bool isLose(Player player) const {
        return !this->isWin(player);
    }

    void undo() {
        player = -player;
        b_board = std::get<0>(board_history.back());
        w_board = std::get<1>(board_history.back());
        board_history.pop_back();
        action_history.pop_back();
    }

    void update(Action action) {
        action_history.push_back(action);
        board_history.push_back(std::make_tuple(b_board, w_board));

        uint64_t b0, b1;
        if (player.isBlack()) {
            b0 = b_board;
            b1 = w_board;
        } else {
            b0 = w_board;
            b1 = b_board;
        }

        const std::string sym = action.toSymbol();
        if (sym != "") {
            const uint64_t put = coordinatesToBits(sym);
            const auto result = reverse(put, b0, b1);
            b0 = std::get<0>(result);
            b1 = std::get<1>(result);
        }

        if (player.isBlack()) {
            player = -player;
            b_board = b0;
            w_board = b1;
        } else {
            player = -player;
            b_board = b1;
            w_board = b0;
        }
    }

    Player getPlayer() const {
        return player;
    }

    std::vector<Action> legalActions() const {
        uint64_t b0, b1;
        if (player.isBlack()) {
            b0 = b_board;
            b1 = w_board;
        } else {
            b0 = w_board;
            b1 = b_board;
        }

        const uint64_t bits = makeLegalBoard(b0, b1);
        if (bits == 0) {
            std::vector<Action> ret;
            ret.push_back(Action::makePass(player));
            return ret;
        }

        const auto legal = bitsToBoard(bits);
        std::vector<Action> ret;
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (legal[y * 8 + x] != 0) {
                    ret.push_back(Action(player, x, y));
                }
            }
        }
        return ret;
    }

    int historySize() const {
        return action_history.size();
    }

    Action getLastAction() const {
        return action_history.back();
    }

private:
    Player player;
    uint64_t b_board;
    uint64_t w_board;
    std::deque<std::tuple<uint64_t, uint64_t>> board_history;
    std::deque<Action> action_history;
};

#endif  // _OTHELLO_ENV_H_