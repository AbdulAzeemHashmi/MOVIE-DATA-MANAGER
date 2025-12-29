#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// Maximum number of connections to create per actor/genre to prevent graph density explosion (To prevent performance issues)
const int max_links = 25; 

// Helper Functions
int get_max(int a, int b) {
    return (a > b) ? a : b;
}

// Formats a string for searching: removes special chars, trims spaces, and converts to lowercase.
string format_key(string str) {
    string temp = "";
    for (char c : str) {
        if (c >= 32 && c <= 126) temp += c; // Keep printable ASCII only
    }
    
    size_t first = temp.find_first_not_of(' ');
    if (string::npos == first) return "";
    size_t last = temp.find_last_not_of(' ');
    temp = temp.substr(first, (last - first + 1));
    
    for (size_t i = 0; i < temp.length(); i++) {
        if (temp[i] >= 'A' && temp[i] <= 'Z') temp[i] += 32;
    }
    return temp;
}

// Cleans a string for display by removing invisible control characters.
string clean_str(string str) {
    string clean = "";
    for (unsigned char c : str) {
        if (c >= 32) clean += c;
    }
    while (!clean.empty() && clean.back() == ' ') clean.pop_back();
    return clean;
}

// Safely converts string to int using stringstream to prevent crashes on bad data.
int to_int(string s) {
    if (s.empty()) return 0;
    int val = 0;
    stringstream ss(s);
    ss >> val; 
    return val;
}

// Safely converts string to float using stringstream.
float to_float(string s) {
    if (s.empty()) return 0.0f;
    float val = 0.0f;
    stringstream ss(s);
    ss >> val; 
    return val;
}

class MovieNode; 
class AVLTree;

// Generic Linked List (Used for storing lists of actors, genres, and movie neighbors.)
template <typename T>
struct list_node {
    T data;
    list_node* next;
    list_node(T value) : data(value), next(nullptr) {}
};

template <typename T>
class LinkedList {
public:
    list_node<T>* head;
    list_node<T>* tail;
    int size; 

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~LinkedList() { clear(); }

    void clear() {
        list_node<T>* current = head;
        while (current != nullptr) {
            list_node<T>* temp = current;
            current = current->next;
            delete temp;
        }
        head = tail = nullptr;
        size = 0;
    }

    // Appends a new item to the end of the list
    void insert(T value) {
        list_node<T>* new_node = new list_node<T>(value);
        if (!head) head = tail = new_node;
        else {
            tail->next = new_node;
            tail = new_node;
        }
        size++;
    }

    // Checks if a value exists in the list
    bool has_item(T value) const {
        list_node<T>* current = head;
        while (current) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }

    // Removes a specific value from the list
    void remove(T value) {
        if (!head) return;
        if (head->data == value) {
            list_node<T>* temp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete temp;
            size--;
            return;
        }
        list_node<T>* current = head;
        while (current->next && current->next->data != value) {
            current = current->next;
        }
        if (current->next) {
            list_node<T>* temp = current->next;
            current->next = temp->next;
            if (temp == tail) tail = current;
            delete temp;
            size--;
        }
    }

    // Removes and returns the first item (used for Queue)
    T pop_front() {
        if (!head) return T(); 
        list_node<T>* temp = head;
        T val = temp->data;
        head = head->next;
        if (!head) tail = nullptr;
        delete temp;
        size--;
        return val;
    }
    
    // Inserts at the front (used for Stack)
    void push_front(T value) {
        list_node<T>* new_node = new list_node<T>(value);
        new_node->next = head;
        head = new_node;
        if (!tail) tail = head;
        size++;
    }

    bool is_empty() const { return head == nullptr; }
    
    void print_list() const {
        list_node<T>* current = head;
        while (current) {
            cout << current->data;
            if (current->next) cout << ", ";
            current = current->next;
        }
    }

