#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <memory>
#include <set>

using namespace std;

bool isMyTurn = true;

struct Planet {

	void set(int myUnits, int myTolerance, int otherUnits, int otherTolerance, int canAssign) {
		_myUnits = myUnits;
		_myTolerance = myTolerance;
		_otherUnits = otherUnits;
		_otherTolerance = otherTolerance;
		_canAssign = canAssign;
	}

	bool isMine() {
		if (isMyTurn) {
			return (_myUnits > _otherUnits);
		} else {
			return (_otherUnits > _myUnits);
		}
	}

	int _myUnits;
	int _myTolerance;
	int _otherUnits;
	int _otherTolerance;
	int _canAssign;
};

ostream& operator<<(ostream& str, const Planet& p) {
	str << "myUnits: " << p._myUnits << ", myTol: " << p._myTolerance
		<< ", otherUnits: " << p._otherUnits << ", otherTol: " << p._otherTolerance << ", canAssign: " << p._canAssign;
	return str;
}

struct Turn {

	int _wins;
	int _losses;
	std::shared_ptr<Turn> _parent;
	bool _myTurn;
	bool _localWin;
	bool _localLoss;
	int _isSpread;
	std::vector<int> _myMoves;
	std::vector<Planet> _planets;
	std::vector<std::shared_ptr<Turn> > _children;

	Turn() : _wins(0), _losses(0), _parent(NULL), _localWin(false), _localLoss(false), _myTurn(false), _isSpread(-1) {}

	void setWin() {
		++_wins;
		if (_parent != NULL) {
			_parent->setWin();
		}
	}

	void setLoss() {
		++_losses;
		if (_parent != NULL) {
			_parent->setLoss();
		}
	}

	void set(const std::vector<std::vector<int>>& graph,
			const std::vector<int>& myMoves,
			int mySpread,
			std::shared_ptr<Turn>& parent,
			std::vector<Planet>& planets,
			bool myTurn) {

		_parent = parent;
		_planets = planets;
		_isSpread= mySpread;
		_myMoves.assign(myMoves.begin(), myMoves.end());

		// TODO Add my units
		for (int i = 0; i < myMoves.size(); ++i) {
            _planets[myMoves[i]]._myUnits++;
		}

		// TODO Update my tolerance
		for (int i = 0; i < _planets.size(); ++i) {
			if (std::find(myMoves.begin(), myMoves.end(), i) != myMoves.end()) {
				_planets[i]._myTolerance--;
			}
		}

		// TODO My spread
		if (mySpread != -1) {
			_planets[mySpread]._myUnits -= 5;
			for (int planetId : graph[mySpread]) {
				_planets[planetId]._myUnits++;
			}
		}
	}

	void printMove() {
		 for (int i = 0; i < _myMoves.size(); ++i) {
				 cout << _myMoves[i] << endl;
			 }
		 if( _isSpread != -1){
			 cout << _isSpread << endl;

		 }else{
			 cout << "NONE" << endl;
		 	 }
		  }
};

void printGraph(std::vector<std::vector<int> >& graph) {
  for (int i = 0; i < graph.size(); ++i) {
	  cerr << i << " -> ";
	  for (int neighbor : graph[i]) {
		  cerr << neighbor << " ";
	  }
	  cerr << endl;
  }
}

void printPlanets(const std::vector<Planet>& planets) {
  for (auto p : planets) {
	  cerr << p << endl;
  }
}

void printMove(pair<vector<int>, int>& move) {
	for(auto m : move.first) {
		cerr << m << " ";
	}
	cerr << endl;
}

std::shared_ptr<Turn> solutionsTree = std::make_shared<Turn>();


