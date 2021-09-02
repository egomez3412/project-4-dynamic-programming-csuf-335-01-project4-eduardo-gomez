
#include<iostream>
#include "maxdefense.hh"
#include "timer.hh"
#include <string>
#include <vector>
using namespace std;

int main()
{
	vector<int> dynamic_n = {1000, 2000, 4000, 6000, 8000};
    //vector<int> exhaust_n = {10, 15, 20, 25, 30};

    //vector<int> dynamic_n = {15};
    //vector<int> exhaust_n = {15};
    /*
    int n = 5;
    cout << "Starting Dynamic greedy algorithm search..." << endl;
    auto all_armors = load_armor_database("armor.csv");
    auto filtered_armors = filter_armor_vector(*all_armors, 1, 3000, n);
    auto dynamic_armors = dynamic_max_defense(*filtered_armors, 7);
    cout << endl;
    cout << "Greedy n is " << n << endl;
    cout << endl;
    //print_armor_vector(*filtered_armors);
    cout << endl;
    print_armor_vector(*dynamic_armors);
    cout << endl;


    int n = 15;
    cout << "Starting exhaustive search..." << endl;
    //and exhaustive search
    //auto all_armors_dynamic = load_armor_database("armor.csv");
    auto all_armors_exhaust = load_armor_database("armor.csv");
    //auto filtered_armors_dynamic = filter_armor_vector(*all_armors_dynamic, 1, 3000, n);
    auto filtered_armors_exhaust = filter_armor_vector(*all_armors_exhaust, 1, 3000, n);
    //auto greedy_armors = greedy_max_defense(*filtered_armors_dynamic, 2000);
    auto exhaustive_armors = exhaustive_max_defense(*filtered_armors_exhaust, 2000);
    cout << endl;
    cout << "Exhaustive n is " << n << endl;
    cout << endl;
    //print_armor_vector(*greedy_armors);
    print_armor_vector(*exhaustive_armors);
    cout << endl;
    */
    auto all_armors_dynamic = load_armor_database("armor.csv");
    //auto all_armors_exhaust = load_armor_database("armor.csv");
    for(int i = 0; i < dynamic_n.size(); i++)
    {
        auto filtered_armors_dynamic = filter_armor_vector(*all_armors_dynamic, 1, 3000, dynamic_n.at(i));
        //auto filtered_armors_exhaust = filter_armor_vector(*all_armors_exhaust, 1, 3000, exhaust_n.at(i));
        Timer timer1;
        auto dynamic_armors = dynamic_max_defense(*filtered_armors_dynamic, 2000);
        double elapsed1 = timer1.elapsed();
        cout << "Dynamic elapsed time = " << elapsed1 << " seconds" << endl;
        cout << "Dynamic n is " << dynamic_n.at(i) << endl;
        cout << endl;
        //print_armor_vector(*dynamic_armors);
        /*
        Timer timer2;
        auto exhaustive_armors = exhaustive_max_defense(*filtered_armors_exhaust, 2000);
        double elapsed2 = timer2.elapsed();
        cout << "Exhaustive elapsed time = " << elapsed2 << " seconds" << endl;
        cout << "Exhaustive n is " << exhaust_n.at(i) << endl;
        cout << endl;
        print_armor_vector(*exhaustive_armors);
        */
    }

    return 0;
}


