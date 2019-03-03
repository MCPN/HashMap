#include <vector>
#include <list>
#include <map>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;
    using InitList = std::initializer_list<std::pair<const KeyType, ValueType>>;

    HashMap(const Hash &hshr=Hash()): sz(0), cp(5), hasher(hshr), items(), table() {
        table.resize(cp);
    }

    HashMap(const HashMap &other): HashMap(other.hasher) {
        if (this != &other) {
            for (auto elem : other) {
                insert(elem);
            }
        }
    }

    template<class Iter>
    HashMap(Iter first, Iter last, const Hash &hshr=Hash()): HashMap(hshr) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    HashMap(const InitList &lst, const Hash &hshr=Hash()): HashMap(hshr) {
        for (auto elem : lst) {
            insert(elem);
        }
    }

    HashMap& operator =(const HashMap &other) {
        if (this != &other) {
            hasher = other.hasher;
            clear();
            for (auto elem : other) {
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
        if (2 * sz > cp) {
            rehash();
        }
    }

    void erase(const KeyType &key) {
        del(key);
        if (!empty() && 8 * sz < cp) {
            rehash(true);
        }
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
            return f -> second;
        }

        std::pair<const KeyType, ValueType> new_elem(key, ValueType());
        insert(new_elem);
        return items.back().second;
    }

    const ValueType& at(const KeyType &key) const {
        const_iterator f = find(key);
        if (f != end()) {
            return f -> second;
        }

        throw std::out_of_range("There is no such key in HashMap");
    }

    void clear() {
        size_t tmp = sz;
        for (size_t i = 0; i < tmp; ++i) {
            erase(items.begin() -> first);
        }
    }

private:
    size_t sz, cp;
    Hash hasher;
    std::list<std::pair<const KeyType, ValueType>> items;
    std::vector<std::pair<iterator, size_t>> table; // size_t is for state: 0 - FREE, 1 - FULL, 2 - DELETED

    void put(const std::pair<const KeyType, ValueType> &elem) {
        size_t pos = hasher(elem.first) % cp;
        for (size_t i = 0; i < cp; ++i) {
            if (table[pos].second != 1) {
                sz++;
                items.push_back(elem);
                table[pos].first = --(items.end());
                table[pos].second = 1;
                return;
            } else if (table[pos].second == 1 && table[pos].first -> first == elem.first) {
                return;
            }

            pos++;
            if (pos == cp) {
                pos = 0;
            }
        }
    }

    void del(const KeyType &key) {
        size_t pos = hasher(key) % cp;
        for (size_t i = 0; i < cp; ++i) {
            if (table[pos].second == 1 && table[pos].first -> first == key) {
                sz--;
                items.erase(table[pos].first);
                table[pos].second = 2;
                return;
            } else if (table[pos].second == 0) {
                return;
            }

            pos++;
            if (pos == cp) {
                pos = 0;
            }
        }
    }

    iterator search(const KeyType &key) {
        size_t pos = hasher(key) % cp;
        for (size_t i = 0; i < cp; ++i) {
            if (table[pos].second == 1 && table[pos].first -> first == key) {
                return table[pos].first;
            } else if (table[pos].second == 0) {
                return end();
            }

            pos++;
            if (pos == cp) {
                pos = 0;
            }
        }

        return end();
    }

    const_iterator search(const KeyType &key) const {
        size_t pos = hasher(key) % cp;
        for (size_t i = 0; i < cp; ++i) {
            if (table[pos].second == 1 && table[pos].first -> first == key) {
                return table[pos].first;
            } else if (table[pos].second == 0) {
                return end();
            }

            pos++;
            if (pos == cp) {
                pos = 0;
            }
        }

        return end();
    }

    void rehash(bool shrink=false) {
        if (shrink) {
            cp /= 2;
        } else {
            cp = 2 * cp + 1;
        }

        table.clear();
        table.resize(cp);
        size_t tmp = sz;
        for (size_t i = 0; i < tmp; ++i) {
            put(*items.begin());
            items.erase(items.begin());
            sz--;
        }
    }
};