bool isNearControlled(int planetId, const vector<Planet>& pv, const vector<vector<int>>& graph) {

	auto neighbors = graph[planetId];
	int controlled = 0;
	if (isMyTurn) {
		for (auto n : neighbors) {
			if (pv[n]._myUnits > pv[n]._otherUnits) {
				++controlled;
			}
		}
	}else{
		for (auto n : neighbors) {
			if (pv[n]._otherUnits > pv[n]._myUnits) {
				++controlled;
			}
		}
	}
	return (controlled != neighbors.size()) and (controlled > 0);
}

vector<int> filterPlanets(const vector<Planet>& planets, const vector<vector<int>>& graph) {
	//printPlanets(planets);

	set<int> filteredPlanets;
	if (isMyTurn) {

		for (int i = 0; i < planets.size(); ++i) {
			auto p = planets[i];

			// Check that this planet is tolerant and is adjacent to one of my controlled planets.
			bool isAdj = isNearControlled(i, planets, graph);
			if ((p._myTolerance > 0) and isAdj) {
				filteredPlanets.insert(i);
			}
		}
	} else {

		for (int i = 0; i < planets.size(); ++i) {
			auto p = planets[i];

			// Check that this planet is tolerant and is adjacent to one of my controlled planets.
			bool isAdj = isNearControlled(i, planets, graph);
			if ((p._otherTolerance > 0) and isAdj) {
				filteredPlanets.insert(i);
			}
		}

	}

	return std::move(vector<int>(filteredPlanets.begin(), filteredPlanets.end()));
}

//std::vector<int> filterPlanets(std::vector<Planet>& planets, std::vector<std::vector<int> >& graph) {
//
//	printGraph(graph);
//	printPlanets(planets);
//
//	set<int> filteredPlanets ;
//	if (isMyTurn) {
//		for (int i = 0; i < planets.size(); ++i) {
//			if( planets[i]._myTolerance > 0 ){
//				if((planets[i]._myUnits >= planets[i]._otherUnits) and (planets[i]._myUnits > 0)){
//					bool isNeighborEnemy = false;
//					for (int neighbor_i : graph[i]) {
//					  if (planets[neighbor_i]._otherUnits >= planets[neighbor_i]._myUnits) {
//						  isNeighborEnemy = true;
//						  filteredPlanets.insert(neighbor_i);
//					  }
//					}
//					if (isNeighborEnemy) {
//						filteredPlanets.insert(i);
//					}
//				}
//			}
//		}
//	} else {
//		for (int i = 0; i < planets.size(); ++i) {
//			if( planets[i]._otherTolerance > 0 ){
//				if((planets[i]._otherUnits >= planets[i]._myUnits)){
//					bool isNeighborEnemy = false;
//					for (int neighbor_i : graph[i]) {
//						if (planets[neighbor_i]._myUnits >= planets[neighbor_i]._otherUnits) {
//							isNeighborEnemy = true;
//							filteredPlanets.insert(neighbor_i);
//						}
//					}
//					if (isNeighborEnemy) {
//						filteredPlanets.insert(i);
//					}
//				}
//			}
//		}
//	}
//
//	return vector<int>(filteredPlanets.begin(), filteredPlanets.end());
//}

void generatePerms(std::vector<int>& prefix, const int length, std::vector<std::vector<int> >& result) {
	const int n = 6;
	if (length == 1) {
		for (int j = 0; j < n; j++) {
			std::vector<int> final(prefix);
			final.push_back(j);

			if( std::accumulate(final.begin(), final.end(), 0) == 5 ) {
				result.push_back(final);
			}
		}
	}//Base case: length = 1, print the string "lenght" times + the remaining letter
	else {
		// One by one add all characters from "str" and recursively call for "lenght" equals to "lenght"-1
		for (int i = 0; i < n; i++) {

			// Next character of input added
			std::vector<int> newvector(prefix);
			newvector.push_back(i);

			if( std::accumulate(newvector.begin(), newvector.end(), 0) <= 5 ) {
				generatePerms( newvector , length - 1, result);  // "lenght" is decreased, because we have added a new character
			}
		}
	}
}

