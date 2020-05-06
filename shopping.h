#pragma once

#include <string>
#include <vector>
#include <algorithm>

struct item {
    std::string name;
    long price; //in cents
    long quantity;

    item(std::vector<std::string> const &);
    bool operator== (item const &);
    bool operator== (std::string const &);
};

struct store {
    std::string name;
    std::string location;
    std::vector<item> inventory;

    store(std::string word, std::string place) : name(word), location(place) {};
    store() = default;
    
    void add_inventory(item const &);
    std::string print_inventory();
    long get_item_price(std::string);
    long get_item_quantity(std::string);
    bool find_item(std::string);
};