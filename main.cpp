// Компілятор: MSVC 1950 (195035730)

#include <iostream>
#include <vector>
#include <random>
#include <compare>
#include <algorithm>
#include <numeric>
#include <map>
#include <stdexcept>
#include <iomanip>
#include <functional>
#include <limits>

using namespace std;

struct Card {
    int value;
    int suit;

    strong_ordering operator<=>(const Card& other) const {
        return value <=> other.value;
    }

    bool operator==(const Card& other) const {
        return value == other.value;
    }
};

class Dealer {
private:
    int cards_per_suit;
    vector<Card> deck;
    size_t current_index;
    mt19937 gen;

    void reset_and_shuffle() {
        deck.clear();
        for (int suit = 0; suit < 4; ++suit) {
            for (int val = 0; val < cards_per_suit; ++val) {
                deck.push_back(Card{ val, suit });
            }
        }
        ranges::shuffle(deck, gen);
        current_index = 0;
    }

public:
    explicit Dealer(int n) : cards_per_suit(n), current_index(0) {
        if (n <= 0) {
            throw invalid_argument("The number of cards of each suit must be a positive number.");
        }
        random_device rd;
        gen.seed(rd());
        reset_and_shuffle();
    }

    Card operator()() {
        if (current_index >= deck.size()) {
            reset_and_shuffle();
        }
        return deck[current_index++];
    }
};

int get_valid_input(const string& prompt, int min_val, int max_val) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min_val && value <= max_val) {
            return value;
        }
        cout << "Error: Enter an integer between " << min_val << " до " << max_val << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

struct StatsResult {
    double avg;
    double median;
};

StatsResult compute_avg_median(vector<int> stack_lengths) {
    if (stack_lengths.empty()) return { 0.0, 0.0 };

    double avg = accumulate(stack_lengths.begin(), stack_lengths.end(), 0.0, plus<double>())
        / static_cast<double>(stack_lengths.size());

    ranges::sort(stack_lengths, less<int>{});
    size_t n = stack_lengths.size();
    double median = (n % 2 == 0)
        ? (stack_lengths[n / 2 - 1] + stack_lengths[n / 2]) / 2.0
        : stack_lengths[n / 2];

    return { avg, median };
}

vector<int> simulate_game(int N, int M) {
    Dealer deal_card(N);
    vector<int> stack_lengths;

    if (M <= 0) return stack_lengths;

    Card prev_card = deal_card();
    int current_len = 1;

    for (int i = 1; i < M; ++i) {
        Card current_card = deal_card();

        if (current_card > prev_card) {
            current_len++;
        }
        else {
            stack_lengths.push_back(current_len);
            current_len = 1;
        }
        prev_card = current_card;
    }
    stack_lengths.push_back(current_len);

    return stack_lengths;
}

void calculate_and_print_stats(const vector<int>& stack_lengths) {
    size_t total_stacks = stack_lengths.size();
    if (total_stacks == 0) return;

    map<int, int> lengths_freq;
    ranges::for_each(stack_lengths, [&](int len) {
        lengths_freq[len]++;
        });

    int max_freq = 0;
    vector<int> modes;
    for (const auto& [len, freq] : lengths_freq) {
        if (freq > max_freq) {
            max_freq = freq;
            modes = { len };
        }
        else if (freq == max_freq) {
            modes.push_back(len);
        }
    }

    auto [avg, median] = compute_avg_median(stack_lengths);

    cout << "SIMULATION RESULTS" << endl;
    cout << "Total stacks formed: " << total_stacks << "\n\n";
    cout << "Frequency of stack lengths:\n";
    cout << fixed << setprecision(2);
    for (const auto& [len, count] : lengths_freq) {
        double percentage = (static_cast<double>(count) / total_stacks) * 100.0;
        cout << "  Length " << len << ": \t" << percentage << "% (" << count << " items.)\n";
    }

    cout << "Statistics: " << endl;
    cout << "Most frequent length: ";
    for (size_t i = 0; i < modes.size(); ++i) {
        cout << modes[i] << (i + 1 < modes.size() ? ", " : "");
    }
    cout << " (frequency: " << max_freq << ")\n";
    cout << "Medium length: " << avg << "\n";
    cout << "Median length: " << median << "\n";
}

void run_experiment() {
    cout << "EXPERIMENT: DEPENDENCE OF STATISTICS ON N (M = 100,000)" << endl;
    const int M_exp = 100000;
    vector<int> test_Ns = { 1, 2, 4, 9, 13, 26, 52 };

    cout << fixed << setprecision(2);
    for (int n : test_Ns) {
        vector<int> stacks = simulate_game(n, M_exp);
        auto [avg, median] = compute_avg_median(stacks);
        cout << "N = " << n << ": Medium = " << avg << ", Median = " << median << "\n";
    }
}

int main() {
    cout << "MSVC Version: " << _MSC_VER << " (" << _MSC_FULL_VER << ")\n";
    try {
        cout << "CARD DEALING SIMULATION (Problem 4)" << endl;

        int N = get_valid_input("Enter the number of cards of each suit N (1 - 1000): ", 1, 1000);
        int M = get_valid_input("Enter the total number of cards M (1 - 50000000): ", 1, 50000000);

        vector<int> results = simulate_game(N, M);
        calculate_and_print_stats(results);

        run_experiment();

    }
    catch (const exception& e) {
        cerr << "\n[CRITICAL ERROR] " << e.what() << "\n";
        return 1;
    }
    return 0;
}