std::vector<std::pair<std::vector<int>, int> > generateMoves(std::vector<Planet>& planets, std::vector<std::vector<int> >& graph) {
	cerr << "generating moves ..." << endl;

	static int units = 5;
	std::vector<std::vector<int> > moves;

	std::vector<std::pair<std::vector<int>, int> > movesGenerated;

	std::vector<int> filteredPlanets = filterPlanets(planets, graph);

	cerr << "Filtered planets :" << endl;
	for (int planet : filteredPlanets) {
		cerr << planet << endl;
	}

	std::vector<std::vector<int> > permutations;
	std::vector<int> prefix = std::vector<int>();

	generatePerms(prefix, filteredPlanets.size(), permutations);

	for (auto permutation : permutations){
		std::vector<int> move ;
		for(int i=0; i< permutation.size() ; i++){
			int units = permutation[i];
			while(units > 0){
				move.push_back(filteredPlanets[i]);
				units--;
			}
		}
		moves.push_back(move);
	}

	cerr << "Generated " << permutations.size() << " moves" << endl;

	// Add spreads for eligible planets
	for (int move_i = 0; move_i < moves.size(); ++move_i) {
		std::pair<std::vector<int>, int> moveWithoutSpread = make_pair(moves[move_i],-1);
		movesGenerated.push_back(moveWithoutSpread);

		for (int planet_i = 0; planet_i < planets.size(); ++planet_i) {
			if (((isMyTurn) && (planets[planet_i]._myUnits > 5))
					|| ((!isMyTurn) && (planets[planet_i]._otherUnits > 5))) {

				// create a new pair with spread
				std::pair<std::vector<int>, int> moveWithSpread = std::make_pair(moves[move_i],planet_i );
				movesGenerated.push_back(moveWithSpread);
			}
		}
	}

	cerr << "Generated " << movesGenerated.size() << " moves" << endl;

	return movesGenerated;
}

void buildSolutions(std::shared_ptr<Turn>& prevTurn, std::vector<Planet>& planets, std::vector<std::vector<int> >& graph, int turnsLeft) {

	// Decrement each planet that has a majority enemy neighbors
	if (!isMyTurn) {
		turnsLeft--;

		vector<int> myDecrementingPlanets;
		vector<int> otherDecrementingPlanets;
		for (int planetId = 0; planetId < planets.size(); ++planetId) {
			const vector<int>& neighbors = graph[planetId];
			int myControlled = 0;
			int otherControlled = 0;
			for (int neighborId = 0; neighborId < neighbors.size(); ++neighborId) {
				const Planet& neighbor = planets[neighborId];
				if (neighbor._myUnits > neighbor._otherUnits) {
					++myControlled;
				} else if (neighbor._myUnits < neighbor._otherUnits) {
					++otherControlled;
				}
			}
			if (myControlled < otherControlled) {
				myDecrementingPlanets.push_back(planetId);
			} else if (myControlled > otherControlled) {
				otherDecrementingPlanets.push_back(planetId);
			}
		}
		for (int dp : myDecrementingPlanets) {
			Planet& p = planets[dp];
			if (p._myUnits > 0) { --p._myUnits; }
		}
		for (int dp : otherDecrementingPlanets) {
			Planet& p = planets[dp];
			if (p._otherUnits > 0) { --p._otherUnits; }
		}

		// Check if this is a final winning/losing turn.
		int myPlanets = 0;
		int otherPlanets = 0;
		for (Planet& p : planets) {
			if (p._myUnits > p._otherUnits) ++myPlanets;
			else if (p._myUnits < p._otherUnits) ++otherPlanets;
		}

		if (otherPlanets == 0) { prevTurn->setWin(); cerr << "Winning solution found at depth " << turnsLeft << endl; return; }
		if (myPlanets == 0) { prevTurn->setLoss(); cerr << "Losing solution found at depth " << turnsLeft << endl; return; }

		if (turnsLeft == 0) {
			if (otherPlanets > myPlanets) { prevTurn->setWin(); cerr << "Winning solution found at depth " << turnsLeft << endl; }
			if (myPlanets > otherPlanets) { prevTurn->setLoss(); cerr << "Losing solution found at depth " << turnsLeft << endl; }
			return;
		}
	}

	isMyTurn = !isMyTurn;
	std::vector<std::pair<std::vector<int>, int> > moves = generateMoves(planets, graph);
	for (auto move : moves) {
		std::shared_ptr<Turn> turn = std::make_shared<Turn>();
		turn->set(graph, move.first, move.second, prevTurn, planets, isMyTurn);
		prevTurn->_children.push_back(turn);

		buildSolutions(turn, planets, graph, turnsLeft);
	}
}

