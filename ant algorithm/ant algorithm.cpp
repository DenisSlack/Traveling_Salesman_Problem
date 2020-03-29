#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <utility>


const int MAX_CITIES = 10;
const int MAX_DISTANCE = 10;
const int MAX_TOUR = MAX_CITIES * MAX_DISTANCE;
const int MAX_ANTS = 30;
const double alpha = 1; // влияние феромона на оптимизацию
const double beta = 5; // степень жадности
const double EVAPORATION = 0.5; // скорость испарения феромона
const double QU = 1.0;
const int MAX_TOURS = 100;
const int MAX_ITER = 5;
const double PHEROMONE = 1. / MAX_CITIES; // пропорциональность нанесения феромона

struct ant
{
	int location;
	std::vector <int> taby_city;
	std::vector <int> path;
	double length;
	int sum_city;
	int next_city;
};


class ant_algorithm
{
public:
	int iter;
	std::vector <int> city_x; 
	std::vector <int> city_y; 
	std::vector <std::vector <double>> distance;
	std::vector <std::vector <double>> pheromone;
	std::vector <ant> ants;
	int best_index;
	double best_result_ant = MAX_TOURS;
	std::vector <int> best_path;
	ant_algorithm(); // инициализация городов
	void AntInit(bool reset); // инициализация муравьев
	int MovingAnts();
	int SelectNextCity(int ant_index);
	double AntProduct(int from, int to);
	void updatePheromoneAnts();
	void run();
};



ant_algorithm::ant_algorithm()
{
	
	distance = std::vector <std::vector <double>>(10, std::vector<double> (10));
	pheromone = std::vector <std::vector <double>>(10, std::vector<double>(10));
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, MAX_DISTANCE);
	for (int i = 0; i < MAX_CITIES; i++)
	{
		city_x.push_back(distribution(generator));
		city_y.push_back(distribution(generator));
		for (int j = 0; j < MAX_CITIES; j++)
		{
			distance[i][j] = 0;
			pheromone[i][j] = PHEROMONE;
		}
	}

	for (int from = 0; from < MAX_CITIES; from++)
	{
		for (int to =0; to < MAX_CITIES; to++)
		{
			if ((0 == distance[from][to]) && (to != from))
			{
				double delta_x = abs(city_x[from] - city_x[to]);
				double delta_y = abs(city_y[from] - city_y[to]);
				distance[from][to] = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
				distance[to][from] = distance[from][to];
			}
		}
	}

	std::cout << "Locations city:\n";
	std::cout << "|" << std::setfill(' ') << std::setw(10) << "Num. city" << std::setfill(' ') << std::setw(2) << "|";
	std::cout << std::setfill(' ') << std::setw(6) << "X" << std::setfill(' ') << std::setw(6) << "|";
	std::cout << std::setfill(' ') << std::setw(6) << "Y" << std::setfill(' ') << std::setw(6) << "|\n";
	std::cout << "------------------------------------\n";
	for (int i = 0; i < city_x.size(); i++)
	{
		std::cout << "|" << std::setfill(' ') << std::setw(6) << i + 1 << std::setfill(' ') << std::setw(6) << "|";
		std::cout << std::setfill(' ') << std::setw(6) << city_x[i] << std::setfill(' ') << std::setw(6) << "|";
		std::cout << std::setfill(' ') << std::setw(6) << city_y[i] << std::setfill(' ') << std::setw(6) << "|\n";
		std::cout << "------------------------------------\n";
	
	}
	int j = 0;
	std::cout << "\nDistance:\n";
	std::cout << "|" << std::setfill(' ') << std::setw(10) << "|";
	for (int i = 0; i < MAX_CITIES; i++) std::cout << std::setfill(' ') << std::setw(5) << i + 1 << std::setfill(' ') << std::setw(5) << "|";
	std::cout << "\n---------------------------------------------------------------------------------------------------------------";
	for (int from = 0; from < MAX_CITIES; from++)
	{
		std::cout << "\n|" << std::setfill(' ') << std::setw(5) << from + 1 << std::setfill(' ') << std::setw(5) << "|";
			for (int to = 0; to < MAX_CITIES; to++)
			{
					std::cout << std::setprecision(4) << std::setfill(' ') << std::setw(5) <<  distance[from][to] << std::setfill(' ') << std::setw(5) << "|";
			}
			std::cout << "\n---------------------------------------------------------------------------------------------------------------";
	}
	
	
	ants = std::vector <ant>(30);
}

