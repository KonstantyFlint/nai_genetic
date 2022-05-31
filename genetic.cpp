#include <iostream>
#include <vector>
#include <random>
#include <functional>

using namespace std;

#define very_long long long int

random_device device;

bool extract_bit(very_long num, int index) {
    return (num >> index) & 1;
}

vector<bool> encode_chromosome(pair<very_long, very_long> pair) {
    vector<bool> out;
    for (int i = 0; i < 64; i++) {
        out.push_back(extract_bit(pair.first, 63 - i));
    }
    for (int i = 0; i < 64; i++) {
        out.push_back(extract_bit(pair.second, 63 - i));
    }
    return out;
}

pair<very_long, very_long> decode_chromosome(vector<bool> bits) {
    very_long first = 0;
    very_long second = 0;

    for (int i = 0; i < 64; i++) {
        first *= 2;
        first += bits[i];
    }
    for (int i = 64; i < 128; i++) {
        second *= 2;
        second += bits[i];
    }
    return {first, second};
}

double cast_to_domain(very_long num) {
    return num / 1.8446744e+18;
}

double himmelblau(double x, double y) {
    return (x * x + y - 11) * (x * x + y - 11) + (x + y * y - 7) * (x + y * y - 7);
}

double himmelblau_fitness(vector<bool> chromosome) {
    auto fenotype = decode_chromosome(chromosome);
    double x = cast_to_domain(fenotype.first);
    double y = cast_to_domain(fenotype.second);
    return 1 / (himmelblau(x, y) + 1);
}

void test_converter() {
    pair<very_long, very_long> a;
    a.first = 5.5340232e+18;
    a.second = 3.6893488e+18;

    vector<bool> bits = encode_chromosome(a);
    pair<very_long, very_long> b = decode_chromosome(bits);
    cout << b.first << " " << b.second << "\n";

    cout << himmelblau_fitness(bits) << "\n";
}

vector<bool> mutate(vector<bool> chromosome) {
    uniform_int_distribution<int> dist(0, chromosome.size() - 1);
    int index = dist(device);
    auto mutated = chromosome;
    mutated[index] = !mutated[index];
    return mutated;
}

pair<vector<bool>, vector<bool>> crossover(pair<vector<bool>, vector<bool>> parents) {
    uniform_int_distribution<int> dist(0, parents.first.size() - 1);
    int index1 = dist(device);
    int index2 = dist(device);
    if (index1 > index2)swap(index1, index2);
    vector<bool> offspring_first = parents.first;
    vector<bool> offspring_second = parents.second;

    for (int i = index1; i <= index2; i++) {
        swap(offspring_first[i], offspring_second[i]);
    }

    return {offspring_first, offspring_second};
}

vector<bool> generate_chromosome(int length) {
    uniform_int_distribution<int> dist(0, 1);
    vector<bool> out;
    for (int i = 0; i < length; i++) {
        out.push_back(dist(device));
    }
    return out;
}

vector<vector<bool>> generate_population(int population_size, int chromosome_length) {
    vector<vector<bool>> population;
    population.reserve(population_size);
    for (int i = 0; i < population_size; i++)
        population.push_back(generate_chromosome(chromosome_length));
    return population;
}

pair<int, int> tournament_selection(vector<double> fitness_vector) {
    uniform_int_distribution<int> dist(0, fitness_vector.size() - 1);
    int a_1 = dist(device);
    int a_2 = dist(device);
    int b_1 = dist(device);
    int b_2 = dist(device);

    int a = fitness_vector[a_1] > fitness_vector[a_2] ? a_1 : a_2;
    int b = fitness_vector[b_1] > fitness_vector[b_2] ? b_1 : b_2;

    return {a, b};
}

vector<double> calculate_fitness(vector<vector<bool>> population, function<double(vector<bool>)> fitness) {
    vector<double> out;
    out.reserve(population.size());
    for (auto &i : population) {
        out.push_back(fitness(i));
    }
    return out;
}

vector<vector<bool>> genetic_algorithm(vector<vector<bool>> initial_population, int iterations, function<double(vector<bool>)> fitness_function) {
    auto population = initial_population;
    for (int i = 0; i < iterations; i++) {
        vector<double> fitness_vector = calculate_fitness(population, fitness_function);
        for (int i = 0; i < fitness_vector.size(); i++) {
            cout << fitness_vector[i] << " ";
        }
        cout << "\n";
        vector<vector<bool>> offspring_population;
        for (int i = 0; i < initial_population.size() / 2; i++) {
            auto parents_ind = tournament_selection(fitness_vector);
            pair<vector<bool>, vector<bool>> parents = {population[parents_ind.first], population[parents_ind.second]};
            auto offspring = crossover(parents);
            offspring_population.push_back(mutate(offspring.first));
            offspring_population.push_back(mutate(offspring.second));
        }
        population = offspring_population;
    }
    return population;
}

int main() {
	auto population = generate_population(20, 128);
    genetic_algorithm(population, 100, himmelblau_fitness);
}
