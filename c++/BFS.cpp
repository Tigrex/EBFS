#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <list>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::list;
using std::vector;

struct Edge {
public:
	int fromIndex;
	int toIndex;
	int departure;
	int arrival;

	int index;

	bool operator==(const Edge &anotherEdge) const { 
        return (fromIndex == anotherEdge.fromIndex && toIndex == anotherEdge.toIndex && departure == anotherEdge.departure && arrival == anotherEdge.arrival);
    }
    
};

namespace std
{
    template <>
    struct hash<Edge>
    {
        size_t operator()(const Edge& e) const {

        
	    	int hash = e.fromIndex;
			hash *= 37;
			hash += e.toIndex;
			hash *= 37;
			hash += e.departure;
			hash *= 37;
			hash += e.arrival;

			return (hash);
	    }

    };
}


// berlin
string fileName = "berlin.txt";
const int NUM_OF_VERTICES = 12746;
const int NUM_OF_EDGES = 1209980;

// wikipedia-growth
// string fileName = "out.wikipedia-growth";
// const int NUM_OF_VERTICES = 1870709;
// const int NUM_OF_EDGES = 39953145;

// munmun_digg_reply
// string fileName = "out.munmun_digg_reply";
// const int NUM_OF_VERTICES = 30360;
// const int NUM_OF_EDGES = 86203;

// loans
// string fileName = "out.prosper-loans";
// const int NUM_OF_VERTICES = 89269;
// const int NUM_OF_EDGES = 3343284;

// digg-friends
// string fileName = "out.digg-friends";
// const int NUM_OF_VERTICES = 279630;
// const int NUM_OF_EDGES = 1731653;





int numOfRuns = 1000;
int departureTime = 0;
bool printResult = false;

bool preprocessed = true;


