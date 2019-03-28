#include <vector>
#include <list>
#include <map>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    const static size_t START_SIZE = 5;
    const static size_t MAX_LOAD_FACTOR = 2, MIN_LOAD_FACTOR = 8;
    const static size_t EXPAND_FACTOR = 2, SHRINK_FACTOR = 2;
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;
    using init_list = std::initializer_list<std::pair<const KeyType, ValueType>>;
    using items_type = std::list<std::pair<const KeyType, ValueType>>;
    using table_type = std::vector<std::pair<iterator, size_t>>; // size_t is for state: 0 - FREE, 1 - FULL, 2 - DELETED

    HashMap(const Hash &hash_function = Hash()): sz(0), cap(START_SIZE), hasher(hash_function), items(), table() {
        table.resize(cap);
    }

    HashMap(const HashMap &other): HashMap(other.hasher) {
        if (this != &other) {
            for (const auto &elem : other) {
                insert(elem);
            }
        }
    }

    template<class Iter>
    HashMap(Iter first, Iter last, const Hash &hash_function = Hash()): HashMap(hash_function) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    HashMap(const init_list &initializer, const Hash &hash_function = Hash()): HashMap(hash_function) {
        for (const auto &elem : initializer) {
            insert(elem);
        }
    }

    HashMap& operator = (const HashMap &other) {
        if (this != &other) {
            hasher = other.hasher;
            clear();
            for (const auto &elem : other) {
                insert(elem);
            }
        }

        return *this;
    }

    size_t size() const {
        return sz;
    }
    bool empty() const {
        return (sz == 0);
    }
    Hash hash_function() const {
        return hasher;
    }

    iterator begin() {
        return items.begin();
    }
    const_iterator begin() const {
        return items.begin();
    }
    iterator end() {
        return items.end();
    }
    const_iterator end() const {
        return items.end();
    }

    void insert(const std::pair<const KeyType, ValueType> &elem) {
        put(elem);
        check_and_rehash();
    }

    void erase(const KeyType &key) {
        del(key);
        check_and_rehash();
    }

    iterator find(const KeyType &key) {
        return search(key);
    }
    const_iterator find(const KeyType &key) const {
        return search(key);
    }

    ValueType& operator [](const KeyType &key) {
        iterator f = find(key);
        if (f != end()) {
            return f->second;
        }

        std::pair<const KeyType, ValueType> new_elem(key, ValueType());
        insert(new_elem);
        return items.back().second;
    }

    const ValueType& at(const KeyType &key) const {
        const_iterator f = find(key);
        if (f != end()) {
            return f->second;
        }

        throw std::out_of_range("There is no such key in HashMap");
    }

    void clear() {
        items.clear();
        table.clear();
        table.resize(START_SIZE);
        sz = 0;
        cap = START_SIZE;
    }

private:
    size_t sz, cap;
    Hash hasher;
    items_type items;
    table_type table;

    void put(const std::pair<const KeyType, ValueType> &elem) {
        size_t pos = hasher(elem.first) % cap;
        for (size_t i = 0; i < cap; ++i) {
            if (table[pos].second != 1) {
                sz++;
                items.push_back(elem);
                table[pos].first = --(items.end());
                table[pos].second = 1;
                return;
            } else if (table[pos].second == 1 && table[pos].first->first == elem.first) {
                return;
            }

            pos++;
            if (pos == cap) {
                pos = 0;
            }
        }
    }

    void del(const KeyType &key) {
        size_t pos = hasher(key) % cap;
        for (size_t i = 0; i < cap; ++i) {
            if (table[pos].second == 1 && table[pos].first->first == key) {
                sz--;
                items.erase(table[pos].first);
                table[pos].second = 2;
                return;
            } else if (table[pos].second == 0) {
                return;
            }

            pos++;
            if (pos == cap) {
                pos = 0;
            }
        }
    }

    iterator search(const KeyType &key) {
        size_t pos = hasher(key) % cap;
        for (size_t i = 0; i < cap; ++i) {
            if (table[pos].second == 1 && table[pos].first->first == key) {
                return table[pos].first;
            } else if (table[pos].second == 0) {
                return end();
            }

            pos++;
            if (pos == cap) {
                pos = 0;
            }
        }

        return end();
    }

    const_iterator search(const KeyType &key) const {
        size_t pos = hasher(key) % cap;
        for (size_t i = 0; i < cap; ++i) {
            if (table[pos].second == 1 && table[pos].first->first == key) {
                return table[pos].first;
            } else if (table[pos].second == 0) {
                return end();
            }

            pos++;
            if (pos == cap) {
                pos = 0;
            }
        }

        return end();
    }

    void check_and_rehash() {
        size_t prev_cap = cap;
        if (MAX_LOAD_FACTOR * sz > cap) {
            cap = EXPAND_FACTOR * cap + 1;
        } else if (MIN_LOAD_FACTOR * sz < cap) {
            cap /= SHRINK_FACTOR;
            if (cap < START_SIZE) {
                cap = START_SIZE;
            }
        }
        if (prev_cap == cap) {
            return;
        }

        items_type new_items = items;
        items.clear();
        table.clear();
        table.resize(cap);
        sz = 0;
        for (const auto &elem : new_items) {
            put(elem);
        }
    }
};
