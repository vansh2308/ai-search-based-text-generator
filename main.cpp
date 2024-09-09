#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <queue>
#include <numeric>
#include <ctime>
#include <cstdlib>
#include <iomanip>

#define ll long long
#define ld double
#define modn 100
#define VANSHU2308                \
    ios_base::sync_with_stdio(0); \
    cin.tie(0);                   \
    cout.tie(0);

using namespace std;

template <typename T>
void display(vector<T> &v){
    for (int i = 0; i < v.size(); i++) { cout << v[i] << " ";}
    cout << endl;
}

template <typename T>
void display_mat(vector<vector<T> > &v){
    cout << endl;
    int m = v.size(), n = v[0].size();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) { cout << v[i][j] << " "; }
        cout << endl;
    }
    cout << endl;
}

ld round_to(ld value, ld precision = 0.0001){
    return round(value / precision) * precision;
}

vector<vector<ld> > mat;
vector<ld> heuristic;
vector<string> vocab;
int L, n;

class Node{
private:
public:
    int i, seq_len;
    vector<int> seq;
    ld cum_cost, hn;
    string type;

    Node(int i, ld cum_cost, int seq_len, vector<int> seq, ld hn = 0, string type = "ucs") {
        this->i = i;
        this->cum_cost = cum_cost;
        this->hn = hn;
        this->seq_len = seq_len;
        this->seq = seq;
        this->type = type;
    }

    ld get_fn() {
        if (this->type == "UCS") {
            return this->cum_cost;
        }
        else if (this->type == "Greedy") {
            return this->hn;
        }
        else if (this->type == "A_star") {
            return this->cum_cost + this->hn;
        }
        return 0;
    }

    // void display() {
    //     printf("%d %f\n", i, cum_cost);
    // }
    vector<int> get_seq() {
        return this->seq;
    }
};

class Fringe {
private:
public:
    bool operator()(Node i, Node j) {
        return i.get_fn() < j.get_fn();
    }
};

vector<int> search(int &total, string type = "UCS") {
    vector<int> res;
    priority_queue<Node, vector<Node>, Fringe> pq;
    vector<int> init_seq;
    init_seq.push_back(-1);
    Node node(-1, 1, 0, init_seq, INT_MAX, type);
    pq.push(node);

    while (!pq.empty()) {
        Node curr = pq.top();
        total++;

        if (curr.seq_len == n) {
            // printf("Node cost : %f\n", curr.get_fn());
            res = curr.get_seq();
            break;
        }

        pq.pop();

        for (int i = 0; i < L; i++) {
            if (i == curr.i) { continue; }

            int row = curr.i == -1 ? L : curr.i;
            ld child_cost = round_to(curr.cum_cost * mat[row][i]);
            vector<int> child_seq = curr.get_seq();
            child_seq.push_back(i);
            Node child(i, child_cost, curr.seq_len + 1, child_seq, heuristic[i], type);
            pq.push(child);
        }
    }

    return res;
}

ld score(vector<int> &seq) {
    ld score = 1;
    for (int i = 1; i < seq.size(); i++) {
        int token_idx = seq[i], prev = seq[i - 1];
        score *= mat[prev == -1 ? L : prev][seq[i]];
    }

    // score *= mat[L + 1][seq[n]];
    return score;
}

void normalize() {
    for (int i = 0; i < L + 2; i++) {
        ld row_sum = accumulate(mat[i].begin(), mat[i].end(), 0.0);
        for (int j = 0; j < L; j++) {
            mat[i][j] /= (row_sum > 0 ? row_sum : 1);
        }
    }
}


void generate_random_transition(){
    mat = vector<vector<ld> > (L+2, vector<ld>(L, 0));
    for(int i=0; i<L+2; i++){
        for(int j=0; j<L; j++){
            mat[i][j] = rand() % modn;
        }
    }
    normalize();
}


int main() {
    // Implementation for working example 
    ifstream vocab_file("./vocab2.txt"), trans_file("./trans2.txt");
    string str;

    cout << endl;

    while (getline(vocab_file, str)) {
        vocab.push_back(str);
    }

    while (getline(trans_file, str)) {
        mat.push_back(vector<ld>());
        while (str.find(' ') != string::npos) {
            string word = str.substr(0, str.find(' '));
            str = str.substr(str.find(' ') + 1);
            mat.back().push_back(stod(word));
        }
        mat.back().push_back(stod(str));
    }

    n = 10;
    L = mat.size() - 2;

    normalize();

    heuristic = mat.back();

    string types[] = {"UCS", "A_star", "Greedy"};
    for (int _ = 0; _ < 3; _++) {
        printf("===================== %s =====================\n", types[_].c_str());
        int nodes_explored = 0;

        clock_t begin = clock();
        vector<int> idxs = search(nodes_explored, types[_]);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

        printf("Time elapsed: %f\n", elapsed_secs);
        printf("Total nodes explored: %d\n", nodes_explored);
        string ans = "\n";

        for (int i = 0; i < idxs.size(); i++) {
            ans += idxs[i] == -1 ? "<sos>" : " " + vocab[idxs[i]];
        }
        ans += " <eos>\n";
        cout << ans << endl;
    }


    cout << endl << endl;
    // Time complexity Analysis 

    // <type, enum(time, nodes_exp), val>  
    vector<vector<vector<ld> > > stats(3, vector<vector<ld> >(2));
    

    for(L=5; L<=15; L+=5){
        generate_random_transition();
        heuristic = mat.back();

        for(n=3; n<8; n++){

            for (int _ = 0; _ < 3; _++) {
                int nodes_explored = 0;

                clock_t begin = clock();
                vector<int> idxs = search(nodes_explored, types[_]);
                clock_t end = clock();
                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

                stats[_][0].push_back(elapsed_secs);
                stats[_][1].push_back(nodes_explored);
            }

        }

    }

    cout << "---------------------- Time Complexity Analsyis -----------------------\n\n" ; 
    std::cout << std::left;
	std::cout <<
	    setw(25) << "[Algorithm]" << setw(25) << "[Avg Time] (secs)" << setw(25) << "[Avg Nodes explored]" << '\n' 
        << setw(30) <<  "-----------------------------------------------------------------------\n" << 
	    setw(25) << "UCS"   << setw(25) << accumulate(stats[0][0].begin(), stats[0][0].end(), 0.0)/15   << setw(25) << accumulate(stats[0][1].begin(), stats[0][1].end(), 0)/15   << '\n' <<
	    setw(25) << "A*"  << setw(25) << accumulate(stats[1][0].begin(), stats[1][0].end(), 0.0)/15     << setw(25) << accumulate(stats[1][1].begin(), stats[1][1].end(), 0)/15      << '\n' <<
	    setw(25) << "Greedy"   << setw(25) << accumulate(stats[2][0].begin(), stats[2][0].end(), 0.0)/15     << setw(25) << accumulate(stats[2][1].begin(), stats[2][1].end(), 0)/15      << '\n';
    cout << endl;
    return 0;
}

