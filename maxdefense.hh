///////////////////////////////////////////////////////////////////////////////
// maxdefense.hh
//
// Compute the set of armos that maximizes defense, within a gold budget,
// with the dynamic method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once


#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


// One armor item available for purchase.
class ArmorItem
{
	//
	public:

		//
		ArmorItem
		(
			const std::string& description,
			size_t cost_gold,
			double defense_points
		)
			:
			_description(description),
			_cost_gold(cost_gold),
			_defense_points(defense_points)
		{
			assert(!description.empty());
			assert(cost_gold > 0);
		}

		//
		const std::string& description() const { return _description; }
		int cost() const { return _cost_gold; }
		double defense() const { return _defense_points; }

	//
	private:

		// Human-readable description of the armor, e.g. "new enchanted helmet". Must be non-empty.
		std::string _description;

		// Cost, in units of gold; Must be positive
		int _cost_gold;

		// Defense points; most be non-negative.
		double _defense_points;
};


// Alias for a vector of shared pointers to ArmorItem objects.
typedef std::vector<std::shared_ptr<ArmorItem>> ArmorVector;


// Load all the valid armor items from the CSV database
// Armor items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<ArmorVector> load_armor_database(const std::string& path)
{
	std::unique_ptr<ArmorVector> failure(nullptr);

	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load armor database; Cannot open file: " << path << std::endl;
		return failure;
	}

	std::unique_ptr<ArmorVector> result(new ArmorVector);

	size_t line_number = 0;
	for (std::string line; std::getline(f, line); )
	{
		line_number++;

		// First line is a header row
		if ( line_number == 1 )
		{
			continue;
		}

		std::vector<std::string> fields;
		std::stringstream ss(line);

		for (std::string field; std::getline(ss, field, '^'); )
		{
			fields.push_back(field);
		}

		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load armor database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl
				;
			return failure;
		}

		std::string
			descr_field = fields[0],
			cost_gold_field = fields[1],
			defense_points_field = fields[2]
			;

		auto parse_dbl = [](const std::string& field, double& output)
		{
			std::stringstream ss(field);
			if ( ! ss )
			{
				return false;
			}

			ss >> output;

			return true;
		};

		std::string description(descr_field);
		double cost_gold, defense_points;
		if (
			parse_dbl(cost_gold_field, cost_gold)
			&& parse_dbl(defense_points_field, defense_points)
		)
		{
			result->push_back(
				std::shared_ptr<ArmorItem>(
					new ArmorItem(
						description,
						cost_gold,
						defense_points
					)
				)
			);
		}
	}

	f.close();

	return result;
}


// Convenience function to compute the total cost and defense in an ArmorVector.
// Provide the ArmorVector as the first argument
// The next two arguments will return the cost and defense back to the caller.
void sum_armor_vector
(
	const ArmorVector& armors,
	int& total_cost,
	double& total_defense
)
{
	total_cost = total_defense = 0;
	for (auto& armor : armors)
	{
		total_cost += armor->cost();
		total_defense += armor->defense();
	}
}


// Convenience function to print out each ArmorItem in an ArmorVector,
// followed by the total kilocalories and protein in it.
void print_armor_vector(const ArmorVector& armors)
{
	std::cout << "*** Armor Vector ***" << std::endl;

	if ( armors.size() == 0 )
	{
		std::cout << "[empty armor list]" << std::endl;
	}
	else
	{
		for (auto& armor : armors)
		{
			std::cout
				<< "Ye olde " << armor->description()
				<< " ==> "
				<< "Cost of " << armor->cost() << " gold"
				<< "; Defense points = " << armor->defense()
				<< std::endl
				;
		}

		int total_cost;
		double total_defense;
		sum_armor_vector(armors, total_cost, total_defense);
		std::cout
			<< "> Grand total cost: " << total_cost << " gold" << std::endl
			<< "> Grand total defense: " << total_defense
			<< std::endl
			;
	}
}


// Convenience function to print out a 2D cache, composed of an std::vector<std::vector<double>>
// For sanity, will refuse to print a cache that is too large.
// Hint: When running this program, you can redirect stdout to a file,
//	which may be easier to view and inspect than a terminal
void print_2d_cache(const std::vector<std::vector<double>>& cache)
{
	std::cout << "*** 2D Cache ***" << std::endl;

	if ( cache.size() == 0 )
	{
		std::cout << "[empty]" << std::endl;
	}
	else if ( cache.size() > 250 || cache[1].size() > 250 )
	{
		std::cout << "[too large]" << std::endl;
	}
	else
	{
		for ( const std::vector<double> row : cache)
		{
			for ( double value : row )
			{
				std::cout << std::setw(5) << value;
			}
			std::cout << std::endl;
		}
	}
}

