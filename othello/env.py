from __future__ import annotations

import os

import numpy as np
import numpy.typing as npt

# isort: off
from othello.libcpp import CEnv

# isort: on

Board = npt.NDArray[np.int32]

__all__ = ["Env", "make"]


class Env(CEnv):
    # def render(self) -> npt.NDArray[np.uint8]:
    #     size = 1024  # size of board
    #     csize = size // 8  # size of cell
    #     margin = 10  # margin from disk to grid line

    #     # Initialize board with green color
    #     img = Image.new("RGB", (size, size), color="#007700")
    #     draw = ImageDraw.Draw(img)

    #     # If board history exists, highlight last move
    #     if self.history_size > 0:
    #         last_action = self.last_action
    #         i, j = last_action.x, last_action.y
    #         if 0 <= i < 8 and 0 <= j < 8:
    #             x = i * csize
    #             y = j * csize
    #             draw.rectangle((x, y, x + csize, y + csize), fill="#00aa00")

    #     # Grid lines
    #     for i in range(1, 8):
    #         y = i * csize
    #         draw.line((0, y, size, y), "black", width=3)

    #     for j in range(1, 8):
    #         x = j * csize
    #         draw.line((x, 0, x, size), "black", width=2)

    #     # Stars
    #     stars = [
    #         (csize * 2 - 10, csize * 2 - 10, csize * 2 + 10, csize * 2 + 10),
    #         (csize * 2 - 10, csize * 6 - 10, csize * 2 + 10, csize * 6 + 10),
    #         (csize * 6 - 10, csize * 2 - 10, csize * 6 + 10, csize * 2 + 10),
    #         (csize * 6 - 10, csize * 6 - 10, csize * 6 + 10, csize * 6 + 10),
    #     ]
    #     for st in stars:
    #         draw.ellipse(st, fill="black")

    #     # Disks
    #     B = (self.board == +1).astype("int32")
    #     W = (self.board == -1).astype("int32")
    #     for i in range(8):
    #         for j in range(8):
    #             px = j * csize + margin
    #             py = i * csize + margin
    #             diameter = csize - margin * 2
    #             xy = (px, py, px + diameter, py + diameter)
    #             if B[i, j] != 0:
    #                 draw.ellipse(xy, fill="black")
    #             if W[i, j] != 0:
    #                 draw.ellipse(xy, fill="white")

    #     return np.array(img, dtype="uint8")

    def __repr__(self) -> str:
        B = (self.board == +1).astype("int32")
        W = (self.board == -1).astype("int32")

        sep = "+-" * 8 + "+" + os.linesep
        ret = ""
        ret += sep
        for i in range(self.board.shape[0]):
            ret += "|"
            for j in range(self.board.shape[1]):
                if B[i, j] != 0:
                    ret += "o"
                elif W[i, j] != 0:
                    ret += "x"
                else:
                    ret += " "
                ret += "|"
            ret += os.linesep
            ret += sep
        return ret

    def __dir__(self) -> list[str]:
        return [
            "copy",
            "reset",
            "update",
            "undo",
            "legal_moves",
            "render",
            "count",
            "is_done",
            "is_win",
            "is_lose",
        ]

    # def _repr_png_(self):
    #     fig, ax = plt.subplots(figsize=(6, 6), dpi=100)
    #     img = self.render()
    #     size = img.shape[0]
    #     csize = size // 8

    #     ax.imshow(img)
    #     ax.xaxis.tick_top()
    #     ax.set(
    #         xticks=np.arange(csize // 2, size, csize),
    #         yticks=np.arange(csize // 2, size, csize),
    #         xticklabels=[chr(ord("A") + i) for i in range(8)],
    #         yticklabels=np.arange(1, 9),
    #     )
    #     data = print_figure(fig, "png")
    #     plt.close(fig)
    #     return data


def make() -> Env:
    return Env()
