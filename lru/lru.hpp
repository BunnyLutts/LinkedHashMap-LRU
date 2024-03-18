#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"
class Hash {
public:
    unsigned int operator()(Integer lhs) const {
        int val = lhs.val;
        return std::hash<int>()(val);
    }
};
class Equal {
public:
    bool operator()(const Integer &lhs, const Integer &rhs) const {
        return lhs.val == rhs.val;
    }
};

namespace sjtu {
    template <class T>
    class double_list {
    private:
        struct Node {
            T *data;
            Node *prev;
            Node *next;
            Node() : data(nullptr), prev(nullptr), next(nullptr) {}
            Node(const T &val) : data(new T(val)), prev(nullptr), next(nullptr) {}
            ~Node() { delete data; }
        };
        Node *head;

    public:
        /**
         * elements
         * add whatever you want
         */

        // --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        double_list() {
            head = new Node();
            head->next = head->prev = head;
        }
        double_list(const double_list<T> &other) {
            head = new Node();
            head->next = head->prev = head;
            for (auto it = other.begin(); it != other.end(); it++) {
                insert_tail(*it);
            }
        }

        double_list<T> &operator=(const double_list<T> &other) {
            clear();
            for (auto it = other.begin(); it != other.end(); it++) {
                insert_tail(*it);
            }
            return *this;
        }

        ~double_list() {
            clear();
            delete head;
        }

        class iterator {
            friend class double_list<T>;

        private:
            const double_list<T> *from;
            Node *ptr;
            iterator(Node *ptr, const double_list<T> *from) : ptr(ptr), from(from) {}

        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */
            iterator() : ptr(nullptr), from(nullptr) {}
            iterator(const iterator &t) : ptr(t.ptr), from(t.from) {}
            ~iterator() {}
            /**
             * iter++
             */
            iterator operator++(int) {
                if (!ptr || !from || ptr == from->head) {
                    throw sjtu::index_out_of_bound();
                }
                iterator tmp(*this);
                ptr = ptr->next;
                return tmp;
            }
            /**
             * ++iter
             */
            iterator &operator++() {
                if (!ptr || !from || ptr == from->head) {
                    throw sjtu::index_out_of_bound();
                }
                ptr = ptr->next;
                return *this;
            }
            /**
             * iter--
             */
            iterator operator--(int) {
                if (!ptr || !from || ptr->prev == from->head) {
                    throw sjtu::index_out_of_bound();
                }
                iterator tmp(*this);
                ptr = ptr->prev;
                return tmp;
            }
            /**
             * --iter
             */
            iterator &operator--() {
                if (!ptr || !from || ptr->prev == from->head) {
                    throw sjtu::index_out_of_bound();
                }
                ptr = ptr->prev;
                return *this;
            }
            /**
             * if the iter didn't point to a value
             * throw " invalid"
             */
            T &operator*() const {
                if (!ptr || ptr->data == nullptr) {
                    throw sjtu::invalid_iterator();
                }
                return *ptr->data;
            }
            /**
             * other operation
             */
            T *operator->() const noexcept {
                if (!ptr || ptr->data == nullptr) {
                    throw sjtu::invalid_iterator();
                }
                return ptr->data;
            }
            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };
        /**
         * return an iterator to the beginning
         */
        iterator begin() const {
            return iterator(head->next, this);
        }
        /**
         * return an iterator to the ending
         * in fact, it returns the iterator point to nothing,
         * just after the last element.
         */
        iterator end() const {
            return iterator(head, this);
        }
        /**
         * if the iter didn't point to anything, do nothing,
         * otherwise, delete the element pointed by the iter
         * and return the iterator point at the same "index"
         * e.g.
         * 	if the origin iterator point at the 2nd element
         * 	the returned iterator also point at the
         *  2nd element of the list after the operation
         *  or nothing if the list after the operation
         *  don't contain 2nd elememt.
         */
        iterator erase(iterator pos) {
            if (pos.ptr->data == nullptr) return pos;
            iterator ret(pos.ptr->next, this);
            pos.ptr->prev->next = pos.ptr->next;
            pos.ptr->next->prev = pos.ptr->prev;
            delete pos.ptr;
            return ret;
        }