// Filter the vector source, i.e. create and return a new ArmorVector
// containing the subset of the armor items in source that match given
// criteria.
// This is intended to:
//	1) filter out armor with zero or negative defense that are irrelevant to our optimization
//	2) limit the size of inputs to the exhaustive search algorithm since it will probably be slow.
//
// Each armor item that is included must have at minimum min_defense and at most max_defense.
//	(i.e., each included armor item's defense must be between min_defense and max_defense (inclusive).
//
// In addition, the the vector includes only the first total_size armor items that match these criteria.
std::unique_ptr<ArmorVector> filter_armor_vector
(
	const ArmorVector& source,
	double min_defense,
	double max_defense,
	int total_size
)
{
	std::unique_ptr<ArmorVector> arr(new ArmorVector);
	for(int i = 0; i < source.size(); i++)
    {
        if((source[i]->defense() <= 0))
        {
            i++;
        }
        if((*arr).size() < total_size &&
           source[i]->defense() >= min_defense &&
           source[i]->defense() <= max_defense)
            arr->push_back(source[i]);
    }
	return arr;
}

// Compute the optimal set of armor items with a dynamic algorithm.
// Specifically, among the armor items that fit within a total_cost gold budget,
// choose the selection of armors whose defense is greatest.
// Repeat until no more armor items can be chosen, either because we've run out of armor items,
// or run out of gold.
std::unique_ptr<ArmorVector> dynamic_max_defense
(
	const ArmorVector& armors,
	int total_cost
)
{
    std::vector<std::vector<double>> cache;
    std::unique_ptr<ArmorVector> source(new ArmorVector(armors));
    std::unique_ptr<ArmorVector> finish(new ArmorVector);

    for(int i = 0; i <= armors.size(); i++)
    {
        cache.push_back(std::vector<double>());
        for(int j = 0; j <= total_cost+1; j++)
            cache.at(i).push_back(0.0);
    }

    double result = 0.0;
    for(int i = 1; i <= armors.size(); i++)
    {
        for(int j = 1; j <= total_cost; j++)
        {
            std::shared_ptr<ArmorItem> item = armors[i-1];
            if((j - item->cost()) >= 0)
                result = std::max((item->defense() + cache[i-1][j-item->cost()]), cache[i-1][j]);
            else
                result = cache[i-1][j];

            cache[i][j] = result;
        }
    }
    double tempCost = total_cost;
    for(int i = armors.size(); i > 0; i--)
    {
        if(cache[i][tempCost] == cache[i-1][tempCost]){continue;}
        else
        {
            finish->push_back(source->at(i-1));
            tempCost -= (armors.at(i-1))->cost();
        }
    }
	return finish;

}

std::vector<std::vector<ArmorItem>> getDefenseSubsets(std::vector<ArmorItem> source)
{
    std::vector<std::vector<ArmorItem>> subset, subTemp;

    std::vector<ArmorItem> temp;
    subset.push_back(temp);

    for(int i = 0; i < source.size(); i++)
    {
        subTemp = subset;
        for(int j = 0; j < subTemp.size(); j++){subTemp[j].push_back(source[i]);}
        for(int k = 0; k < subTemp.size(); k++){subset.push_back(subTemp[k]);}
    }
    return subset;
}

// Compute the optimal set of armor items with an exhaustive search algorithm.
// Specifically, among all subsets of armor items,
// return the subset whose gold cost fits within the total_cost budget,
// and whose total defense is greatest.
// To avoid overflow, the size of the armor items vector must be less than 64.
std::unique_ptr<ArmorVector> exhaustive_max_defense
(
	const ArmorVector& armors,
	double total_cost
)
{
	const int n = armors.size();
	assert(n < 64);

    std::unique_ptr<ArmorVector> bestDefense(new ArmorVector);
    ArmorVector& bestDefensePtr = *bestDefense;

    std::vector<std::vector<ArmorItem>> DefenseSubset;
    std::vector<std::unique_ptr<ArmorVector>> candidateDefense;

    std::vector<ArmorItem> source;
    std::vector<ArmorItem> bestArmorSet;

    std::unique_ptr<ArmorVector> x(new ArmorVector);
    candidateDefense.push_back(std::move(x));


    double max_defense = 0, candidate_cost = 0, candidate_defense = 0;
    int candidate_index = 0;

    for(int counter = 0; counter < armors.size(); counter++)
    {
        source.push_back(*armors.at(counter));
    }

    DefenseSubset = getDefenseSubsets(source);
    for(int i = 0; i < DefenseSubset.size(); i++)
    {
        candidate_cost = candidate_defense = 0;
        std::vector<ArmorItem> temp = DefenseSubset[i];

        for(int j = 0; j < temp.size(); j++)
        {
            candidate_cost += temp[j].cost();
            candidate_defense += temp[j].defense();
        }
        if(candidate_defense > max_defense && candidate_cost <= total_cost)
        {
            max_defense = candidate_defense;
            candidate_index = i;
        }
    }
    bestArmorSet = DefenseSubset[candidate_index];
    for (auto &best : bestArmorSet)
    {
        auto ptr = std::make_shared<ArmorItem>(best);
        bestDefensePtr.push_back(ptr);
    }
    return bestDefense;
}









