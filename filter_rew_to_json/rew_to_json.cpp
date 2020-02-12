#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using json = nlohmann::json;

struct PeakingFilter {
    int fc;
    double gain;
    double q;
};

using Filters = vector<PeakingFilter>;

Filters parse(const string &filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "No file with name " << filename << endl;
        throw exception();
    }

    Filters filters;
    string line;
    while (getline(file, line)) {
        istringstream stream(line);
        string tmp;
        stream >> tmp;
        if (tmp == "Filter") {
            stream >> tmp;
            if (tmp == "Settings") {
                continue;
            }
        } else {
            continue;
        }

        // At this point tmp contains filter number, ignore it
        stream >> tmp;
        if (tmp != "ON") {
            continue;
        }

        PeakingFilter filter;
        stream >> tmp; // Ignore filter type
        if (tmp != "PK") {
            continue;
        }
        stream >> tmp; // Ignore Fc
        stream >> tmp;
        filter.fc = lround(stod(tmp));
        stream >> tmp; // Hz
        stream >> tmp; // Gain
        stream >> tmp;
        filter.gain = stod(tmp);
        stream >> tmp;
        stream >> tmp;
        stream >> tmp;
        filter.q = stod(tmp);

        cout << "Read filter with frequency " << filter.fc << " gain " << filter.gain << " and Q " << filter.q << endl;
        filters.push_back(filter);
    }

    file.close();
    return filters;
}

void write_json(const Filters &filters) {
    const string API_KEY = "api_1_eq";
    const string TYPE_KEY = "type";
    const string BANDS_KEY = "bands";
    const string PARAMETRIC = "parametric";
    const string FREQUENCY = "freq";
    const string GAIN = "gain";
    const string Q = "q";
    const string TYPE_PEAKING = "peaking";
    json eq;
    json bands = json::array();

    eq[API_KEY][TYPE_KEY] = PARAMETRIC;
    for (size_t i = 0; i < filters.size(); i++) {
        json band;
        band[FREQUENCY] = filters.at(i).fc;
        band[GAIN] = filters.at(i).gain;
        band[Q] = filters.at(i).q;
        band[TYPE_KEY] = TYPE_PEAKING;
        bands.push_back(band);
    }
    eq[API_KEY][BANDS_KEY] = bands;

    ofstream file("json_filters.txt");
    file << setw(2) << eq;
    file.close();
}

void print_help(const string &name) {
    cout << "Usage: " << name << " [filename]\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return -1;
    }

    Filters filters;
    try {
        filters = parse(argv[1]);
    } catch (...) {
        return -1;
    }

    write_json(filters);
    return 0;
}