#include <array>
#include <iostream>
#include <optional>
#include <type_traits>
#include <vector>

template <std::size_t N, typename T = unsigned int,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
using board = std::array<std::array<T, N * N>, N * N>;

template <std::size_t N, typename T = unsigned int,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class sudoku_solver {
    using board = board<N, T>;

    static auto constexpr MAX = N * N;

    board b;

    constexpr bool is_valid_placement(board &solution, std::size_t row,
                                      std::size_t col,
                                      T const &possible_value) const {
        for (std::size_t i = 0; i < MAX; i++) {
            if (solution[row][i] == possible_value ||
                solution[i][col] == possible_value ||
                solution[N * (row / N) + i / N][N * (col / N) + i % N] ==
                    possible_value)
                return false;
        }
        return true;
    }

    constexpr std::optional<board> solve_sudoku(board &solution) const {
        constexpr auto SIZE = N * N;
        for (std::size_t row = 0; row < SIZE; row++) {
            for (std::size_t col = 0; col < SIZE; col++) {
                if (solution[row][col] == 0) {
                    for (T value = 1; value <= SIZE; value++) {
                        if (is_valid_placement(solution, row, col, value)) {
                            solution[row][col] = value;

                            if (solve_sudoku(solution)) {
                                return solution;
                            } else {
                                solution[row][col] = 0;
                            }
                        }
                    }
                    return std::nullopt;
                }
            }
        }
        return solution;
    }

   public:
    constexpr sudoku_solver(board const &b) : b(b) {}

    sudoku_solver(std::vector<std::vector<T>> const &v) {
        for (std::size_t i = 0; i < MAX; i++) {
            for (std::size_t j = 0; j < MAX; j++) {
                b[i][j] = v[i][j];
            }
        }
    }

    constexpr std::optional<board> get_solved() const {
        board solved = b;
        return solve_sudoku(solved);
    }
};

using solver = sudoku_solver<3>;

void solve_at_comptime() {
    std::cout << "SOLVED AT COMPTIME (gets embedded in the assembly/binary)\n";

    board<3> constexpr b{{{0, 7, 2, /**/ 0, 0, 4, /**/ 9, 0, 0},
                          {3, 0, 4, /**/ 0, 8, 9, /**/ 1, 0, 0},
                          {8, 1, 9, /**/ 0, 0, 6, /**/ 2, 5, 4},
                          /************************************/
                          /************************************/
                          {7, 0, 1, /**/ 0, 0, 0, /**/ 0, 9, 5},
                          {9, 0, 0, /**/ 0, 0, 2, /**/ 0, 7, 0},
                          {0, 0, 0, /**/ 8, 0, 7, /**/ 0, 1, 2},
                          /************************************/
                          /************************************/
                          {4, 0, 5, /**/ 0, 0, 1, /**/ 6, 2, 0},
                          {2, 3, 7, /**/ 0, 0, 0, /**/ 5, 0, 1},
                          {0, 0, 0, /**/ 0, 2, 5, /**/ 7, 0, 0}}};

    solver constexpr s{b};

    if constexpr (auto constexpr solved = s.get_solved()) {
        for (auto const &row : *solved) {
            for (auto const &element : row) {
                std::cout << element << ' ';
            }
            std::cout << '\n';
        }
    } else {
        std::cout << "Not solvable\n";
    }
}

void solve_at_runtime() {
    std::vector<std::vector<unsigned int>> b;

    b.resize(9);
    for (auto &row : b) row.resize(9);

    std::cout << "Enter the board:\n";
    for (auto &row : b) {
        for (auto &e : row) {
            std::cin >> e;
        }
    }

    solver s{b};

    std::cout << "SOLVED AT RUNTIME\n";
    if (auto const solved = s.get_solved()) {
        for (auto const &row : *solved) {
            for (auto const &element : row) {
                std::cout << element << ' ';
            }
            std::cout << '\n';
        }
    } else {
        std::cout << "Not solvable\n";
    }
}

int main() {
    solve_at_comptime();
    solve_at_runtime();
    return 0;
}
