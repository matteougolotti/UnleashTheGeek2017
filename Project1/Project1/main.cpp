#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <memory>
#include <set>

using namespace std;

struct Planet {

	void set(int maximizingUnits, int maximizingTolerance, int minimizingUnits, int minimizingTolerance, int canAssign) {
		_maximizingUnits = maximizingUnits;
		_maximizingTolerance = maximizingTolerance;
		_minimizingUnits = minimizingUnits;
		_minimizingTolerance = minimizingTolerance;
		_canAssign = canAssign;
	}

	bool isMine(bool maximizingPlayer) {
		if (maximizingPlayer) {
			return (_maximizingUnits > _minimizingUnits);
		} else {
			return (_minimizingUnits > _maximizingUnits);
		}
	}

	int _maximizingUnits;
	int _maximizingTolerance;
	int _minimizingUnits;
	int _minimizingTolerance;
	int _canAssign;
};

ostream& operator<<(ostream& str, const Planet& p) {
	str << "myUnits: " << p._maximizingUnits << ", myTol: " << p._maximizingTolerance
		<< ", otherUnits: " << p._minimizingUnits << ", otherTol: " << p._minimizingTolerance << ", canAssign: " << p._canAssign;
	return str;
}

struct Turn {
	bool _maximizingPlayer;
	int _spread;
	std::vector<int> _moves;
	std::vector<Planet> _planets;

	Turn(const std::vector<std::vector<int>>& graph,
		const std::vector<int>& moves,
		int spread,
		std::vector<Planet>& planets,
		bool maximizingPlayer)
	: _moves(moves),
	_spread(spread),
	_planets(planets),
	_maximizingPlayer(maximizingPlayer) {

		// Add units
		for (int i = 0; i < moves.size(); ++i) {
			if (_maximizingPlayer) {
				_planets[moves[i]]._maximizingUnits++;
			}
			else {
				_planets[moves[i]]._minimizingUnits++;
			}
		}

		// Update tolerance
		for (int i = 0; i < _planets.size(); ++i) {
			if (std::find(moves.begin(), moves.end(), i) != moves.end()) {
				if (_maximizingPlayer) {
					_planets[i]._maximizingTolerance--;
				}
				else {
					_planets[i]._minimizingTolerance--;
				}
			}
		}

		// My spread
		if (_spread != -1) {
			_planets[_spread]._maximizingUnits -= 5;
			for (int planetId : graph[_spread]) {
				_planets[planetId]._maximizingUnits++;
			}
		}

	}

	bool isWinningTurn() {
		int maximizingPlanets = 0;
		int minimizingPlanets = 0;

		for (Planet& planet : _planets) {
			if (planet.isMine(true)) {
				maximizingPlanets++;
			}
			else if (planet.isMine(false)) {
				minimizingPlanets++;
			}
		}

		if (_maximizingPlayer) {
			return (minimizingPlanets == 0);
		} else {
			return (maximizingPlanets == 0);
		}

		/*if (_maximizingPlayer) {
			return (maximizingPlanets > minimizingPlanets);
		}
		else {
			return (maximizingPlanets < minimizingPlanets);
		}*/
	}

	int score(bool maximizingPlayer) {
		int maximizingPlanets = 0;
		int minimizingPlanets = 0;

		for (Planet& planet : _planets) {
			if (planet.isMine(true)) {
				maximizingPlanets++;
			}
			else if (planet.isMine(false)) {
				minimizingPlanets++;
			}
		}

		// return maximizingPlanets - minimizingPlanets;

		if (maximizingPlayer) {
			return maximizingPlanets - minimizingPlanets;
		}
		else {
			return minimizingPlanets - maximizingPlanets;
		} 
	}

