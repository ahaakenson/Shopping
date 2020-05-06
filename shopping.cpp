#include <algorithm>
#include <string>
#include <vector>

#include "shopping.h"

item::item(std::vector<std::string> const & input){
    name = input[0];
    quantity = std::stol(input[1]);
    price = std::stod(input[2]) * 100.;
}

bool item::operator==(item const & input_item){
    return name == input_item.name;
}

bool item::operator==(std::string const & input_string){
    return name == input_string;
}

void store::add_inventory(item const & input_item){
    //adds item to inventory list, or increases quantity if item already there
    auto it = std::find(inventory.begin(), inventory.end(), input_item);
    if(it == inventory.end()){
        inventory.push_back(input_item);
        return;
    }
    it->quantity += input_item.quantity;
    return;
}

long store::get_item_price(std::string item_name){
    auto it = std::find(inventory.begin(), inventory.end(), item_name);
    return it->price;
}

long store::get_item_quantity(std::string item_name){
    auto it = std::find(inventory.begin(), inventory.end(), item_name);
    return it->quantity;
}

bool store::find_item(std::string item_name){
    //checks if item exists in inventory
    auto it = std::find(inventory.begin(), inventory.end(), item_name);
    if(it == inventory.end()){
        return false;
    }
    return true;
}