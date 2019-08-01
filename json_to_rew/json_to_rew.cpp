#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using json = nlohmann::json;

struct PeakingFilter {
    int fc;
    double gain;
    double q;
    string type;
};

using Filters = vector<PeakingFilter>;

void print_help(const string &name) {
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return -1;
    }

    // Parse JSON
    ifstream json_file(argv[1]);
    if (!json_file) {
        cout << argv[1] << " does not exist\n";
        return -1;
    }
    json json_filters;
    json_file >> json_filters;
    json_file.close();

    Filters filters;
    json eq = json_filters["api_1_eq"]["bands"];
    for (auto &band : eq) {
        auto type = band["type"];
        auto q = band["q"];
        auto fc = band["freq"];
        auto gain = band["gain"];

        cout << type << " " << q << " " << fc << " " << gain << endl;
        PeakingFilter filter;
        filter.fc = fc;
        filter.type = type;
        if (type == "peaking") {
            filter.q = q;
        }
        if (type == "peaking" || type == "low_shelf" || type == "high_shelf") {
            filter.gain = gain;
        }
        filters.push_back(filter);
    }

    // Write to file
    ofstream file(string(argv[1]) + ".txt");
    for (auto &filter : filters) {
        file << "Filter 0: ON ";
        if (filter.type == "low_pass") {
            file << "LP";
        } else if (filter.type == "high_pass") {
            file << "HP";
        } else if (filter.type == "low_shelf") {
            file << "LS";
        } else if (filter.type == "high_shelf") {
            file << "HS";
        } else {
            file << "PK";
        }
        file << " Fc " << filter.fc << " Hz ";
        if (filter.type == "low_shelf" || filter.type == "high_shelf" || filter.type == "peaking") {
            file << "Gain " << filter.gain << " dB ";
        }
        if (filter.type == "peaking") {
            file << "Q " << filter.q;
        }
        file << endl;
    }
    file.close();

    return 0;
}