    // helper for accessing data by index during parsing
    T get_at(int index) const {
        if (index < 0 || index >= size) return T();
        list_node<T>* current = head;
        int count = 0;
        while (current != nullptr) {
            if (count == index) return current->data;
            count++;
            current = current->next;
        }
        return T();
    }
};

// Custom Queue implementation for BFS
template <typename T>
class my_queue {
    LinkedList<T> list;
public:
    void enqueue(T val) { list.insert(val); }
    T dequeue() { return list.pop_front(); }
    bool empty() const { return list.is_empty(); }
};

// Custom Stack implementation for DFS
template <typename T>
class my_stack {
    LinkedList<T> list;
public:
    void push(T val) { list.push_front(val); }
    T pop() { return list.pop_front(); }
    bool empty() const { return list.is_empty(); }
};

// MovieNode Class
// Represents a single movie and its attributes.
// Also acts as a Graph Vertex and AVL Tree Node.
class MovieNode {
public:
    string title;
    string search_key; 
    string director;
    int year;
    float rating;
    int duration;

    // Attributes stored as lists
    LinkedList<string> actors;
    LinkedList<string> genres;

    // AVL Tree pointers
    MovieNode* left;
    MovieNode* right;
    int height;

    // Graph Edge List (Adjacency List)
    LinkedList<MovieNode*> neighbors; 
    
    // Traversal flags
    bool visited;
    MovieNode* parent; // For path reconstruction

    MovieNode(string t, int y, float r, int dur, string dir) {
        title = clean_str(t); 
        search_key = format_key(t);    
        year = y;
        rating = r;
        duration = dur;
        director = dir;
        left = right = nullptr;
        height = 1;
        visited = false;
        parent = nullptr;
    }

    void add_actor(string name) { 
        if(!actors.has_item(name)) actors.insert(name); 
    }
    
    void add_genre(string name) { 
        if(!genres.has_item(name)) genres.insert(name); 
    }
    
    // Creates a graph edge between this movie and another
    void add_link(MovieNode* other) {
        if (other == this) return; 
        if (!neighbors.has_item(other)) {
            neighbors.insert(other);
        }
    }

    void set_rating(float r) {
        this->rating = r;
        cout << "Rating for '" << title << "' updated to " << r << "/10" << endl;
    }

    // Copies data from another node (used during AVL deletion)
    void copy_data(MovieNode* other) {
        this->title = other->title;
        this->search_key = other->search_key;
        this->director = other->director;
        this->year = other->year;
        this->rating = other->rating;
        this->duration = other->duration;

        this->actors.clear();
        this->genres.clear();
        this->neighbors.clear(); 

        this->actors.head = other->actors.head;
        this->actors.tail = other->actors.tail;
        this->genres.head = other->genres.head;
        this->genres.tail = other->genres.tail;
        
        other->actors.head = other->actors.tail = nullptr;
        other->genres.head = other->genres.tail = nullptr;
    }

    void show_details() const {
        cout << "---------------------------------" << endl;
        cout << "Title:    " << title << " (" << year << ")" << endl;
        cout << "Director: " << director << endl;
        cout << "Rating:   " << rating << "/10" << endl;
        cout << "Cast:     "; actors.print_list(); cout << endl;
        cout << "Genres:   "; genres.print_list(); cout << endl;
        cout << "---------------------------------" << endl;
    }
};

// Hash Table Class
// Used to index movies by Actor, Genre, or Director.
struct ActorNode {
    string key; 
    LinkedList<MovieNode*> movies; 
    ActorNode* next; 
    ActorNode(string k) : key(k), next(nullptr) {}
};

class HashTable {
private:
    static const int tbl_size = 20011; // Large prime number to reduce collisions
    ActorNode* table[tbl_size];

    int calc_hash(string key) const {
        unsigned long h = 0;
        for (char c : key) h = (h * 31) + c;
        return h % tbl_size;
    }

public:
    HashTable() {
        for (int i = 0; i < tbl_size; i++) table[i] = nullptr;
    }