int main() {


	vector<Edge> edges;

	/*
	********************************************************************************************************
	Read data file
	********************************************************************************************************
	*/
	cout << "Reading data file..." << endl;

	std::ifstream input("e:/data/" + fileName + ".edges");
	string line;
	while (std::getline(input, line)) {
		std::istringstream iss(line);

		int fromIndex;
		int toIndex;
		int departure;
		int arrival;

		if (!(iss >> fromIndex >> toIndex >> departure >> arrival)) {
			break;
		}

		// Add edge
		Edge e;
		e.fromIndex = fromIndex;
		e.toIndex = toIndex;
		e.departure = departure;
		e.arrival = arrival;

		edges.push_back(e);
		

	}
	input.close();
	


	/*
	********************************************************************************************************
	Update edge index
	********************************************************************************************************
	*/

	for (int i = 0; i < NUM_OF_EDGES; i++) {
		Edge* e = &(edges.at(i));
		e->index = i;

		// Need to assign it back..
		//edges[i] = e;
	}


	/*
	********************************************************************************************************
	Build outgoing edges map
	********************************************************************************************************
	*/

	unordered_map<int, unordered_map<int, vector<Edge>>> outgoingMap;	

	for (int i = 0; i < NUM_OF_EDGES; i++) {
		Edge e = edges.at(i);


		auto searchFrom = outgoingMap.find(e.fromIndex);
		if (searchFrom != outgoingMap.end()) {

			unordered_map<int, vector<Edge>>* map = &(searchFrom->second);

			auto searchTo = map->find(e.toIndex);
			if (searchTo != map->end()) {
				vector<Edge>* outgoingEdges = &(searchTo->second);

				outgoingEdges->push_back(e);
				// Need to assign it back
				// map*[e.toIndex] = outgoingEdges;

			} else {
				vector<Edge> outgoingEdges;

				outgoingEdges.push_back(e);
				(*map)[e.toIndex] = outgoingEdges;

			}

			// Need to assign it back
			// outgoingMap[e.fromIndex] = map;

		} else {
			vector<Edge> outgoingEdges;
			outgoingEdges.push_back(e);

			unordered_map<int, vector<Edge>> map;
			map[e.toIndex] = outgoingEdges;

			outgoingMap[e.fromIndex] = map;
		}

	}

	struct sort_by_departure {
		bool operator() (Edge& lhs, Edge& rhs) { return lhs.departure < rhs.departure; }
	};

	int totalNumOfEdges = 0;

	for (unordered_map<int, unordered_map<int, vector<Edge>>>::iterator it = outgoingMap.begin(); it != outgoingMap.end(); ++it) {

		int fromIndex = it->first;
		unordered_map<int, vector<Edge>>* map = &(it->second);

		for (unordered_map<int, vector<Edge>>::iterator it2 = map->begin(); it2 != map->end(); ++it2) {

			int toIndex = it2->first;
			vector<Edge>* outgoingEdges = &(it2->second);
			
			std::sort(outgoingEdges->begin(), outgoingEdges->end(), sort_by_departure());

			// Need to assign it back....
			// map[toIndex] = outgoingEdges;
			// outgoingMap[fromIndex] = map;

			totalNumOfEdges += outgoingEdges->size();
		}

	}

	cout << "The total number of edges in the outgoing map: " << totalNumOfEdges << endl;



	/*
	********************************************************************************************************
	Build earlist catch dependency
	********************************************************************************************************
	*/

	unordered_map<Edge, vector<Edge>> depedencies;

	if (preprocessed) {

		for (int i = 0; i < NUM_OF_EDGES; i++) {
			Edge e = edges.at(i);

			auto searchFrom = outgoingMap.find(e.toIndex);
			if (searchFrom == outgoingMap.end()) {
				continue;
			}

			unordered_map<int, vector<Edge>> map = searchFrom->second;

			for (unordered_map<int, vector<Edge>> ::iterator it = map.begin(); it != map.end(); ++it) {

				vector<Edge> outgoingEdges = it->second;

				for (vector<Edge>::iterator it2 = outgoingEdges.begin(); it2 != outgoingEdges.end(); ++it2) {

					Edge edge = *it2;

					if (edge.departure >= e.arrival) {

						auto search1 = depedencies.find(e);
						if (search1 != depedencies.end()) {

							vector<Edge>* edges = &(search1->second);

							edges->push_back(edge);

							// Need to assign it back
							// depedencies[e] = edges;

						} else {
							vector<Edge> edges;
							edges.push_back(edge);

							depedencies[e] = edges;
						}	

						break;
					}


				}

				
			}


		}


		int totalDependencies = 0;
		for (unordered_map<Edge, vector<Edge>> ::iterator it = depedencies.begin(); it != depedencies.end(); ++it) {
			vector<Edge> edges = it->second;

			totalDependencies += edges.size();
		}

		cout << "Total number of dependencies in dependency map: " << totalDependencies << endl;


	}




	/*
	********************************************************************************************************
	Use arrays to store dependencies
	********************************************************************************************************
	*/

	vector<vector<Edge>> dependenciesArray;

	if (preprocessed) {

		for (int i = 0; i < NUM_OF_EDGES; i++) {
			Edge e = edges.at(i);

			auto search = depedencies.find(e);
			if (search != depedencies.end()) {

				vector<Edge> v = search->second;
				dependenciesArray.push_back(v);

			} else {
				dependenciesArray.push_back(vector<Edge>());

			}

		}

		int totalDependencies = 0;

		for (int i = 0; i < NUM_OF_EDGES; i++) {
			vector<Edge> v = dependenciesArray.at(i);
		
			totalDependencies += v.size();
		}

		cout << "Total number of dependencies in dependency array: " << totalDependencies << endl;

	}


	


	/*
	********************************************************************************************************
	Get shortest paths -- bfs
	********************************************************************************************************
	*/

	std::cout << endl;


	int* labels = new int[NUM_OF_VERTICES];

	int* visitedEdges = new int[NUM_OF_EDGES];

	int imax = std::numeric_limits<int>::max();

	
	auto start = std::chrono::system_clock::now();

	for (int i = 0; i < numOfRuns; i++) {

		// Initialize labels
		for (int j = 0; j < NUM_OF_VERTICES; j++) {
			labels[j] = imax;
		}

		// Initialize visited
		for (int j = 0; j < NUM_OF_EDGES; j++) {
			visitedEdges[j] = 0;
		}


		//labels[NUM_OF_VERTICES - 1 - i] = departureTime;		
		labels[i] = departureTime;		

		vector<Edge> current;
		vector<Edge> next;


		// Add the first level
		//auto searchFrom = outgoingMap.find(NUM_OF_VERTICES - 1 - i);
		auto searchFrom = outgoingMap.find(i);
		if (searchFrom != outgoingMap.end()) {

			unordered_map<int, vector<Edge>> map = searchFrom->second;

			for (unordered_map<int, vector<Edge>> ::iterator it = map.begin(); it != map.end(); ++it) {

				vector<Edge> outgoingEdges = it->second;

				for (vector<Edge>::iterator it2 = outgoingEdges.begin(); it2 != outgoingEdges.end(); ++it2) {
					Edge e = *it2;

					if (e.departure >= departureTime) {
						current.push_back(e);
						break;
					}

				}

			}

		}


		while (current.size() > 0) {

			// Scan current
			for (auto it = current.begin(); it != current.end(); ++it) {
				Edge e = *it;

				// Update visited set
				if (visitedEdges[e.index] == 1) {
					continue;
				}

				visitedEdges[e.index] = 1;

				if (e.arrival >= labels[e.toIndex]) {
					continue;
				}

				labels[e.toIndex] = e.arrival;

				// Add next bfs level

				if (preprocessed) {

					vector<Edge> edges = dependenciesArray.at(e.index);

					if (edges.size() == 0) {
						continue;
					} 

					for (vector<Edge>::iterator it = edges.begin(); it != edges.end(); ++it) {

						Edge nextE = *it;

						if (nextE.departure >= e.arrival) {

							if (visitedEdges[nextE.index] == 0) {

								// if (next.find(nextE) == next.end()) {

									next.push_back(nextE);	

								// }
								
							}

						}

					}

				} else {
					auto searchFrom = outgoingMap.find(e.toIndex);
					if (searchFrom == outgoingMap.end()) {
						continue;
					}

					int deadline = e.arrival;

					unordered_map<int, vector<Edge>> map = searchFrom->second;

					for (unordered_map<int, vector<Edge>> ::iterator it2 = map.begin(); it2 != map.end(); ++it2) {

						vector<Edge> outgoingEdges = it2->second;

						if (outgoingEdges.size() == 0) {
							continue;
						}

						int left = 0;
						int right = outgoingEdges.size() - 1;
						int middle;

						Edge* candidate = NULL;

						while (left <= right) {
							middle = (left + right) / 2;

							Edge* nextE = &outgoingEdges[middle];

							if (nextE->departure >= deadline) {
								candidate = nextE;
								right = middle - 1;
							} else {
								left = middle + 1;
							}

						}

						if (candidate != NULL && visitedEdges[candidate->index] == 0) {
							next.push_back(*candidate);
						}


					}

				}

			
			}

			
			current = next;

			next.clear();


		}

		
		if (printResult) {
			int numReachable = 0;
			for (int k = 0; k < NUM_OF_VERTICES; k++) {
				if (labels[k] != imax) {
					numReachable++;
				}
			}
			cout << "Number of reachable vertices: " << numReachable << endl;

		}


		// int numVisited = 0;
		// for (int k = 0; k < NUM_OF_EDGES; k++) {
		// 	if (visitedEdges[k] == 1) {
		// 		numVisited++;
		// 	}
		// }
		// cout << "Number of visited edges: " << numVisited << endl;
		
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "BFS: " << elapsed.count() << endl;

	delete[] labels;
	delete[] visitedEdges;

	

	/*
	********************************************************************************************************
	Clean up
	********************************************************************************************************
	*/



	for (unordered_map<int, unordered_map<int, vector<Edge>>> ::iterator it = outgoingMap.begin(); it != outgoingMap.end(); ++it) {
		unordered_map<int, vector<Edge>> map = it->second;
		map.clear();
	}

	edges.clear();
	outgoingMap.clear();
	


	return 0;


}





