// Single-file A* grid pathfinding example (C++11+)
//
// Note (Windows): If you include <Windows.h> somewhere in your project,
// it may define min/max macros that conflict with std::min/std::max.
// Common fixes are:
//   #define NOMINMAX
//   #include <Windows.h>
// Or using (std::min)(a,b) / (std::max)(a,b) in code.

#include <algorithm>   // std::min, std::max, std::reverse
#include <array>       // std::array
#include <cmath>       // std::sqrt
#include <cstddef>     // std::size_t
#include <iomanip>     // std::setprecision
#include <iostream>    // std::cout
#include <limits>      // std::numeric_limits
#include <queue>       // std::priority_queue
#include <string>      // std::string
#include <utility>     // std::pair
#include <vector>      // std::vector

// ---------------------------------
// Basic geometry types
// ---------------------------------

template <typename T>
struct Point {
    T x;
    T y;

    constexpr Point() : x(T{}), y(T{}) {}
    constexpr Point(T x_, T y_) : x(x_), y(y_) {}

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
};

using PointI = Point<int>;

// ---------------------------------
// Grid / Map representation
// ---------------------------------

class Grid {
public:
    explicit Grid(const std::vector<std::string>& ascii)
        : height_(static_cast<int>(ascii.size())),
        width_(ascii.empty() ? 0 : static_cast<int>(ascii.front().size())),
        walkable_(static_cast<std::size_t>(width_* height_), true) {

        for (int y = 0; y < height_; ++y) {
            if (static_cast<int>(ascii[y].size()) != width_) {
                throw std::runtime_error("All rows must have the same width");
            }
            for (int x = 0; x < width_; ++x) {
                const char c = ascii[y][x];
                walkable_[static_cast<std::size_t>(index(PointI{ x, y }))] = (c != '#');
            }
        }
    }

    int width() const { return width_; }
    int height() const { return height_; }

    bool inBounds(const PointI& p) const {
        return (p.x >= 0 && p.x < width_ && p.y >= 0 && p.y < height_);
    }

    bool isWalkable(const PointI& p) const {
        if (!inBounds(p)) return false;
        return walkable_[static_cast<std::size_t>(index(p))];
    }

    int index(const PointI& p) const {
        return p.y * width_ + p.x;
    }

    PointI pointFromIndex(int idx) const {
        return PointI{ idx % width_, idx / width_ };
    }

private:
    int height_ = 0;
    int width_ = 0;
    std::vector<bool> walkable_;
};

// ---------------------------------
// A* configuration
// ---------------------------------

enum class Movement { Four, Eight };
enum class Heuristic { Manhattan, Octile };

struct AStarConfig {
    Movement movement = Movement::Eight;
    Heuristic heuristic = Heuristic::Octile;

    // Move costs (typical choices)
    //   orthCost = 1.0
    //   diagCost = 1.0      (diagonal costs same as orthogonal; "Chebyshev" geometry)
    //   diagCost = sqrt(2)  (Euclidean-like step length on a grid)
    float orthCost = 1.0f;
    float diagCost = 1.41421356237f;

    // If true, diagonal move (dx,dy) is allowed only when both adjacent orthogonal cells are walkable.
    // This prevents "corner cutting" through obstacle corners.
    bool forbidCornerCutting = false;
};

// ---------------------------------
// Node record stored per grid cell
// ---------------------------------

struct Node {
    float g = std::numeric_limits<float>::infinity();
    float h = 0.0f;
    float f = std::numeric_limits<float>::infinity();

    int parent = -1;   // parent cell index for path reconstruction
    bool closed = false;
};

// Entry stored in the open set (priority queue)
struct OpenEntry {
    int idx;
    float f;
    float g;
    float h;

    OpenEntry() : idx(-1), f(0), g(0), h(0) {}
    OpenEntry(int idx_, float f_, float g_, float h_) : idx(idx_), f(f_), g(g_), h(h_) {}
};

// Min-heap behavior for std::priority_queue (which is a max-heap by default)
struct OpenEntryGreater {
    bool operator()(const OpenEntry& a, const OpenEntry& b) const {
        if (a.f != b.f) return a.f > b.f;  // lower f is better
        if (a.h != b.h) return a.h > b.h;  // tie-break: closer to goal first
        if (a.g != b.g) return a.g < b.g;  // then prefer deeper (larger g)
        return a.idx > b.idx;              // deterministic
    }
};

