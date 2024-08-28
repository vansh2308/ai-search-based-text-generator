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
#define ll long long
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

double round_to(double value, double precision = 0.0001){
    return round(value / precision) * precision;
}

vector<vector<double> > mat;
vector<double> heuristic;
vector<string> vocab;
int L, n;

class Node{
private:
public:
    int i, seq_len;
    vector<int> seq;
    double cum_cost, hn;
    string type;

    Node(int i, double cum_cost, int seq_len, vector<int> seq, double hn = 0, string type = "ucs") {
        this->i = i;
        this->cum_cost = cum_cost;
        this->hn = hn;
        this->seq_len = seq_len;
        this->seq = seq;
        this->type = type;
    }

    double get_fn() {
        if (this->type == "ucs") {
            return this->cum_cost;
        }
        else if (this->type == "greedy") {
            return this->hn;
        }
        else {
            return this->cum_cost + this->hn;
        }
    }

    void display() {
        printf("%d %f\n", i, cum_cost);
    }
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

vector<int> search(int &total, string type = "ucs") {
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
            double child_cost = round_to(curr.cum_cost * mat[row][i]);
            vector<int> child_seq = curr.get_seq();
            child_seq.push_back(i);
            Node child(i, child_cost, curr.seq_len + 1, child_seq, heuristic[i], type);
            pq.push(child);
        }
    }

    return res;
}

double score(vector<int> &seq) {
    double score = 1;
    for (int i = 1; i < seq.size(); i++) {
        int token_idx = seq[i], prev = seq[i - 1];
        score *= mat[prev == -1 ? L : prev][seq[i]];
        score = round_to(score);
    }
    score *= mat[L + 1][seq[n]];
    return score;
}

void normalize() {
    for (int i = 0; i < L + 2; i++) {
        double row_sum = accumulate(mat[i].begin(), mat[i].end(), 0.0);
        for (int j = 0; j < L; j++) {
            mat[i][j] /= (row_sum > 0 ? row_sum : 1);
        }
    }
}

int main() {
    ifstream vocab_file("./vocab2.txt"), trans_file("./trans2.txt");
    string str;

    cout << endl;

    while (getline(vocab_file, str)) {
        vocab.push_back(str);
    }

    while (getline(trans_file, str)) {
        mat.push_back(vector<double>());
        while (str.find(' ') != string::npos) {
            string word = str.substr(0, str.find(' '));
            str = str.substr(str.find(' ') + 1);
            mat.back().push_back(stod(word));
        }
        mat.back().push_back(stod(str));
    }

    n = 9;
    L = mat.size() - 2;

    normalize();

    heuristic = mat.back();

    string types[3] = {"ucs", "greedy", "astar"};
    for (int _ = 0; _ < 3; _++) {
        printf("===================== %s =====================\n", types[_].c_str());
        int nodes_explored = 0;

        clock_t begin = clock();
        vector<int> idxs = search(nodes_explored, types[_]);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

        printf("Total score: %f\n", score(idxs));
        printf("Time elapsed: %f\n", elapsed_secs);
        printf("Total nodes explored: %d\n", nodes_explored);
        string ans = "\n";

        for (int i = 0; i < idxs.size(); i++) {
            ans += idxs[i] == -1 ? "<sos>" : " " + vocab[idxs[i]];
        }
        ans += " <eos>\n";
        cout << ans << endl;
    }

    return 0;
}