#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using json = nlohmann::json;

struct Filter {
    string type = "";
    int fc = -1;
    double gain = 0;
    double q = -1;
};

using Filters = vector<Filter>;

const string FILTER_PEAKING = "peaking";
const string FILTER_HIGH_SHELF = "high_shelf";
const string FILTER_LOW_SHELF = "low_shelf";
const string FILTER_HIGH_PASS = "high_pass";
const string FILTER_LOW_PASS = "low_pass";

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

        Filter filter;
        string actual_type;
        stream >> tmp; // Filter type
        actual_type = tmp;
        if (tmp == "PK") {
            filter.type = FILTER_PEAKING;
        } else if (tmp == "LPQ" || tmp == "LP") {
            filter.type = FILTER_LOW_PASS;
        } else if (tmp == "HPQ" || tmp == "HP") {
            filter.type = FILTER_HIGH_PASS;
        } else if (tmp == "LSQ" || tmp == "LS") {
            filter.type = FILTER_LOW_SHELF;
        } else if (tmp == "HSQ" || tmp == "HS") {
            filter.type = FILTER_HIGH_SHELF;
        } else {
            // Invalid
            cout << "Skipping invalid filter " << tmp << endl;
            continue;
        }
        // Warn about LSQ/HSQ
        if (tmp == "LSQ" || tmp == "HSQ") {
            cout << "LSQ/HSQ are currently being exported without Q from REW, consider adding Q manually after conversion is done\n";
        }
        stream >> tmp; // Ignore Fc
        stream >> tmp; // Frequency
        filter.fc = lround(stod(tmp));
        stream >> tmp; // Hz
        if (filter.type == FILTER_PEAKING || filter.type == FILTER_LOW_SHELF || filter.type == FILTER_HIGH_SHELF) {
            stream >> tmp; // Gain
            stream >> tmp; // Gain value
            filter.gain = stod(tmp);
            stream >> tmp; // dB
        }
        if (filter.type == FILTER_PEAKING || actual_type == "LPQ" || actual_type == "HPQ") {
            stream >> tmp; // Q
            stream >> tmp; // Q value
            filter.q = stod(tmp);
        }

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
    json eq;
    json bands = json::array();

    eq[API_KEY][TYPE_KEY] = PARAMETRIC;
    for (size_t i = 0; i < filters.size(); i++) {
        json band;
        band[FREQUENCY] = filters.at(i).fc;
        if (filters.at(i).q >= 0) {
            band[Q] = filters.at(i).q;
        }
        if (filters.at(i).type != FILTER_HIGH_PASS && filters.at(i).type != FILTER_LOW_PASS) {
            band[GAIN] = filters.at(i).gain;
        }
        band[TYPE_KEY] = filters.at(i).type;
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