    ~HashTable() {
        for (int i = 0; i < tbl_size; i++) {
            ActorNode* curr = table[i];
            while (curr != nullptr) {
                ActorNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    // Inserts a movie into a specific bucket (Key: Actor/Genre Name)
    // Also builds the Graph: If movies share a bucket, they are connected.
    void insert_item(string raw_key, MovieNode* movie) {
        string k = format_key(raw_key);
        if (k == "") return;
        
        int idx = calc_hash(k);
        ActorNode* curr = table[idx];

        while (curr != nullptr) {
            if (curr->key == k) {
                if (curr->movies.has_item(movie)) return;

                // Create graph edges with existing movies in this bucket
                list_node<MovieNode*>* existing = curr->movies.head;
                int count = 0;
                while (existing != nullptr && count < max_links) {
                    movie->add_link(existing->data);
                    existing->data->add_link(movie);
                    existing = existing->next;
                    count++;
                }
                curr->movies.insert(movie);
                return;
            }
            curr = curr->next;
        }

        ActorNode* new_node = new ActorNode(k);
        new_node->movies.insert(movie);
        new_node->next = table[idx];
        table[idx] = new_node;
    }

    LinkedList<MovieNode*>* find_item(string key) {
        string k = format_key(key);
        int idx = calc_hash(k);
        ActorNode* curr = table[idx];
        while (curr != nullptr) {
            if (curr->key == k) return &curr->movies;
            curr = curr->next;
        }
        return nullptr;
    }

    // Removes a specific movie reference from an index bucket
    void remove_ref(string key, MovieNode* node) {
        string k = format_key(key);
        int idx = calc_hash(k);
        ActorNode* curr = table[idx];
        while (curr != nullptr) {
            if (curr->key == k) {
                curr->movies.remove(node);
                return;
            }
            curr = curr->next;
        }
    }
};

// AVL Tree Class
// Stores movies sorted by title, ensuring balanced height for efficient search.
class AVLTree {
private:
    MovieNode* root;
    HashTable* indexer; 

    int get_h(const MovieNode* n) const {
        if (n == nullptr) return 0;
        return n->height;
    }

    int get_bal(const MovieNode* n) const {
        if (n == nullptr) return 0;
        return get_h(n->left) - get_h(n->right);
    }

    // Right Rotation for balancing Left-Heavy trees
    MovieNode* rot_right(MovieNode* y) {
        MovieNode* x = y->left;
        MovieNode* t2 = x->right;
        x->right = y;
        y->left = t2;
        y->height = get_max(get_h(y->left), get_h(y->right)) + 1;
        x->height = get_max(get_h(x->left), get_h(x->right)) + 1;
        return x;
    }

    // Left Rotation for balancing Right-Heavy trees
    MovieNode* rot_left(MovieNode* x) {
        MovieNode* y = x->right;
        MovieNode* t2 = y->left;
        y->left = x;
        x->right = t2;
        x->height = get_max(get_h(x->left), get_h(x->right)) + 1;
        y->height = get_max(get_h(y->left), get_h(y->right)) + 1;
        return y;
    }

    // Recursively inserts a new node and rebalances the tree
    MovieNode* insert_rec(MovieNode* node, MovieNode* new_node) {
        if (node == nullptr) return new_node;
        
        if (new_node->search_key < node->search_key) 
            node->left = insert_rec(node->left, new_node);
        else if (new_node->search_key > node->search_key) 
            node->right = insert_rec(node->right, new_node);
        else 
            return node; // Duplicate keys not allowed in BST structure

        // Update height
        node->height = 1 + get_max(get_h(node->left), get_h(node->right));
        
        int bal = get_bal(node);

        // Perform rotations if unbalanced
        if (bal > 1 && new_node->search_key < node->left->search_key) 
            return rot_right(node);
        
        if (bal < -1 && new_node->search_key > node->right->search_key) 
            return rot_left(node);
        
        if (bal > 1 && new_node->search_key > node->left->search_key) {
            node->left = rot_left(node->left);
            return rot_right(node);
        }
        
        if (bal < -1 && new_node->search_key < node->right->search_key) {
            node->right = rot_right(node->right);
            return rot_left(node);
        }
        return node;
    }

    MovieNode* get_min(MovieNode* node) {
        MovieNode* curr = node;
        while (curr->left != nullptr) curr = curr->left;
        return curr;
    }

    // Cleans up pointers in the Hash Table and Neighbor lists before deleting a node
    void clear_node_refs(MovieNode* node) {
        if (!node || !indexer) return;
        list_node<MovieNode*>* n_ptr = node->neighbors.head;
        while (n_ptr) {
            if (n_ptr->data) {
                n_ptr->data->neighbors.remove(node);
            }
            n_ptr = n_ptr->next;
        }
        list_node<string>* a_ptr = node->actors.head;
        while (a_ptr) {
            indexer->remove_ref(a_ptr->data, node);
            a_ptr = a_ptr->next;
        }
        list_node<string>* g_ptr = node->genres.head;
        while (g_ptr) {
            indexer->remove_ref(g_ptr->data, node);
            g_ptr = g_ptr->next;
        }
    }

    // Re-adds node attributes to the Hash Table (used after swapping data during deletion)
    void reindex(MovieNode* node) {
        if (!indexer) return;
        list_node<string>* a_ptr = node->actors.head;
        while (a_ptr) {
            indexer->insert_item(a_ptr->data, node);
            a_ptr = a_ptr->next;
        }
        list_node<string>* g_ptr = node->genres.head;
        while (g_ptr) {
            indexer->insert_item(g_ptr->data, node);
            g_ptr = g_ptr->next;
        }
    }

    // Recursively deletes a node by key and rebalances
    MovieNode* delete_rec(MovieNode* root, string key) {
        if (root == nullptr) return root;

        if (key < root->search_key) root->left = delete_rec(root->left, key);
        else if (key > root->search_key) root->right = delete_rec(root->right, key);
        else {
            // Node with one or no child
            if ((root->left == nullptr) || (root->right == nullptr)) {
                MovieNode* temp = root->left ? root->left : root->right;
                if (temp == nullptr) {
                    temp = root;
                    clear_node_refs(temp);
                    root = nullptr;
                    delete temp;
                } else {
                    MovieNode* to_del = root;
                    root = temp; 
                    clear_node_refs(to_del);
                    delete to_del;
                }
            } else {
                // Node with two children: Get inorder successor
                MovieNode* temp = get_min(root->right);
                clear_node_refs(temp);
                clear_node_refs(root);
                root->copy_data(temp); 
                reindex(root);
                root->right = delete_rec(root->right, temp->search_key);
            }
        }

        if (root == nullptr) return root;

        root->height = 1 + get_max(get_h(root->left), get_h(root->right));
        int bal = get_bal(root);

        // Rebalance
        if (bal > 1 && get_bal(root->left) >= 0) return rot_right(root);
        if (bal > 1 && get_bal(root->left) < 0) {
            root->left = rot_left(root->left);
            return rot_right(root);
        }
        if (bal < -1 && get_bal(root->right) <= 0) return rot_left(root);
        if (bal < -1 && get_bal(root->right) > 0) {
            root->right = rot_right(root->right);
            return rot_left(root);
        }
        return root;
    }

    MovieNode* search_rec(MovieNode* root, string key) const {
        if (root == nullptr || root->search_key == key) return root;
        if (root->search_key > key) return search_rec(root->left, key);
        return search_rec(root->right, key);
    }

    void inorder_rec(const MovieNode* root) const {
        if (root != nullptr) {
            inorder_rec(root->left);
            cout << root->title << " (" << root->year << ")\n";
            inorder_rec(root->right);
        }
    }
    
    // Recursive traversal for Year Search
    void search_year_rec(const MovieNode* root, int y, bool& f) const {
        if(!root) return;
        search_year_rec(root->left, y, f);
        if(root->year == y) {
            cout << "- " << root->title << endl;
            f = true;
        }
        search_year_rec(root->right, y, f);
    }

    // Recursive traversal for Rating Search
    void search_rating_rec(const MovieNode* root, float min, float max, bool& f) const {
        if(!root) return;
        search_rating_rec(root->left, min, max, f);
        if(root->rating >= min && root->rating <= max) {
            cout << "- " << root->title << " [" << root->rating << "]" << endl;
            f = true;
        }
        search_rating_rec(root->right, min, max, f);
    }
    
    void destroy_rec(MovieNode* node) {
        if (node) {
            destroy_rec(node->left);
            destroy_rec(node->right);
            delete node;
        }
    }

    void reset_flags(MovieNode* node) {
        if(!node) return;
        node->visited = false;
        node->parent = nullptr;
        reset_flags(node->left);
        reset_flags(node->right);
    }

public:
    AVLTree() : root(nullptr), indexer(nullptr) {}
    ~AVLTree() { destroy_rec(root); }

    void set_idx(HashTable* ht) { indexer = ht; }
    void insert(MovieNode* n) { root = insert_rec(root, n); }
    
    void remove_node(string t) {
        if (!find_movie(t)) {
            cout << "Movie not found.\n";
            return;
        }
        root = delete_rec(root, format_key(t));
        cout << "Movie '" << t << "' deleted.\n";
    }
    
    MovieNode* find_movie(string t) { return search_rec(root, format_key(t)); }
    void print_all() const { inorder_rec(root); }
    
    void find_by_year(int y) const {
        bool f = false;
        cout << "\n--- Movies from " << y << " ---\n";
        search_year_rec(root, y, f);
        if(!f) cout << "None found.\n";
    }

    void find_by_rating(float min, float max) const {
        bool f = false;
        cout << "\n--- Movies rated " << min << " to " << max << " ---\n";
        search_rating_rec(root, min, max, f);
        if(!f) cout << "None found.\n";
    }
    
    void clear_flags() { reset_flags(root); }
};

// Graph Class
// Handles Recommendations (BFS/DFS) and Shortest Path logic.
class Graph {
public:
    // Recommendation using Breadth-First Search (BFS)
    // Finds immediate and close neighbors first.
    void recommend_bfs(MovieNode* start, AVLTree& tree, int limit) {
        if (!start) return;
        tree.clear_flags(); 

        my_queue<MovieNode*> q; 
        q.enqueue(start);
        start->visited = true;

        cout << "\n--- Top " << limit << " Recommendations for '" << start->title << "' ---\n";
        int count = 0;
        
        while (!q.empty()) {
            MovieNode* curr = q.dequeue();
            list_node<MovieNode*>* n_ptr = curr->neighbors.head;
            while (n_ptr != nullptr) {
                MovieNode* neighbor = n_ptr->data;
                if (neighbor && !neighbor->visited) {
                    neighbor->visited = true;
                    q.enqueue(neighbor);
                    if (neighbor != start) {
                        cout << "-> " << neighbor->title << " (" << neighbor->rating << "/10)\n";
                        count++;
                    }
                }
                n_ptr = n_ptr->next;
                if (count >= limit) return;
            }
        }
        if (count == 0) cout << "No related movies found.\n";
    }

    // Recommendation using Depth-First Search (DFS)
    // Explores deep into a specific genre/actor chain.
    void recommend_dfs(MovieNode* start, AVLTree& tree, int limit) {
        if (!start) return;
        tree.clear_flags();

        my_stack<MovieNode*> s;
        s.push(start);
        start->visited = true;

        cout << "\n--- DFS Recommendation for '" << start->title << "' ---\n";
        int count = 0;

        while (!s.empty()) {
            MovieNode* curr = s.pop();
            
            if (curr != start) {
                cout << "-> " << curr->title << "\n";
                count++;
            }
            if (count >= limit) break;

            list_node<MovieNode*>* n_ptr = curr->neighbors.head;
            while (n_ptr) {
                MovieNode* neighbor = n_ptr->data;
                if (neighbor && !neighbor->visited) {
                    neighbor->visited = true;
                    s.push(neighbor);
                }
                n_ptr = n_ptr->next;
            }
        }
    }

    // Finds the shortest path between two movies using BFS and parent pointers
    void shortest_path(MovieNode* start, MovieNode* end, AVLTree& tree) {
        if (!start || !end) return;
        tree.clear_flags();

        my_queue<MovieNode*> q;
        q.enqueue(start);
        start->visited = true;
        
        bool found = false;
        
        while (!q.empty()) {
            MovieNode* curr = q.dequeue();
            if (curr == end) {
                found = true;
                break;
            }
            list_node<MovieNode*>* n_ptr = curr->neighbors.head;
            while (n_ptr != nullptr) {
                MovieNode* neighbor = n_ptr->data;
                if (!neighbor->visited) {
                    neighbor->visited = true;
                    neighbor->parent = curr; 
                    q.enqueue(neighbor);
                }
                n_ptr = n_ptr->next;
            }
        }

        if (found) {
            cout << "\n--- Shortest Connection Path ---\n";
            print_path(end);
            cout << endl;
        } else {
            cout << "\nNo connection found.\n";
        }
    }

    // Connects two people (Actors/Director) via movies they participated in.
    void connect_actors(string a1, string a2, HashTable& idx, AVLTree& tree) {
        LinkedList<MovieNode*>* movies1 = idx.find_item(a1);
        if (!movies1) {
            cout << "Actor/Director 1 (" << a1 << ") not found.\n";
            return;
        }

        tree.clear_flags();
        my_queue<MovieNode*> q;

        // Initialize queue with all movies of the first person
        list_node<MovieNode*>* curr_mov = movies1->head;
        while(curr_mov) {
            curr_mov->data->visited = true;
            q.enqueue(curr_mov->data);
            curr_mov = curr_mov->next;
        }

        bool found = false;
        MovieNode* meet = nullptr;

        while(!q.empty()) {
            MovieNode* curr = q.dequeue();

            // Check if actor 2 is in cast
            list_node<string>* cast = curr->actors.head;
            while(cast) {
                if(format_key(cast->data) == format_key(a2)) {
                    found = true;
                    meet = curr;
                    break;
                }
                cast = cast->next;
            }
            // Check if person 2 is the director
            if (!found && format_key(curr->director) == format_key(a2)) {
                found = true;
                meet = curr;
            }
            
            if(found) break;

            list_node<MovieNode*>* n_ptr = curr->neighbors.head;
            while (n_ptr != nullptr) {
                MovieNode* neighbor = n_ptr->data;
                if (!neighbor->visited) {
                    neighbor->visited = true;
                    neighbor->parent = curr;
                    q.enqueue(neighbor);
                }
                n_ptr = n_ptr->next;
            }
        }

        if (found) {
            cout << "\n--- Connection Found! ---\n";
            cout << a1 << " is connected to " << a2 << " via:\n";
            print_path(meet);
            cout << " -> (Involved: " << a2 << ")\n";
        } else {
            cout << "No connection found between these actors/directors.\n";
        }
    }

    // Recursively prints path from end node back to start
    void print_path(MovieNode* node) const {
        if (node == nullptr) return;
        print_path(node->parent);
        if (node->parent != nullptr) cout << " -> ";
        cout << "[" << node->title << "]";
    }
};

// CSV Parsing Logic
LinkedList<string>* parse_csv_line(string line) {
    LinkedList<string>* tokens = new LinkedList<string>();
    string curr_tok = "";
    bool in_quotes = false;
    
    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        if (c == '\"') {
            if (in_quotes && i + 1 < line.length() && line[i + 1] == '\"') {
                curr_tok += '\"'; // handle escaped double quotes
                i++;
            } else {
                in_quotes = !in_quotes; 
            }
        } 
        else if (c == ',' && !in_quotes) {
            tokens->insert(curr_tok);
            curr_tok = "";
        } 
        else curr_tok += c;
    }
    tokens->insert(curr_tok); 
    return tokens;
}

// Data Loading Logic
// Reads the CSV, parses fields, creates nodes, and builds the graph.
void load_data(string fname, AVLTree& tree, HashTable& idx) {
    ifstream file(fname);
    if (!file.is_open()) {
        cout << "Could not open " << fname << endl;
        return;
    }
    
    cout << "Loading dataset... ";
    string line;
    int count = 0;
    int skipped = 0;
    int duplicates = 0;
    getline(file, line); // Skip Header

    while (getline(file, line)) {
        if (line.empty()) continue;
        
        LinkedList<string>* row = parse_csv_line(line);
        // Ensure row has enough columns (Index 25 is imdb_score)
        if (row->size <= 25) { 
            skipped++; 
            delete row; 
            continue; 
        }

        string raw_title = row->get_at(11); 
        string clean_t = clean_str(raw_title);
        
        if (clean_t.length() > 0) {
            // Check for duplicates
            if (tree.find_movie(clean_t) != nullptr) {
                duplicates++;
                delete row;
                continue; 
            }

            string d = clean_str(row->get_at(1));
            int dur = to_int(row->get_at(3));
            
            // Extract Actors
            string act1 = clean_str(row->get_at(10));
            string act2 = clean_str(row->get_at(6));
            string act3 = clean_str(row->get_at(14)); 

            string g_str = row->get_at(9);
            int y = to_int(row->get_at(23));
            float r = to_float(row->get_at(25));

            MovieNode* m = new MovieNode(clean_t, y, r, dur, d);

            // Index Actors and add to Node
            if (act1.length() > 1) { 
                m->add_actor(act1); 
                idx.insert_item(act1, m); 
            }
            if (act2.length() > 1) { 
                m->add_actor(act2); 
                idx.insert_item(act2, m); 
            }
            if (act3.length() > 1) { 
                m->add_actor(act3); 
                idx.insert_item(act3, m); 
            }

            // Index Director
            if (d.length() > 1) {
                idx.insert_item(d, m);
            }

            // Parse and Index Genres (separated by '|')
            string temp_g = "";
            for (char c : g_str) {
                if (c == '|') {
                    if (temp_g.length() > 1) {
                        m->add_genre(temp_g);
                        idx.insert_item(temp_g, m); 
                    }
                    temp_g = "";
                } else temp_g += c;
            }
            if (temp_g.length() > 1) {
                m->add_genre(temp_g);
                idx.insert_item(temp_g, m);
            }

            tree.insert(m);
            count++;
        } else {
            skipped++;
        }

        delete row;
    }
    file.close();

    cout << "Finished Loading!\n";
    cout << "Loaded: " << count << " | Skipped: " << skipped << " | Duplicates: " << duplicates << endl;
}

int get_valid_input() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(); 
    return x;
}

int main() {
    AVLTree tree;
    HashTable idx; 
    Graph graph;

    tree.set_idx(&idx);
    load_data("movie_metadata.csv", tree, idx);

    int choice;
    string in_str, in_str2;
    int y_in;
    float min_r, max_r;
    float new_r;
    int limit;

    do {
        cout << "\n=== MOVIES MANAGER ===\n";
        cout << "1. Display All\n";
        cout << "2. Search Title\n";
        cout << "3. Search Actor/Genre/Director\n";
        cout << "4. Search Year\n";
        cout << "5. Search Rating\n";
        cout << "6. Recommendations (BFS)\n";
        cout << "7. Recommendations (DFS)\n";
        cout << "8. Shortest Path (Movies)\n";
        cout << "9. Shortest Path (Actors/Directors)\n";
        cout << "10. Update Rating\n"; 
        cout << "11. Delete Movie\n";
        cout << "12. Find Co-Actors\n";
        cout << "13. Exit\n";
        cout << "Choice: ";
        
        choice = get_valid_input(); 

        switch(choice) {
            case 1: tree.print_all(); break;
            case 2:
                cout << "Title: "; getline(cin, in_str);
                { 
                    MovieNode* res = tree.find_movie(in_str); 
                    if (res) res->show_details(); 
                    else cout << "Not found.\n"; 
                }
                break;
            case 3:
                cout << "Actor/Genre/Director: "; getline(cin, in_str);
                {
                    LinkedList<MovieNode*>* res = idx.find_item(in_str);
                    if (res) {
                        cout << "\n--- Results ---\n";
                        list_node<MovieNode*>* curr = res->head;
                        while (curr) { cout << "- " << curr->data->title << endl; curr = curr->next; }
                    } else cout << "No matches found.\n";
                }
                break;
            case 4:
                cout << "Year: "; y_in = get_valid_input();
                tree.find_by_year(y_in);
                break;
            case 5:
                cout << "Min Rating: "; cin >> min_r;
                cout << "Max Rating: "; cin >> max_r; cin.ignore();
                tree.find_by_rating(min_r, max_r);
                break;
            case 6:
                cout << "Movie: "; getline(cin, in_str);
                cout << "Num recs: "; limit = get_valid_input();
                {
                    MovieNode* start = tree.find_movie(in_str);
                    if (start) graph.recommend_bfs(start, tree, limit);
                    else cout << "Movie not found.\n";
                }
                break;
            case 7:
                cout << "Movie: "; getline(cin, in_str);
                cout << "Num recs: "; limit = get_valid_input();
                {
                    MovieNode* start = tree.find_movie(in_str);
                    if (start) graph.recommend_dfs(start, tree, limit);
                    else cout << "Movie not found.\n";
                }
                break;
            case 8:
                cout << "Movie 1: "; getline(cin, in_str);
                cout << "Movie 2: "; getline(cin, in_str2);
                {
                    MovieNode* m1 = tree.find_movie(in_str);
                    MovieNode* m2 = tree.find_movie(in_str2);
                    if (m1 && m2) graph.shortest_path(m1, m2, tree);
                    else cout << "Movies not found.\n";
                }
                break;
            case 9: 
                cout << "Person 1: "; getline(cin, in_str);
                cout << "Person 2: "; getline(cin, in_str2);
                graph.connect_actors(in_str, in_str2, idx, tree);
                break;
            case 10:
                cout << "Title: "; getline(cin, in_str);
                {
                    MovieNode* res = tree.find_movie(in_str);
                    if (res) {
                        cout << "Current: " << res->rating << ". New: ";
                        cin >> new_r;
                        res->set_rating(new_r);
                    } else cout << "Not found.\n";
                }
                break;
            case 11: 
                cout << "Title to delete: "; getline(cin, in_str);
                tree.remove_node(in_str);
                break;
            case 12: 
                cout << "Actor: "; getline(cin, in_str);
                {
                    LinkedList<MovieNode*>* res = idx.find_item(in_str);
                    if (res) {
                        cout << "\n--- Co-Actors of " << in_str << " ---\n";
                        LinkedList<string> printed; 
                        list_node<MovieNode*>* m = res->head;
                        while(m) {
                            list_node<string>* a = m->data->actors.head;
                            while(a) {
                                if (format_key(a->data) != format_key(in_str) && !printed.has_item(a->data)) {
                                    cout << a->data << ", ";
                                    printed.insert(a->data);
                                }
                                a = a->next;
                            }
                            m = m->next;
                        }
                        cout << endl;
                    } else cout << "Actor not found.\n";
                }
                break;
            case 13: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 13);

    return 0;
}