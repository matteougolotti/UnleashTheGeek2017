#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <memory>
#include <set>

using namespace std;


// Moves generation

void generaterec(int arraySize, int sum, int upper, vector<int>& move){
	if(arraySize <= 0 ){
		return;
	}
	if(sum == 0){
		int zeroes = arraySize ;
		while(zeroes > 0){
			move.push_back(0) ;
			arraySize --;
			zeroes --;
		}
		return;
	}
	else {
		for(int i = upper; i > 0 ; i--){
			if(sum-i >= 0 && arraySize > 0){
				move.push_back(i) ;
				generaterec(arraySize - 1, sum-i, i, move ) ;
				break;
			}
		}
	}
}

void generate(int arraySize, int sum, int upper, vector<vector<int>>& moves) {
	for(int i = upper; i>0 ;i--){
		if(sum-i >= 0){
			vector<int> newMove;
			newMove.push_back(i) ;
			generaterec(arraySize - 1,sum-i, i, newMove );
			if( arraySize > 5 || std::accumulate(newMove.begin(), newMove.end(), 0) == 5){
				moves.push_back(newMove);
			}
		}
	}
}

struct Planet {

	void set(int maximizingUnits, int maximizingTolerance, int minimizingUnits, int minimizingTolerance, int canAssign) {
		_maximizingUnits = maximizingUnits;
		_maximizingTolerance = maximizingTolerance;
		_minimizingUnits = minimizingUnits;
		_minimizingTolerance = minimizingTolerance;
		_canAssign = canAssign;
	}

