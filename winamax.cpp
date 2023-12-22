#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#define TABLEN 119

enum DIRECTION {
          DOWN = 'v',
          UP = '^',
          LEFT = '<',
          RIGHT = '>',
          NONE = 0
        };


typedef struct BALL {
    int row;
    int col;
    int shotCount;
    bool firstShot = true;
} BALL;

typedef struct NODE {
    bool visited = false;
    char environnement;
    DIRECTION direction = NONE;
    bool balled = false;
}   NODE;

class GolfSolver
{
    
    public:
        typedef bool (GolfSolver::*VisitDirection)(int, int, int, bool);
        static inline bool isBall(char c) { return c >= '1'  && c <= '9'; };
        static inline bool isWaterHazard(char c) { return c == WATER_HAZARD; }
        static inline bool isGrass(char c) { return c == GRASS ; };
        static inline bool isDirection(char c) { return c == DOWN || c == UP || c == LEFT || c == RIGHT;  };
        
        /*Constructor*/
        GolfSolver(const std::vector<std::vector<NODE>>& golfMap, const BALL& ball, int ballCount) {
            this->_ball = ball;
            this -> _golfMap = golfMap ;
            _numCol = _golfMap[0].size();
            _numRow = this->_golfMap.size();
            this->_hole = ballCount;
            _visitOrClean[UP] = &GolfSolver::visitUpOrClearUp;
            _visitOrClean[DOWN] = &GolfSolver::visitDownOrClearDown;
            _visitOrClean[LEFT] = &GolfSolver::visitLeftOrClearLeft;
            _visitOrClean[RIGHT] = &GolfSolver::visitRightOrClearRight;
        };
        
        void printGolfMap(const std::string& visit = "RESULT") {
            for (size_t i = 0; i < this ->_golfMap.size(); i++)
            {
                for (size_t j = 0; j < this->_golfMap[i].size(); j++)
                {
                    char c = this->_golfMap[i][j].direction != NONE ? this->_golfMap[i][j].direction : static_cast<char>(GRASS);
                    std::cout << c;
                }
                std::cout << std::endl;
            }
            
        }


        bool holeEmUp(int row, int col, int shotCount) {

            if (this->_golfMap[row][col].environnement == HOLE) {
                BALL prevBAll = this->_ball;
                this->_golfMap[row][col].balled = true;
                this->_hole--;
                if (this->findNextBall() && (this->_hole
                    && !this->holeEmUp(this->_ball.row, this->_ball.col, this->_ball.shotCount)))
                {
                    this->_ball = prevBAll;
                    this->_hole++;
                    this->_golfMap[row][col].balled = false;
                    return false;
                }
                return true;
            }

            if (this->_hole == 0) return true;

            if (shotCount == 0) return false;

            if (row + shotCount < this->_numRow && (!this->checkIfVisitedAndCanCross(row, col, shotCount, DOWN) &&
                    (holeEmUp(row + shotCount, col, shotCount - 1) || (this->*_visitOrClean[DOWN])(row, col, shotCount, false))))
            {
                return true;
            }

            if (col - shotCount >= 0 && (!this->checkIfVisitedAndCanCross(row, col, shotCount, LEFT) &&
                    (holeEmUp(row, col - shotCount, shotCount - 1) || (this->*_visitOrClean[LEFT])(row, col, shotCount, false))))
            {
                return true;
            }

            if (col + shotCount < this->_numCol && (!this->checkIfVisitedAndCanCross(row, col, shotCount, RIGHT) &&
                    (holeEmUp(row, col + shotCount, shotCount - 1) || (this->*_visitOrClean[RIGHT])(row, col, shotCount, false))))
            {
                return true;
            }
            
            if (row - shotCount >= 0 && (!this->checkIfVisitedAndCanCross(row, col, shotCount, UP) &&
                    (holeEmUp(row - shotCount,  col, shotCount - 1) || (this->*_visitOrClean[UP])(row, col, shotCount, false))))
            {
                return true;
            }

            return false;
        };


    private:
        enum ENVIRONMENT {
            HOLE = 'H',
            WATER_HAZARD = 'X',
            GRASS = '.',
        };

        int _hole = 0;
        int _numCol = 0;
        int _numRow = 0;
        BALL _ball;
        std::vector<std::vector<NODE>> _golfMap;
        std::map<DIRECTION, GolfSolver::VisitDirection> _visitOrClean;

        void updateNode(int row, int col, bool visited, DIRECTION direction) {
            this->_golfMap[row][col].visited = visited;
            this->_golfMap[row][col].direction = direction;
        }

        bool isUnvalidHole(bool stop, int row, int col) {
            return this->_golfMap[row][col].environnement == HOLE && (!stop || this->_golfMap[row][col].balled);
        };