// ---------------------------------
// Heuristics
// ---------------------------------

static float heuristicCost(const PointI& from, const PointI& to, const AStarConfig& cfg) {
    const int dx = std::abs(to.x - from.x);
    const int dy = std::abs(to.y - from.y);

    switch (cfg.heuristic) {
    case Heuristic::Manhattan:
        // L1 metric: |dx| + |dy|
        return cfg.orthCost * static_cast<float>(dx + dy);

    case Heuristic::Octile: {
        // Octile distance (8-connected grid) general form:
        //   h = (max(dx,dy) - min(dx,dy)) * D + min(dx,dy) * D2
        // where D is orthCost, D2 is diagCost.
        const int mn = (std::min)(dx, dy); // (std::min) avoids Windows min macro expansion
        const int mx = (std::max)(dx, dy);
        return (static_cast<float>(mx - mn) * cfg.orthCost) + (static_cast<float>(mn) * cfg.diagCost);
    }
    }

    return 0.0f; // unreachable
}

// ---------------------------------
// A* search implementation
// ---------------------------------

std::vector<PointI> aStarFindPath(
    const Grid& grid,
    const PointI& start,
    const PointI& goal,
    const AStarConfig& cfg,
    std::size_t* outExpandedNodes = nullptr)
{
    // Complexity notes (typical):
    // - Let V be the number of grid cells and E the number of directed edges (~4V or ~8V).
    // - With a binary heap open set, each push/pop is O(log N), where N is the current open size.
    // - Because std::priority_queue has no decrease-key, we may push duplicates; we discard stale
    //   entries on pop. This typically keeps the same asymptotic behavior, with a constant-factor
    //   overhead.
    //
    // Optimality notes:
    // - If edge costs are non-negative and the heuristic never overestimates the real remaining cost
    //   (admissible), A* is optimal. With a closed set (graph search), using a consistent heuristic
    //   avoids re-expansions and preserves optimality.

    if (outExpandedNodes) *outExpandedNodes = 0;

    if (!grid.inBounds(start) || !grid.inBounds(goal)) return {};
    if (!grid.isWalkable(start) || !grid.isWalkable(goal)) return {};

    const int startIdx = grid.index(start);
    const int goalIdx = grid.index(goal);

    std::vector<Node> nodes(static_cast<std::size_t>(grid.width() * grid.height()));

    std::priority_queue<OpenEntry, std::vector<OpenEntry>, OpenEntryGreater> open;

    nodes[static_cast<std::size_t>(startIdx)].g = 0.0f;
    nodes[static_cast<std::size_t>(startIdx)].h = heuristicCost(start, goal, cfg);
    nodes[static_cast<std::size_t>(startIdx)].f = nodes[static_cast<std::size_t>(startIdx)].h;

    open.push(OpenEntry{ startIdx,
                        nodes[static_cast<std::size_t>(startIdx)].f,
                        nodes[static_cast<std::size_t>(startIdx)].g,
                        nodes[static_cast<std::size_t>(startIdx)].h });

    const std::array<PointI, 8> dirs = {
        PointI{ 1,  0}, PointI{-1,  0}, PointI{ 0,  1}, PointI{ 0, -1},
        PointI{ 1,  1}, PointI{ 1, -1}, PointI{-1,  1}, PointI{-1, -1}
    };

    const int dirCount = (cfg.movement == Movement::Eight) ? 8 : 4;

    while (!open.empty()) {
        const OpenEntry cur = open.top();
        open.pop();

        Node& curNode = nodes[static_cast<std::size_t>(cur.idx)];

        // Skip entries that are stale (we already found a better path) or already closed.
        // This is a common pattern because std::priority_queue has no "decrease-key".
        if (curNode.closed) continue;
        if (cur.g != curNode.g) continue;

        curNode.closed = true;
        if (outExpandedNodes) ++(*outExpandedNodes);

        if (cur.idx == goalIdx) {
            // Reconstruct path by following parent pointers.
            std::vector<PointI> path;
            int walk = goalIdx;
            while (walk != -1) {
                path.push_back(grid.pointFromIndex(walk));
                walk = nodes[static_cast<std::size_t>(walk)].parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        const PointI curPos = grid.pointFromIndex(cur.idx);

        for (int i = 0; i < dirCount; ++i) {
            const PointI d = dirs[static_cast<std::size_t>(i)];
            const PointI next{ curPos.x + d.x, curPos.y + d.y };

            if (!grid.isWalkable(next)) continue;

            const bool isDiag = (d.x != 0 && d.y != 0);

            if (isDiag && cfg.forbidCornerCutting) {
                // requires both adjacent orthogonal steps to be free
                const PointI n1{ curPos.x + d.x, curPos.y };
                const PointI n2{ curPos.x, curPos.y + d.y };
                if (!grid.isWalkable(n1) || !grid.isWalkable(n2)) continue;
            }

            const int nextIdx = grid.index(next);
            Node& nextNode = nodes[static_cast<std::size_t>(nextIdx)];
            if (nextNode.closed) continue;

            const float stepCost = isDiag ? cfg.diagCost : cfg.orthCost;
            const float tentativeG = curNode.g + stepCost;

            if (tentativeG < nextNode.g) {
                nextNode.parent = cur.idx;
                nextNode.g = tentativeG;
                nextNode.h = heuristicCost(next, goal, cfg);
                nextNode.f = nextNode.g + nextNode.h;

                open.push(OpenEntry{ nextIdx, nextNode.f, nextNode.g, nextNode.h });
            }
        }
    }

    return {}; // no path
}

// ---------------------------------
// Debug printing
// ---------------------------------

static void printPathAsAscii(
    const std::vector<std::string>& raw,
    const std::vector<PointI>& path,
    const PointI& start,
    const PointI& goal)
{
    std::vector<std::string> out = raw;
    for (const auto& p : path) {
        if (p == start || p == goal) continue;
        out[static_cast<std::size_t>(p.y)][static_cast<std::size_t>(p.x)] = '*';
    }
    out[static_cast<std::size_t>(start.y)][static_cast<std::size_t>(start.x)] = 'S';
    out[static_cast<std::size_t>(goal.y)][static_cast<std::size_t>(goal.x)] = 'G';

    for (const auto& line : out) {
        std::cout << line << '\n';
    }
}

static void printPathCoordinates(const std::vector<PointI>& path) {
    for (std::size_t i = 0; i < path.size(); ++i) {
        std::cout << '(' << path[i].x << ',' << path[i].y << ')';
        if (i + 1 != path.size()) std::cout << " -> ";
    }
    std::cout << '\n';
}

int main() {
    // '.' = free, '#' = obstacle
    const std::vector<std::string> map = {
        "..........",
        ".####.....",
        ".#..#.....",
        ".#..#..###",
        ".#..#....#",
        ".#.......#",
        ".#######.#",
        "......#..#",
        "......#..#",
        ".........."
    };

    const Grid grid(map);
    const PointI start{ 0, 0 };
    const PointI goal{ 9, 9 };

    // Demo 1: 4-direction, Manhattan
    {
        AStarConfig cfg;
        cfg.movement = Movement::Four;
        cfg.heuristic = Heuristic::Manhattan;
        cfg.orthCost = 1.0f;
        cfg.diagCost = 1.0f; // unused in 4-dir

        std::size_t expanded = 0;
        const auto path = aStarFindPath(grid, start, goal, cfg, &expanded);

        std::cout << "[4-dir + Manhattan] path_len=" << path.size() << ", expanded=" << expanded << "\n";
        if (path.empty()) {
            std::cout << "No path\n\n";
        }
        else {
            printPathAsAscii(map, path, start, goal);
            printPathCoordinates(path);
            std::cout << '\n';
        }
    }

    // Demo 2: 8-direction, Octile, diagCost = sqrt(2)
    {
        AStarConfig cfg;
        cfg.movement = Movement::Eight;
        cfg.heuristic = Heuristic::Octile;
        cfg.orthCost = 1.0f;
        cfg.diagCost = 1.41421356237f; // sqrt(2)
        cfg.forbidCornerCutting = false; // set true if you want to prevent diagonals through corners

        std::size_t expanded = 0;
        const auto path = aStarFindPath(grid, start, goal, cfg, &expanded);

        std::cout << "[8-dir + Octile] path_len=" << path.size() << ", expanded=" << expanded << "\n";
        if (path.empty()) {
            std::cout << "No path\n";
        }
        else {
            printPathAsAscii(map, path, start, goal);
            printPathCoordinates(path);
        }
    }

    return 0;
}
