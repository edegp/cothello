#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "cache.h"
#include "player.h"
#include "action.h"
#include "env.h"

template <typename T, int Width, int Height>
py::array_t<T> to_numpy(const std::array<T, Width * Height> &data) {
    return py::array_t<T>({ Width, Height }, data.data());
}

PYBIND11_MODULE(libcpp, m_cpp) {
    // Cache
    auto cls_cache = py::class_<Cache>(m_cpp, "Cache");
    cls_cache.def(py::init<size_t>(), py::arg("capacity"), "Constructor with cache capacity")
        .def("get", 
             [](Cache &self, uint64_t key) -> py::object {
                 double value;
                 if (self.get(key, value)) {
                     return py::float_(value);
                 } else {
                     return py::none();
                 }
             },
             py::arg("key"),
             "Retrieve value from cache. Returns None if key not found.")
        .def("put",
             &Cache::put,
             py::arg("key"),
             py::arg("value"),
             "Insert or update a key-value pair in the cache.")
        .def("size",&Cache::size,"Get the number of items in the cache.")
        .def("to_dict",
             &Cache::export_cache,
             "Export the entire cache as a dictionary.")
        .def("from_dict",
             &Cache::import_cache,
             py::arg("data"),
             "Import cache data from a dictionary.");
    // Player
    auto cls_player = py::class_<Player>(m_cpp, "Player");
    cls_player.def(py::init<int>())
        .def_property_readonly("name", &Player::str, "Player name")
        .def_property_readonly("value", &Player::getValue, "Player value")
        .def_property_readonly_static("BLACK", [](py::object) { return (int)Player::BLACK; })
        .def_property_readonly_static("WHITE", [](py::object) { return (int)Player::WHITE; })
        .def_static(
            "as_black", [](py::object) { return Player(Player::BLACK); }, "Create a black player")
        .def_static(
            "as_white", [](py::object) { return Player(Player::WHITE); }, "Create a white player")
        .def("is_black", &Player::isBlack, "Check if the player is black")
        .def("is_white", &Player::isWhite, "Check if the player is white")
        .def("__eq__", &Player::operator==)
        .def("__ne__", &Player::operator!=)
        .def("__neg__", [](const Player &self) { return -self; })
        .def("__str__", &Player::str)
        .def("__repr__", &Player::repr);

    // Action
    auto cls_action = py::class_<Action>(m_cpp, "Action");
    cls_action.def(py::init<Player, int, int>())
        .def("to_symbol", &Action::toSymbol)
        .def_property_readonly("player", &Action::getPlayer, "Player of the action")
        .def_property_readonly("x", &Action::getX, "X coordinate")
        .def_property_readonly("y", &Action::getY, "Y coordinate")
        .def("__str__", &Action::str)
        .def("__repr__", &Action::repr);

    // Env
    auto cls_env = py::class_<Env>(m_cpp, "CEnv");
    cls_env.def(py::init<>())
        .def("copy", &Env::copy)
        .def("reset", &Env::reset)
        .def("is_done", &Env::isDone)
        .def_property_readonly("board", [](const Env &self) { return to_numpy<int, 8, 8>(self.board()); })
        .def("count", [](const Env &self, int player) { return self.count(Player(player)); })
        .def("is_win", &Env::isWin, "Check if the player wins", py::arg("player"))
        .def("is_lose", &Env::isLose, "Check if the player loses", py::arg("player"))
        .def("undo", &Env::undo, "Undo the last action")
        .def("update", &Env::update, "Update the environment with the action", py::arg("action"))
        .def("turn_change", &Env::turnChange, "Change the turn")
        .def("legal_actions", &Env::legalActions, "Get the list of legal actions")
        .def_property_readonly("player", &Env::getPlayer, "Current player")
        .def_property_readonly("history_size", &Env::historySize)
        .def_property_readonly("last_action", &Env::getLastAction)
        .def_property_readonly("w_board", &Env::getWBoard)
        .def_property_readonly("b_board", &Env::getBBoard);

    // Bitboard
    auto m_bb = m_cpp.def_submodule("bitboard", "Bitboard submodule");
    m_bb.def("c_coordinates_to_bits", &coordinatesToBits);
    m_bb.def("c_bits_to_board", &bitsToBoard);
    m_bb.def("c_make_legal_board", &makeLegalBoard);
    m_bb.def("c_can_put", &isAbleToPut);
    m_bb.def("c_reverse", &reverse);
    m_bb.def("c_is_done", &isGameSet);
    m_bb.def("c_bit_count", &bitCount);
}