        bool visitDownOrClearDown(int row, int col, int shotCount, bool visit) {
            size_t i = row;
            size_t stop = i + shotCount;

            if (!visit)
            {    
                for (; i < stop; i++)
                {
                    updateNode(i, col, false, NONE);
                }
                
                return false;
            }

            for (; i < stop; i++)
            {
                if (GolfSolver::cantCross(i + 1, col, shotCount, stop == i + 1))
                {
                    for (size_t j = row; j < i; j++)
                    {
                        updateNode(j, col, false, NONE);
                    }
                    return true;
                }
                updateNode(i, col, true, DOWN);
            }

            return false;
        }

        bool visitUpOrClearUp(int row, int col, int shotCount, bool visit) {
            size_t i = row;
            size_t stop = i - shotCount;
            
            if (!visit)
            {    
                for (; i > stop; i--)
                {
                    updateNode(i, col, false, NONE);
                }
                
                return false;
            }

            for (; i > stop; i--)
            {
                if (GolfSolver::cantCross(i - 1, col, shotCount, stop == i - 1))
                {
                    for (size_t j = row; j > i; j--)
                    {
                        updateNode(j, col, false, NONE);
                    }
                    return true;
                }
                updateNode(i, col, true, UP);
            }
            return false;
        }

        bool visitRightOrClearRight(int row, int col, int shotCount, bool visit) {

            size_t i = col;
            size_t stop = i + shotCount;

            if (!visit)
            {    
                for (; i < stop; i++)
                {
                    updateNode(row, i, false, NONE);
                }
                
                return false;
            }


            for (; i < stop; i++)
            {
                if (GolfSolver::cantCross(row, i + 1, shotCount, stop == i + 1))
                {
                    for (size_t j = col; j < i; j++)
                    {
                        updateNode(row, j, false, NONE);
                    }
                    
                    return true;
                }
                updateNode(row, i, true, RIGHT);
            }

            return false;
        }

        bool visitLeftOrClearLeft(int row, int col, int shotCount, bool visit) {

            size_t i = col;

            size_t stop = i - shotCount;

            if (!visit)
            {    
                for (; i > stop; i--)
                {
                    updateNode(row, i, false, NONE);
                }
                
                return false;
            }

            for (; i > stop; i--)
            {
                if (GolfSolver::cantCross(row, i - 1, shotCount, stop == i - 1))
                {
                    for (size_t j = col; j > i; j--)
                    {
                        updateNode(row, j, false, NONE);
                    }
                    return true;
                }
                updateNode(row, i, true, LEFT);
            }
            return false;
        }

        bool checkIfVisitedAndCanCross(int row, int col, int shotCount, DIRECTION direction) {

            if((this->*_visitOrClean[direction])(row, col, shotCount, true))
                return true;

            return  false;
        }

        bool cantCross(int row, int col, int shotCount, bool stop) {
            char c = this->_golfMap[row][col].environnement;
            return this->_golfMap[row][col].visited
                    || isBall(c)
                    || GolfSolver::isDirection(c)
                    || (GolfSolver::isUnvalidHole(stop, row, col))
                    || (stop && (GolfSolver::isWaterHazard(c)));
        }

        bool findNextBall() 
        {
            size_t j = this->_ball.col + 1;
            for (size_t i = this->_ball.row; i < this->_golfMap.size(); i++)
            {
                for (; j < this->_golfMap[0].size(); j++)
                {
                    if (GolfSolver::isBall(this->_golfMap[i][j].environnement)){
                        this->_ball.row = i;
                        this->_ball.col = j;
                        this->_ball.shotCount = this->_golfMap[i][j].environnement - '0';
                        return true;
                    }
                }
                j = 0;
            }

            return true;
        }

};

int main()
{
    int width;
    int height;
    std::cin >> width >> height; std::cin.ignore();
    int  ballCount = 0;
    bool foundFirstBall = false;
    std::vector<std::vector<NODE>> golf(height);
    NODE env;
    BALL ball; 
    for (int i = 0; i < height; i++) {
        std::string row;
        std::cin >> row; std::cin.ignore();
        golf[i].resize(width);
        for (size_t j = 0; j < width; j++)
        {
            if (GolfSolver::isBall(row[j])) {
                if (!foundFirstBall) 
                {
                    ball.row = i;
                    ball.col = j;
                    ball.shotCount = row[j] - '0';
                    foundFirstBall = true;
                }
                ballCount++;
            }
            golf[i][j].environnement = row[j];
        }
        
    }
    GolfSolver golfLair(golf, ball, ballCount);
    golfLair.holeEmUp(ball.row, ball.col, ball.shotCount);
    golfLair.printGolfMap();
}