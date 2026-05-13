#pragma once
#include <vector>
#include <queue>
#include <unordered_set>
#include <map>
#include <SFML/Graphics.hpp>

struct PathNode {
    sf::Vector2i pos;
    int gCost = 0;
    int hCost = 0; 
    int fCost() const { return gCost + hCost; }

    bool operator>(const PathNode& other) const {
        return fCost() > other.fCost();
    }
};

class PathfindingSystem {
public:
    
    static int getDistance(sf::Vector2i a, sf::Vector2i b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    // A* Algoritması
    static std::vector<sf::Vector2i> findPath(sf::Vector2i startPos, sf::Vector2i targetPos, const std::function<bool(int, int)>& isWalkable) {
        std::vector<sf::Vector2i> path;

        
        if (!isWalkable(targetPos.x, targetPos.y)) return path;

        std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
        std::map<int, sf::Vector2i> cameFrom;
        std::map<int, int> gCosts; 

     
        auto getGridId = [](sf::Vector2i p) { return p.y * 1000 + p.x; };

        openSet.push({ startPos, 0, getDistance(startPos, targetPos) });
        gCosts[getGridId(startPos)] = 0;

        sf::Vector2i directions[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} }; // W, S, A, D

        while (!openSet.empty()) {
            PathNode current = openSet.top();
            openSet.pop();

            
            if (current.pos == targetPos) {
                sf::Vector2i curr = targetPos;
                while (curr != startPos) {
                    path.push_back(curr);
                    curr = cameFrom[getGridId(curr)];
                }
                std::reverse(path.begin(), path.end()); 
                return path;
            }

         
            for (auto dir : directions) {
                sf::Vector2i neighbor = current.pos + dir;

                if (!isWalkable(neighbor.x, neighbor.y)) continue;

                int tentativeGCost = gCosts[getGridId(current.pos)] + 1;

                if (gCosts.find(getGridId(neighbor)) == gCosts.end() || tentativeGCost < gCosts[getGridId(neighbor)]) {
                    gCosts[getGridId(neighbor)] = tentativeGCost;
                    cameFrom[getGridId(neighbor)] = current.pos;
                    openSet.push({ neighbor, tentativeGCost, getDistance(neighbor, targetPos) });
                }
            }
        }
        return path; 
    }
};