	void printMove() {
		 for (int i = 0; i < _moves.size(); ++i) {
				 cout << _moves[i] << endl;
			 }
		 if( _spread != -1){
			 cout << _spread << endl;

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

bool isNearControlled(int planetId, const vector<Planet>& pv, 
	const vector<vector<int>>& graph,
	bool maximizingPlayer) {

	auto neighbors = graph[planetId];
	int controlled = 0;
	if (maximizingPlayer) {
		for (auto n : neighbors) {
			if (pv[n]._maximizingUnits > pv[n]._minimizingUnits) {
				++controlled;
			}
		}
	} else {
		for (auto n : neighbors) {
			if (pv[n]._minimizingUnits > pv[n]._maximizingUnits) {
				++controlled;
			}
		}
	}
	return (controlled != neighbors.size()) && (controlled > 0);
}

vector<int> filterPlanets(const vector<Planet>& planets, 
	const vector<vector<int>>& graph,
	bool maximizingPlayer) {
	//printPlanets(planets);

	set<int> filteredPlanets;
	if (maximizingPlayer) {
		for (int i = 0; i < planets.size(); ++i) {
			auto p = planets[i];

			// Check that this planet is tolerant and is adjacent to one of my controlled planets.
			bool isAdj = isNearControlled(i, planets, graph, maximizingPlayer);
			if ((p._maximizingTolerance > 0) && isAdj) {
				filteredPlanets.insert(i);
			}
		}
	} else {
		for (int i = 0; i < planets.size(); ++i) {
			auto p = planets[i];

			// Check that this planet is tolerant and is adjacent to one of my controlled planets.
			bool isAdj = isNearControlled(i, planets, graph, maximizingPlayer);
			if ((p._minimizingTolerance > 0) && isAdj) {
				filteredPlanets.insert(i);
			}
		}
	}

	return std::move(vector<int>(filteredPlanets.begin(), filteredPlanets.end()));
}

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

std::vector<std::pair<std::vector<int>, int> > generateMoves(std::vector<Planet>& planets, 
	std::vector<std::vector<int> >& graph,
	bool maximizingPlayer) {
	cerr << "generating moves ..." << endl;

	static int units = 5;
	std::vector<std::vector<int> > moves;

	std::vector<std::pair<std::vector<int>, int> > movesGenerated;

	std::vector<int> filteredPlanets = filterPlanets(planets, graph, maximizingPlayer);

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
			if (((maximizingPlayer) && (planets[planet_i]._maximizingUnits > 5))
					|| ((!maximizingPlayer) && (planets[planet_i]._minimizingUnits > 5))) {

				// create a new pair with spread
				std::pair<std::vector<int>, int> moveWithSpread = std::make_pair(moves[move_i],planet_i );
				movesGenerated.push_back(moveWithSpread);
			}
		}
	}

	cerr << "Generated " << movesGenerated.size() << " moves" << endl;

	return movesGenerated;
}

int minmax(vector<vector<int>>& graph,
	Turn& turn,
	int depth,
	bool maximizingPlayer) {
	
	if ((depth == 0) || turn.isWinningTurn()) {
		return turn.score(maximizingPlayer);
    }
    
	if (maximizingPlayer) {
		int bestValue = std::numeric_limits<int>::min();
		auto moves = generateMoves(turn._planets, graph, maximizingPlayer);
		for (auto move : moves) {
			Turn nextTurn(graph, move.first, move.second, turn._planets, maximizingPlayer);
			auto v = minmax(graph, nextTurn, depth, false);
			bestValue = max(bestValue, v);
			return bestValue;
		}
	}
	else {
		int bestValue = std::numeric_limits<int>::max();
		auto moves = generateMoves(turn._planets, graph, maximizingPlayer);
		for (auto move : moves) {
			Turn nextTurn(graph, move.first, move.second, turn._planets, maximizingPlayer);
			auto v = minmax(graph, nextTurn, depth - 1, true);
			bestValue = min(bestValue, v);
			return bestValue;
		}
	}
}

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

    // All the Planets
    std::vector<Planet> planets;
    planets.reserve(planetCount);
    for (int i = 0; i < planetCount; ++i) {
    	planets.push_back(Planet());
    }

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
        
		auto moves = generateMoves(planets, graph, true);
		int best = std::numeric_limits<int>::min();
		int nextMove = 0;
		for (int i = 0; i < moves.size(); ++i) {
			Turn turn(graph, moves[i].first, moves[i].second, planets, true);
			int val = minmax(graph, turn, planets.size(), false);
			if (val > best) {
				nextMove = i;
				best = val;
			}
		}

		printMove(moves[nextMove]);
    }
}
