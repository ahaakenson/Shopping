#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <utility>
#include <iterator>

#include "shopping.h"



std::vector<std::string> split(std::string const & input, char delimiter){
    //splits a string into a vector of strings
    //separates strings based on delimiting character input
    std::vector<std::string> split_string;
    std::istringstream iss(input);
    std::string word;
    while(getline(iss, word, delimiter)){
        split_string.push_back(word);
    }
    split_string[2].erase(0, 1); //erases dollar sign
    return split_string;
}

void generate_items(store & input_store){
    //takes in store inventory information and creates item objects
    std::string line;
    while(getline(std::cin, line)){
        if(std::find(line.begin(), line.end(), ',') == line.end()){
            return; //stops adding to store inventory if a blank line is found
        }
        std::vector<std::string> split_line = split(line, ',');
        item item_line(split_line);
        input_store.add_inventory(item_line);
    }
}

store input_store(){
    std::string store_name, store_location;
    getline(std::cin, store_name); getline(std::cin, store_location);
    store store_inst(store_name, store_location); 
    generate_items(store_inst);
    return store_inst;
}

std::vector<store> generate_stores(long num_stores){
    std::vector<store> store_vec;
    for(long i = 0; i < num_stores; ++i){
        store store_inst = input_store();
        store_vec.push_back(store_inst);
    }
    return store_vec;
}

long get_store_long(){
    //gets info on how many stores there are
    std::string line;
    getline(std::cin, line);
    std::istringstream iss(line);
    long num_stores;
    iss >> num_stores;
    return num_stores;
}

std::pair<std::map<std::string, long>, std::vector<std::string>> get_shopping_list(){
    //uses shopping list info from cin to make a map of shopping item to quantity needed
    std::string line;
    getline(std::cin, line);
    std::istringstream iss(line);

    long list_size;
    iss >> list_size;

    std::map<std::string, long> shopping_map;
    std::vector<std::string> purchase_vector; //vector for sorting items by name

    for(long i = 0; i < list_size; ++i){
        getline(std::cin, line);
        std::istringstream item_iss(line);
        long quantity; std::string name;

        item_iss >> quantity;
        getline(item_iss, name);
        name.erase(0,1); //deletes leading space

        shopping_map[name] = quantity;
        purchase_vector.push_back(name);
    }
    std::pair<std::map<std::string, long>, std::vector<std::string>> result = std::make_pair(shopping_map, purchase_vector);
    return result;
}

void print_store_info(std::vector<store> const & store_vec){
    std::cout << "Store Related Information (ordered by in-file order):" << std::endl;
    std::cout << "There are " << store_vec.size() << " store(s)." << std::endl;

    for (store const & store_inst : store_vec){
        std::cout << store_inst.name << " has " <<  store_inst.inventory.size() << " distinct items." << std::endl;
    }
    std::cout << std::endl;
}

std::pair<std::map<std::string, long>, std::vector<std::string>> make_item_map(std::vector<store> const & store_vec){
    //compiles all store inventories into one single item map
    std::map<std::string, long> item_map;
    std::vector<std::string> name_vector;

    for(store const & store_inst: store_vec){
        for(item const & item_inst: store_inst.inventory){
            auto it = std::find(name_vector.begin(), name_vector.end(), item_inst.name);
            if(it == name_vector.end()){
                name_vector.push_back(item_inst.name);
                item_map[item_inst.name] = item_inst.quantity;
            }
            else{
                item_map.at(item_inst.name) += item_inst.quantity;
            }
        }
    }
    std::sort(name_vector.begin(), name_vector.end());
    std::pair<std::map<std::string, long>, std::vector<std::string>> result = std::make_pair(item_map, name_vector);
    return result;
}


void print_item_info(std::map<std::string, long> const & item_map, std::vector<std::string> const & name_vector){
    std::cout << "Item Related Information (ordered alphabetically):" << std::endl;
    std::cout << "There are " << item_map.size() << " distinct item(s) available for purchase." << std::endl;
    for (auto const & name : name_vector){
        std::cout << "There are " << item_map.at(name) << ' ' << name << "(s)." << std::endl;
    }
    std::cout << std::endl;
}

std::vector<store> find_stores(std::vector<store> const & master_list, std::string item_name){
    //finds all of the stores carrying the input item
    std::vector<store> stores_w_item;
    auto lambda = [&] (store input) {return input.find_item(item_name); }; //returns true if item is in store inventory
    std::copy_if(master_list.begin(), master_list.end(), std::back_inserter(stores_w_item), lambda);
    return stores_w_item; 
}

long buy_items(std::string const & name, long quantity, std::vector<store> const & store_vec){
    //calculates the cheapest way to buy selected items
    std::cout << "Trying to order " << quantity << ' ' << name << "(s)." << std::endl;
    long total_cost = 0;
    std::vector<store> stores_with_item = find_stores(store_vec, name);
    std::cout << stores_with_item.size() << " store(s) sell " << name << '.' << std::endl;
    std::ostringstream oss;

    while(quantity > 0 and stores_with_item.size() != 0){
        auto lambda = [&] (store input1, store input2) {return input1.get_item_price(name) < input2.get_item_price(name); };
        auto it = std::min_element(stores_with_item.begin(), stores_with_item.end(), lambda); //finds store with lowest price of selected item
        
        store minimum = *it;
        long store_stock = minimum.get_item_quantity(name), store_price = minimum.get_item_price(name);
        
        if(quantity <= store_stock){
            total_cost += quantity * store_price;
            oss << "Order " << quantity << " from " << minimum.name << " in " << minimum.location << std::endl;
            quantity = 0;
        }
        else{
            total_cost += store_stock * store_price;
            oss << "Order " << store_stock << " from " << minimum.name << " in " << minimum.location << std::endl;
            quantity -= store_stock;
            stores_with_item.erase(it, it + 1);
        }
    }
    std::cout << "Total price: $" << std::fixed << std::setprecision(2) << total_cost / 100. << std::endl;
    std::cout << oss.str() << std::endl;
    return total_cost;
}

void print_shopping_info(std::map<std::string, long> const & shopping_list, std::vector<store> store_vec, std::vector<std::string> const & purchase_vector){
    std::cout << "Shopping:" << std::endl;
    long total_price = 0;

    for(auto const & name : purchase_vector){
        long quantity = shopping_list.at(name);
        total_price += buy_items(name, quantity, store_vec);
    }
    std::cout << "Be sure to bring $" << std::fixed << std::setprecision(2) << total_price / 100. << " when you leave for the stores." << std::endl;
}

int main(){
    long num_stores = get_store_long();
    std::vector<store> store_vec = generate_stores(num_stores);
    auto [shopping_list, purchase_vector] = get_shopping_list();
    
    print_store_info(store_vec);
    auto [item_map, name_vector] = make_item_map(store_vec);
    print_item_info(item_map, name_vector);
    print_shopping_info(shopping_list, store_vec, purchase_vector);
}