void ant_algorithm::AntInit(bool reset)
{
	int top_count = 0;
	for (int ant = 0; ant < MAX_ANTS; ant++)
	{
		if (top_count == MAX_CITIES) top_count = 0;
		if (reset)
		{
			std::cout << "\n-------------------------------------------\n";
			std::cout << "Ant num -> " << std::setfill(' ') << std::setw(2) << ant+1 <<  std::setfill(' ') << std::setw(31) << "|\n";
			std::cout << "Length -> " << std::setprecision(4) <<  ants[ant].length << std::setfill(' ') << std::setw(29) << "|\n";
			std::cout << "Path -> ( ";
			for (int i = 0; i < MAX_CITIES; i++) std::cout << ants[ant].path[i] << "  ";
			std::cout << ") |\n-------------------------------------------\n";
		}
		if (reset && ants[ant].length < best_result_ant)
		{
			best_result_ant = ants[ant].length;
			best_index = ant;
			best_path = ants[ant].path;
		}
		ants[ant].location = top_count++;
		ants[ant].taby_city = std::vector <int>(10);
		ants[ant].path = std::vector <int>(10);
		for (int from = 0; from < MAX_CITIES; from++)
		{
			ants[ant].taby_city[from] = 0;
			ants[ant].path[from] = -1;
		}
		ants[ant].sum_city = 1;
		ants[ant].path[0] = ants[ant].location;
		ants[ant].next_city = -1;
		ants[ant].length = 0;
		ants[ant].taby_city[ants[ant].location] = 1;
	}

	

}

int ant_algorithm::SelectNextCity(int ant_index)
{
	double sigma = 0, probably = 0, to_city = 0;
	int from = ants[ant_index].location;
	for (int to = 0; to < MAX_CITIES; to++)
	{
		if (ants[ant_index].taby_city[to] == 0)
		{
			double temp = AntProduct(from, to);
			sigma += temp;
		}
	}
	if (sigma > 0)
	{
		while (true)
		{
			to_city++;
			if (to_city >= MAX_CITIES) to_city = 0;
			if (0 == ants[ant_index].taby_city[to_city])
			{
				probably = (AntProduct(from, to_city)) / sigma;

				std::mt19937 engine;
				std::random_device device;
				engine.seed(device());
				std::uniform_real_distribution<double> distribution(0, 1);
				double temp = distribution(engine);
				if (temp < probably) break;
			}
		}
	}
	return to_city;
}



double ant_algorithm::AntProduct(int from, int to)
{
	if (distance[from][to] > 0)
	{
		double temp =  pow(pheromone[from][to], alpha)*(1 / pow(distance[from][to], beta));
		return temp;

	}
	else return 0;
}

void ant_algorithm::updatePheromoneAnts()
{
	// испарение
	for (int from = 0; from < MAX_CITIES; from++)
	{
		for (int to = 0; to < MAX_CITIES; to++)
		{
			if (from != to)
			{
				pheromone[from][to] *= (1 - EVAPORATION);
				if (pheromone[from][to] < 0) pheromone[from][to] = PHEROMONE;
			}
		}
	}
	// нанесение
	int from, to;
	for (int ant = 0; ant < MAX_ANTS; ant++)
	{
		for (int i = 0; i < MAX_CITIES; i++)
		{
			if (i < MAX_CITIES - 1)
			{
				from = ants[ant].path[i];
				to = ants[ant].path[i + 1];
			}
			else
			{
				from = ants[ant].path[i];
				to = ants[ant].path[0];
			}
			pheromone[from][to] += QU / ants[ant].length;
			pheromone[to][from] = pheromone[from][to];
		}
	}
	for (int from = 0; from < MAX_CITIES; from++)
		for (int to = 0; to < MAX_CITIES; to++)
			pheromone[from][to] *= EVAPORATION;
}

void ant_algorithm::run()
{
	while (iter < MAX_ITER)
	{
		int moves = MovingAnts();
		if ( moves == 0 )
		{
			iter++;
			updatePheromoneAnts();
			if (MAX_ITER != iter)
			{
				std::cout << "\n\nIteration -> " << iter << "\n";
				AntInit(true);
			}
		}
	}
	std::cout << "BEST SOLUTION -> " << best_result_ant << std::endl;
	std::cout << "Best path -> ";
	for (int i = 0; i < MAX_CITIES; i++) std::cout << best_path[i]+1 << "  ";
}


int ant_algorithm::MovingAnts()
{
	int moving = 0;
	for (int k = 0; k < MAX_ANTS; k++)
	{
		if (ants[k].sum_city < MAX_CITIES)
		{
			ants[k].next_city = SelectNextCity(k);
			ants[k].taby_city[ants[k].next_city] = 1;
			int index_city = ants[k].sum_city++;
			ants[k].path[index_city] = ants[k].next_city;
			
			ants[k].length += distance[ants[k].location][ants[k].next_city];
			if (MAX_CITIES == ants[k].sum_city)
			{
				ants[k].length += distance[ants[k].path[MAX_CITIES - 1]][ants[k].path[0]];
			}
			ants[k].location = ants[k].next_city;
			moving++;
		}
	}
	return moving;
}


int main()
{
	ant_algorithm myalgotithm;
	myalgotithm.AntInit(false);
	myalgotithm.run();
 

}

