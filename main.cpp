#include <iostream>
#include <cmath>
#include <stack>
#include <vector>

using namespace std;

// types
// 0 - item
// 1 - +
// 2 - -
// 3 - *
// 4 - /
// 5 - ^
// 6 - (
// 7 - )

struct token {
    int type{};
    double value{};
};

bool try_catch(vector<token> &v, string &s) {
    try {
        v.emplace_back(0, stod(s));
    } catch (const out_of_range &e) {
        cerr << "NUMBER TOO BIG" << endl;
        return false;
    } catch (...) {
        cerr << "INVALID INPUT" << endl;
        return false;
    }
    return true;
}

optional<vector<token> > tokenize(string &s) {
    vector<token> tokens;
    string v;
    for (auto it: s) {
        if (!(it >= '0' && it <= '9') && !(it == '.' || it == ',') && !v.empty()) {
            if (!try_catch(tokens, v)) return nullopt;
            v.clear();
        }
        if (it == '-') {
            if (tokens.empty() || tokens[(int) tokens.size() - 1].type != 0) v.push_back('-');
            else tokens.emplace_back(2, 0.0);
        } else if (it == '+') tokens.emplace_back(1, 0.0);
        else if (it == '*') tokens.emplace_back(3, 0.0);
        else if (it == '/') tokens.emplace_back(4, 0.0);
        else if (it == '^') tokens.emplace_back(5, 0.0);
        else if (it == '(') tokens.emplace_back(6, 0.0);
        else if (it == ')') tokens.emplace_back(7, 0.0);
        else if (it == '.' || it == ',') {
            v.push_back('.');
        } else if (it >= '0' && it <= '9') {
            v.push_back(it);
        } else if (it != ' ') {
            cerr << "INVALID INPUT" << endl;
            return nullopt;
        }
    }
    if (!v.empty()) {
        if (!try_catch(tokens, v)) return nullopt;
        v.clear();
    }
    return tokens;
}

int priority(int &type) {
    if (type == 1 || type == 2) return 1;
    if (type == 3 || type == 4) return 2;
    if (type == 5) return 3;
    return 4;
}

optional<vector<token> > shunting_yard(const optional<vector<token> > &input) {
    vector<token> tokens;
    stack<token> s;
    for (auto it: *input) {
        if (it.type == 0) {
            tokens.emplace_back(it);
        } else {
            if (it.type == 6) {
                s.push(it);
            } else if (it.type == 7) {
                while (!s.empty() && s.top().type != 6) {
                    tokens.emplace_back(s.top());
                    s.pop();
                }
                if (s.empty()) {
                    cerr << "INVALID INPUT" << endl;
                    return nullopt;
                }
                s.pop();
            } else {
                while (!s.empty() && s.top().type != 6 &&
                       (it.type != 5
                            ? priority(it.type) <= priority(s.top().type)
                            : priority(it.type) < priority(s.top().type))) {
                    tokens.emplace_back(s.top());
                    s.pop();
                }
                s.push(it);
            }
        }
    }
    while (!s.empty()) {
        tokens.emplace_back(s.top());
        s.pop();
    }
    return tokens;
}

[[noreturn]] int main() {
    while (true) {
        string input;
        getline(cin, input);
        auto tokens = tokenize(input);
        if (!tokens) continue;
        auto rpn = shunting_yard(tokens);
        if (!rpn) continue;
        stack<token> s;
        bool e = false;
        for (auto it: *rpn) {
            if (it.type == 0) {
                s.push(it);
            } else {
                if (s.size() < 2) {
                    cerr << "INVALID INPUT" << endl;
                    e = true;
                    break;
                }
                double c1 = s.top().value;
                s.pop();
                double c2 = s.top().value;
                s.pop();
                if (it.type == 1) {
                    s.push({0, c2 + c1});
                }
                if (it.type == 2) {
                    s.push({0, c2 - c1});
                }
                if (it.type == 3) {
                    s.push({0, c2 * c1});
                }
                if (it.type == 4) {
                    s.push({0, c2 / c1});
                    if (isinf(s.top().value) || isnan(s.top().value)) {
                        cerr << "CALCULATION ERROR" << endl;
                        e = true;
                        break;
                    }
                }
                if (it.type == 5) {
                    s.push({0, pow(c2, c1)});
                    if (isinf(s.top().value) || isnan(s.top().value)) {
                        cerr << "CALCULATION ERROR" << endl;
                        e = true;
                        break;
                    }
                }
            }
        }
        if (e) continue;
        if (s.size() == 1) {
            cout << s.top().value << endl;
        } else {
            cerr << "INVALID INPUT" << endl;
        }
    }
}