        /**
         * the following are operations of double list
         */
        void insert_head(const T &val) {
            Node *obj = new Node(val);
            obj->next = head->next;
            obj->prev = head;
            obj->next->prev = obj->prev->next = obj;
        }

        void insert_tail(const T &val) {
            Node *obj = new Node(val);
            obj->next = head;
            obj->prev = head->prev;
            obj->next->prev = obj->prev->next = obj;
        }

        void delete_head() {
            if (empty()) return;
            Node *obj = head->next;
            obj->prev->next = obj->next;
            obj->next->prev = obj->prev;
            delete obj;
        }

        void delete_tail() {
            if (empty()) return;
            Node *obj = head->prev;
            obj->prev->next = obj->next;
            obj->next->prev = obj->prev;
            delete obj;
        }

        void clear() {
            for (; !empty(); ) {
                delete_head();
            }
        }

        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() {
            return head->next == head;
        }
    };

    template <
        class Key,
        class T,
        class Hash = std::hash<Key>,
        class Equal = std::equal_to<Key> >
    class hashmap {
    private:
        using value_type = pair<const Key, T>;
        using List = double_list<value_type>;

        int capacity, size;
        double load_factor;
        List* data;

        int pos(const Key &key) const {
            return Hash()(key) % capacity;
        }

        bool eq(const Key &key1, const Key &key2) const {
            return Equal()(key1, key2);
        }

    public:
        /**
         * elements
         * add whatever you want
         */

        // --------------------------

        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        hashmap(int capacity = 10, double load_factor = 0.75) : capacity(capacity), load_factor(load_factor), size(0) {
            data = new List[capacity];
        }
        hashmap(const hashmap &other) : capacity(other.capacity), load_factor(other.load_factor), size(other.size) {
            data = new List[capacity];
            for (int i=0; i<capacity; i++) {
                data[i] = other.data[i];
            }
        }
        ~hashmap() {
            delete[] data;
        }
        hashmap &operator=(const hashmap &other) {
            capacity = other.capacity;
            load_factor = other.load_factor;
            delete[] data;
            data = new List[capacity];
            for (int i=0; i<capacity; i++) {
                data[i] = other.data[i];
            }
            return *this;
        }

        class iterator {
            friend class hashmap<Key, T, Hash, Equal>;
        private:
            typename List::iterator ptr;
            iterator(typename List::iterator ptr) : ptr(ptr) {}

        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */

            iterator() : ptr(){
            }
            iterator(const iterator &t) : ptr(t.ptr) {
            }
            ~iterator() {}

            /**
             * if point to nothing
             * throw
             */
            value_type &operator*() const {
                return *ptr;
            }

            /**
             * other operation
             */
            value_type *operator->() const noexcept {
                return &(*ptr);
            }
            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };

        void clear() {
            for (int i = 0; i < capacity; i++) {
                data[i].clear();
            }
        }
        /**
         * you need to expand the hashmap dynamically
         */
        void expand() {
            hashmap tmp(capacity * 2, load_factor);
            for (int i = 0; i < capacity; i++) {
                for (auto it = data[i].begin(); it!= data[i].end(); it++) {
                    tmp.insert(*it);
                }
            }
            *this = tmp;
        }

        /**
         * the iterator point at nothing
         */
        iterator end() const {
            return iterator();
        }
        /**
         * find, return a pointer point to the value
         * not find, return the end (point to nothing)
         */
        iterator find(const Key &key) const {
            int index = pos(key);
            for (auto it = data[index].begin(); it!= data[index].end(); it++) {
                if (eq(it->first, key)) {
                    return iterator(it);
                }
            }
            return end();
        }
        /**
         * already have a value_pair with the same key
         * -> just update the value, return false
         * not find a value_pair with the same key
         * -> insert the value_pair, return true
         */
        sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
            if (size >= capacity * load_factor) expand();
            int index = pos(value_pair.first);
            for (auto it = data[index].begin(); it!= data[index].end(); it++) {
                if (eq(it->first, value_pair.first)) {
                    it->second = value_pair.second;
                    return sjtu::pair<iterator, bool>(iterator(it), false);
                }
            }
            data[index].insert_head(value_pair);
            return sjtu::pair<iterator, bool>(iterator(data[index].begin()), true);
        }
        /**
         * the value_pair exists, remove and return true
         * otherwise, return false
         */
        bool remove(const Key &key) {
            iterator it = find(key);
            if (it==end()) return false;
            data[pos(key)].erase(it.ptr);
            return true;
        }
    };

    template <
        class Key,
        class T,
        class Hash = std::hash<Key>,
        class Equal = std::equal_to<Key> >
    class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
    public:
        typedef pair<const Key, T> value_type;
        /**
         * elements
         * add whatever you want
         */
        // --------------------------
        class const_iterator;
        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            iterator() {
            }
            iterator(const iterator &other) {
            }
            ~iterator() {
            }

            /**
             * iter++
             */
            iterator operator++(int) {}
            /**
             * ++iter
             */
            iterator &operator++() {}
            /**
             * iter--
             */
            iterator operator--(int) {}
            /**
             * --iter
             */
            iterator &operator--() {}

            /**
             * if the iter didn't point to a value
             * throw "star invalid"
             */
            value_type &operator*() const {
            }
            value_type *operator->() const noexcept {
            }

            /**
             * operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {}
            bool operator!=(const iterator &rhs) const {}
            bool operator==(const const_iterator &rhs) const {}
            bool operator!=(const const_iterator &rhs) const {}
        };

        class const_iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            const_iterator() {
            }
            const_iterator(const iterator &other) {
            }

            /**
             * iter++
             */
            const_iterator operator++(int) {}
            /**
             * ++iter
             */
            const_iterator &operator++() {}
            /**
             * iter--
             */
            const_iterator operator--(int) {}
            /**
             * --iter
             */
            const_iterator &operator--() {}

            /**
             * if the iter didn't point to a value
             * throw
             */
            const value_type &operator*() const {
            }
            const value_type *operator->() const noexcept {
            }

            /**
             * operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {}
            bool operator!=(const iterator &rhs) const {}
            bool operator==(const const_iterator &rhs) const {}
            bool operator!=(const const_iterator &rhs) const {}
        };

        linked_hashmap() {
        }
        linked_hashmap(const linked_hashmap &other) {
        }
        ~linked_hashmap() {
        }
        linked_hashmap &operator=(const linked_hashmap &other) {
        }

        /**
         * return the value connected with the Key(O(1))
         * if the key not found, throw
         */
        T &at(const Key &key) {
        }
        const T &at(const Key &key) const {
        }
        T &operator[](const Key &key) {
        }
        const T &operator[](const Key &key) const {
        }

        /**
         * return an iterator point to the first
         * inserted and existed element
         */
        iterator begin() {
        }
        const_iterator cbegin() const {
        }
        /**
         * return an iterator after the last inserted element
         */
        iterator end() {
        }
        const_iterator cend() const {
        }
        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() const {
        }

        void clear() {
        }

        size_t size() const {
        }
        /**
         * insert the value_piar
         * if the key of the value_pair exists in the map
         * update the value instead of adding a new element，
         * then the order of the element moved from inner of the
         * list to the head of the list
         * and return false
         * if the key of the value_pair doesn't exist in the map
         * add a new element and return true
         */
        pair<iterator, bool> insert(const value_type &value) {
        }
        /**
         * erase the value_pair pointed by the iterator
         * if the iterator points to nothing
         * throw
         */
        void remove(iterator pos) {
        }
        /**
         * return how many value_pairs consist of key
         * this should only return 0 or 1
         */
        size_t count(const Key &key) const {
        }
        /**
         * find the iterator points at the value_pair
         * which consist of key
         * if not find, return the iterator
         * point at nothing
         */
        iterator find(const Key &key) {
        }
    };

    class lru {
        using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
        using value_type = sjtu::pair<const Integer, Matrix<int> >;

    public:
        lru(int size) {
        }
        ~lru() {
        }
        /**
         * save the value_pair in the memory
         * delete something in the memory if necessary
         */
        void save(const value_type &v) const {
        }
        /**
         * return a pointer contain the value
         */
        Matrix<int> *get(const Integer &v) const {
        }
        /**
         * just print everything in the memory
         * to debug or test.
         * this operation follows the order, but don't
         * change the order.
         */
        void print() {
        }
    };
}

#endif