	bool isMine(bool maximizingPlayer) const {
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
	int _id;
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
			if (_maximizingPlayer) {
				_planets[_spread]._maximizingUnits -= 5;
			}
			else {
				_planets[_spread]._minimizingUnits -= 5;
			}

			for (int planetId : graph[_spread]) {
				if (_maximizingPlayer) {
					_planets[planetId]._maximizingUnits++;
				}
				else {
					_planets[planetId]._minimizingUnits++;
				}
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

		return (minimizingPlanets == 0) || (maximizingPlanets == 0);
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

		return maximizingPlanets - minimizingPlanets;

		/*if (maximizingPlayer) {
			return maximizingPlanets - minimizingPlanets;
		}
		else {
			return minimizingPlanets - maximizingPlanets;
		}*/
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

struct PlanetComp {
	const vector<Planet>& pv;
	const vector<vector<int>>& g;
	bool maxPlayer;

	PlanetComp(const vector<Planet>& planets, const vector<vector<int>>& graph, bool maximizingPlayer)
	: pv(planets), g(graph), maxPlayer(maximizingPlayer) {}

	bool operator() (const Planet* lp, const Planet* rp) const {
		return count_adjacent_not_mine(*lp) > count_adjacent_not_mine(*rp);
	}

	int count_adjacent_not_mine(const Planet& p) const {
		int num = 0;
		const vector<int>& v = g[p._id]; // neighbor planet ids
		for (auto pid : v) {
			if (not pv[pid].isMine(maxPlayer)) {
				++num;
			}
		}
		return num;
	}
};

vector<int> rankPlanets(const vector<int>& filtered_planets, const vector<Planet>& planets, const vector<vector<int>>& graph, bool maximizingPlayer) {
	vector<const Planet*> pv;
	pv.reserve(filtered_planets.size());
	for (int fp : filtered_planets) {
		pv.push_back(&planets[fp]);
	}
	sort(pv.begin(), pv.end(), PlanetComp(planets, graph, maximizingPlayer));

	vector<int> ranks;
	ranks.reserve(pv.size());
	for (const Planet* p : pv) {
		ranks.push_back(p->_id);
	}
	return ranks;
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
		cout << m << endl;
	}

	if (move.second != -1) {
		cout << move.second << endl;
	} else {
		cout << "NONE" << endl;
	}
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

void generatePermsRec(std::vector<int>& baseSolution, std::vector<std::vector<int>>& solutions, int unitsLeft) {
  if (unitsLeft == 0) {
	  solutions.push_back(baseSolution);
	  return;
  }

  for (int i = 0; i < baseSolution.size(); ++i) {
	  baseSolution[i]++;
	  generatePermsRec(baseSolution, solutions, unitsLeft - 1);
	  baseSolution[i]--;
  }
}

void generatePermsInit(std::vector<int>& filteredPlanets, std::vector<std::vector<int>>& solutions) {
  std::vector<int> baseSolution(filteredPlanets.size(), 0);

  std::vector<std::vector<int>> filteredPlanetsIndexesSolutions;
  generatePermsRec(baseSolution, filteredPlanetsIndexesSolutions, 5);

  for (int solution_i = 0; solution_i < filteredPlanetsIndexesSolutions.size(); ++solution_i) {
	  std::vector<int> globalPlanetsSolution;
	  for (int planet_i = 0; planet_i < filteredPlanetsIndexesSolutions[solution_i].size(); ++planet_i) {
          for (int n = 0; n < filteredPlanetsIndexesSolutions[solution_i][planet_i]; ++n) {
        	  globalPlanetsSolution.push_back(filteredPlanets[planet_i]);
          }
	  }
    solutions.push_back(globalPlanetsSolution);
  }
}

void generatePerms(std::vector<int>& prefix, const int length, std::vector<std::vector<int> >& result) {
	const int n = 6;

	if (length < 1) {
		return;
	}

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

void generateMoves(std::vector<Planet>& planets,
	std::vector<std::vector<int> >& graph,
	bool maximizingPlayer,
	std::vector<std::pair<std::vector<int>, int> >& solutions) {

	cerr << "generating moves ..." << endl;

	static int units = 5;
    // std::vector<std::vector<int> > moves;

	std::vector<int> filteredPlanets = filterPlanets(planets, graph, maximizingPlayer);

	std::vector<int> rankedPlanets = rankPlanets(filteredPlanets, planets, graph, maximizingPlayer);

	cerr << "Filtered planets :" << endl;
	for (int planet : rankedPlanets) {
		cerr << planet << endl;
	}

	std::vector<std::vector<int> > permutations;
	std::vector<int> prefix = std::vector<int>();

	// TODO Add to have permutations back
	//generatePerms(prefix, filteredPlanets.size(), permutations);

	/* New solution */
	/*generatePermsInit(filteredPlanets, permutations);
	std::vector<std::vector<int> >& moves = permutations;*/

	// End new solution

	// New New solution

    generate(rankedPlanets.size(), 5, 5, permutations);
    cerr << "Generated " << permutations.size() << " permutations" << endl;
	/* End new new solution */

	// TODO Add to have permutations back
	std::vector<std::vector<int> > moves;
	for (auto permutation : permutations){
		std::vector<int> move ;
		for(int i=0; i< permutation.size() ; i++){
			int units = permutation[i];
			while(units > 0){
				move.push_back(rankedPlanets[i]);
				units--;
			}
		}
		moves.push_back(move);
	}

	cerr << "Generated " << permutations.size() << " moves" << endl;

	// Add spreads for eligible planets
	/*for (int move_i = 0; move_i < moves.size(); ++move_i) {
		std::pair<std::vector<int>, int> moveWithoutSpread = make_pair(moves[move_i], -1);
		solutions.push_back(moveWithoutSpread);

		Turn turn(graph, moveWithoutSpread.first, moveWithoutSpread.second, planets, maximizingPlayer);

		for (int planet_i : filteredPlanets) {
			if (((maximizingPlayer) && (turn._planets[planet_i]._maximizingUnits >= 5))
				|| ((!maximizingPlayer) && (turn._planets[planet_i]._minimizingUnits >= 5))) {

				// create a new pair with spread
				std::pair<std::vector<int>, int> moveWithSpread = std::make_pair(moves[move_i], planet_i);
				solutions.push_back(moveWithSpread);
			}
		}
	}*/

	// New new spread logic

	// figure out the planet
	int spreadPlanet = -1;
	int maxUnits = -1;

	if(maximizingPlayer){

			for (int i = 0; i < planets.size(); i++) {
				auto p = planets[i];
				if ( p._maximizingUnits > maxUnits && p._maximizingUnits - 5 > p._minimizingUnits && graph[i].size() > 5) {
					spreadPlanet = i;
					maxUnits = planets[spreadPlanet]._maximizingUnits;
				}
				}

	}
	else{
			for (int i = 0; i < planets.size(); i++) {
				auto p = planets[i];
				if (p._minimizingUnits > maxUnits && p._minimizingUnits - 5 > p._maximizingUnits  && graph[i].size() > 5) {
					spreadPlanet = i;
					maxUnits = planets[spreadPlanet]._maximizingUnits;
				}
				}

	}


	for (auto move : moves) {
		solutions.push_back(make_pair(move, spreadPlanet));
	}

	// End new new spread logic

	cerr << "Generated " << solutions.size() << " moves" << endl;
}

int minmax(vector<vector<int>>& graph,
	Turn& turn,
	int depth,
	bool maximizingPlayer,
	int alpha,
	int beta) {
	
	if ((depth == 0) || turn.isWinningTurn()) {
		cerr << "Returning minmax from depth " << depth << endl;
		return turn.score(maximizingPlayer);
    }
    
	if (maximizingPlayer) {
		int v = std::numeric_limits<int>::min();
		std::vector<std::pair<std::vector<int>, int> > moves;
		generateMoves(turn._planets, graph, maximizingPlayer, moves);
		for (auto move : moves) {
			Turn nextTurn(graph, move.first, move.second, turn._planets, maximizingPlayer);
			v = max(v, minmax(graph, nextTurn, depth, false, alpha, beta));
			alpha = max(alpha, v);
			if (beta <= alpha) {
				break;
			}
		}
	    return v;
	}
	else {
		int v = std::numeric_limits<int>::max();
		std::vector<std::pair<std::vector<int>, int> > moves;
		generateMoves(turn._planets, graph, maximizingPlayer, moves);
		for (auto move : moves) {
			Turn nextTurn(graph, move.first, move.second, turn._planets, maximizingPlayer);
			v = min(v, minmax(graph, nextTurn, depth - 1, true, alpha, beta));
			beta = min(beta, v);
			if (beta <= alpha) {
				break;
			}
		}
		return v;
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
    	planets.back()._id = i;
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
        
        std::vector<std::pair<std::vector<int>, int> > moves;
		generateMoves(planets, graph, true, moves);
		int best = std::numeric_limits<int>::min();
		int nextMove = 0;
		for (int i = 0; i < moves.size(); ++i) {
			Turn turn(graph, moves[i].first, moves[i].second, planets, true);
			int val = minmax(graph, turn, 1, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
			if (val > best) {
				nextMove = i;
				best = val;
			}
		}

		printMove(moves[nextMove]);
    }
}