void buildSolutionsRoot(std::vector<Planet>& planets, std::vector<std::vector<int> >& graph) {
	cerr << "Calculating solutions from root" << endl;

	std::shared_ptr<Turn> firstTurn = solutionsTree;
	std::vector<std::pair<std::vector<int>, int> > moves = generateMoves(planets, graph);
    for (auto move : moves) {

    	printMove(move);

    	std::shared_ptr<Turn> turn = std::make_shared<Turn>();

    	cerr << "before set" << endl;

    	turn->set(graph, move.first, move.second, firstTurn, planets, true);

    	cerr << "Before pushback" << endl;

    	firstTurn->_children.push_back(turn);
    	isMyTurn = false;

    	cerr << "Before build solution" << endl;

    	// Graph depth planets.size() - 1 for full solution
    	buildSolutions(turn, planets, graph, 2);

    }
}

void calculateTargets(const vector<Planet>& planets,
		              const vector<vector<int>>& graph,
					  vector<int>& targets,
					  int& spreadPlanet) {

}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    vector<vector<int>> graph;

    int planetCount;
    int edgeCount;
    cin >> planetCount >> edgeCount; cin.ignore();
    graph.resize(planetCount);
    for (int i = 0; i < edgeCount; i++) {
        int planetA;
        int planetB;
        cin >> planetA >> planetB; cin.ignore();

        // Double linkage
        graph[planetA].push_back(planetB);
        graph[planetB].push_back(planetA);
    }

    // Do we need to do ensure uniqueness?

    // All the Planets
    std::vector<Planet> planets;
    planets.reserve(planetCount);
    for (int i = 0; i < planetCount; ++i) {
    	planets.push_back(Planet());
    }

    // The IDs of target planets during a turn
    std::vector<int> targets(5, 0); // Initialized to target only planet 0

    // Unit spread planet (-1 if none)
    int spreadPlanet = -1;

    bool init = false;

	shared_ptr<Turn> currentTurn = solutionsTree;

    // game loop
    while (1) {

        for (int i = 0; i < planetCount; ++i) {
            int myUnits;
            int myTolerance;
            int otherUnits;
            int otherTolerance;
            int canAssign;
            cin >> myUnits >> myTolerance >> otherUnits >> otherTolerance >> canAssign; cin.ignore();

            planets[i].set(myUnits, myTolerance, otherUnits, otherTolerance, canAssign);
        }

        if (not init) {
        	buildSolutionsRoot(planets, graph);
        	init = true;
        }

        //calculateTargets(planets, graph, targets, spreadPlanet);
        int maxGain = 0;
        shared_ptr<Turn> bestSolution = currentTurn->_children[0];
        for (shared_ptr<Turn>& solution : currentTurn->_children) {
        	if (solution->_wins - solution->_losses > maxGain) {
              bestSolution = solution;
        	}
        }

        bestSolution->printMove();

        currentTurn = bestSolution;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

//        for (int i = 0; i < 5; ++i) {
//        	cout << targets[i] << endl;
//        }
//
//        if (spreadPlanet == -1) {
//            cout << "NONE" << endl;
//        } else {
//        	cout << spreadPlanet << endl;
//        }
    }
}
