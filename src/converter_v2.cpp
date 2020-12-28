//
// Converter of Exact Cover with Colors instance problems
// between C.Solnon's Dancing cells/Sparse sets input format
// and D.Knuth's DLX algorithm input format.
//
// Author: Loic Saos <lsaos@sniegu.fr>
// Date: 2020-10-23
// License: MIT
// Part of INSA Lyon PSAT with E.de Brye, Q.Ferro, R.Fournier
//
// Use: converter k2s|s2k [-i inputFilepath] [-o outputFilepath]
// k2s = Knuth -> Solnon format
// s2k = Solnon -> Knuth format
// 
// Standard stream is used if filepath is not specified for input
// or output.
// You must check converted data is not bigger than
// hardcoded maximum input of your algorithm.
//
// TODO:
// - Normalize names and colors (20 chars max for Solnon, 8 for Knuth + illegal values)
// - More checks for input validity
// - Option to pretty print instance
// - Option to remove duplicate options and other useless things (DLX-PRE by Knuth)
//

// Standard library includes
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Use standard library namespace
using namespace std;

// Class representing Exact Cover with Colors instance.
class ExactCoverInstance
{
public:
	// Value for not colored item.
	static constexpr size_t NoColor{ numeric_limits<size_t>::max() };

	// Item in set.
	struct Item
	{
		string name; // Name of item.
		bool primary; // True if the item is primary, false if secondary.
	};

	// Item in subset.
	struct OptionItem
	{
		size_t item; // Index of item.
		size_t color; // Index of color. Only for secondary items. NoColor means no color.
	};

	// Option is a subset, a list of items in set.
	typedef vector<OptionItem> Option;

	// Color is just an identifier.
	typedef string Color;

public:
	// Empty constructor.
	ExactCoverInstance()
		: m_secItemCount(0) {
	}

public:
	// Load instance from the specified stream.
	// Return false if loading failed.
	bool from(istream& s, bool knuthFormat, ostream& err) {
		if (knuthFormat) {
			return fromK(s, err);
		}
		else {
			return fromS(s, err);
		}
	}

	// Empty the instance.
	void clear() {
		m_options.clear();
		m_items.clear();
		m_colors.clear();
		m_secItemCount = 0;
	}

public:
	// Write instance to the specified stream.
	// Return false if writing failed.
	bool to(ostream& s, bool solnonFormat, ostream& err) const {
		if (solnonFormat) {
			return toS(s, err);
		}
		else {
			return toK(s, err);
		}
	}

	// Return true if the instance is valid, false otherwise.
	// If the instance is not valid error will be written to err.
	bool validate(ostream& err) const {
		if (m_items.empty()) {
			err << "No items in set" << endl;
			return false;
		}
		if (m_options.empty()) {
			err << "No subsets" << endl;
			return false;
		}
		if (m_secItemCount >= m_items.size()) {
			err << "Invalid number of secondary items" << endl;
			return false;
		}
		for (const Item& itm : m_items) {
			if (itm.name.empty()) {
				err << "Item name cannot be empty" << endl;
				return false;
			}
		}
		for (const Color& c : m_colors) {
			if (c.empty()) {
				err << "Color name cannot be empty" << endl;
				return false;
			}
		}
		for (size_t i = 0; i < m_items.size(); ++i) {
			if (m_items[i].primary != (i >= m_secItemCount)) {
				err << "Invalid primary/secondary item index" << endl;
				return false;
			}
		}
		for (size_t i = 1; i < m_items.size(); ++i) {
			for (size_t j = 0; j < i; ++j) {
				if (m_items[j].name == m_items[i].name) {
					err << "Duplicated item names '" << m_items[i].name << '\'' << endl;
					return false;
				}
			}
		}
		for (const Option& o : m_options) {
			if (o.empty()) {
				err << "Empty subset not supported" << endl;
				return false;
			}
			for (const OptionItem& oi : o) {
				if (oi.item >= m_items.size()) {
					err << "Item index " << oi.item << " out of bounds" << endl;
					return false;
				}
				if (oi.color != NoColor) {
					const Item& itm(m_items[oi.item]);
					if (itm.primary) {
						err << "Primary item '" << itm.name << "' cannot be colored" << endl;
						return false;
					}
					if (oi.color >= m_colors.size()) {
						err << "Color index " << oi.color << " out of bounds" << endl;
						return false;
					}
				}
			}
			for (size_t i = 1; i < o.size(); ++i) {
				for (size_t j = 0; j < i; ++j) {
					if (o[j].item == o[i].item) {
						err << "Duplicated items '" << m_items[o[i].item].name << "' in subset" << endl;
						return false;
					}
				}
			}
		}
		return true;
	}

private:
	// Load instance in Solnon format from the specified stream.
	// Return false if loading failed.
	bool fromS(istream& s, ostream& err) {
		clear();

		size_t itemCount(0);
		s >> itemCount;

		s >> m_secItemCount;

		size_t optionCount(0);
		s >> optionCount;

		string line;
		getline(s, line); // End of line

		for (size_t i = 0; i < itemCount; ++i) {
			getline(s, line);

			Item itm;
			itm.name = move(line);
			itm.primary = i >= m_secItemCount;
			m_items.emplace_back(move(itm));
		}

		for (size_t i = 0; i < optionCount; ++i) {
			getline(s, line);
			Option o;
			size_t j(0);
			while (!line.empty() && line.find_first_not_of(" \t") != string::npos) {
				OptionItem oi;
				oi.item = stoul(line, &j);
				line.erase(0, j);
				if (line.size() > 1 && line.front() == ':') {
					line.erase(line.begin());
					const Color color = string(1,(char)(stoul(line, &j) + 65));
					auto it(find(m_colors.cbegin(), m_colors.cend(), color));
					if (it != m_colors.cend()) {
						oi.color = distance(m_colors.cbegin(), it);
					}
					else {
						oi.color = m_colors.size();
						m_colors.emplace_back(color);
					}
					line.erase(0, j);
				}
				else {
					oi.color = NoColor;
				}
				o.push_back(oi);
			}
			m_options.emplace_back(move(o));
		}

		return true;
	}

	// Load instance in Knuth format from the specified stream.
	// Return false if loading failed.
	bool fromK(istream& s, ostream& err) {
		clear();

		string line;
		getline(s, line); // End of line
		if (line.empty()) {
			err << "Missing items line" << endl;
			return false;
		}
		istringstream si(line);
		Item itm;
		itm.primary = true;
		while (si >> itm.name) {
			if (itm.name == "|") {
				itm.primary = false;
			}
			else {
				if (!itm.primary) {
					++m_secItemCount;
				}
				m_items.push_back(itm);
			}
		}
		// Need to reverse because secondary items are after primary
		reverse(m_items.begin(), m_items.end());

		while (getline(s, line)) {
			if (line.find_first_not_of(" \t") != string::npos) {
				Option o;
				size_t i(0);
				while (i < line.size()) {
					size_t nameStartIndex(line.find_first_not_of(" \t", i));
					if (nameStartIndex == string::npos) {
						nameStartIndex = i;
					}
					const size_t nameEndIndex(line.find_first_of(": \t", nameStartIndex));
					if (nameEndIndex == nameStartIndex) {
						break;
					}
					const string name(line.substr(nameStartIndex, nameEndIndex - nameStartIndex));
					i = nameEndIndex;
					auto it(find_if(m_items.cbegin(), m_items.cend(), [name](const Item& i) -> bool { return i.name == name; }));
					if (it == m_items.cend()) {
						err << "Unknown item name '" << name << '"' << endl;
						return false;
					}
					OptionItem oi;
					oi.item = distance(m_items.cbegin(), it);
					if (i + 1 < line.size() && line[i] == ':') {
						const Color color(1, line[i + 1]);
						i += 2;
						auto it(find(m_colors.cbegin(), m_colors.cend(), color));
						if (it != m_colors.cend()) {
							oi.color = distance(m_colors.cbegin(), it);
						}
						else {
							oi.color = m_colors.size();
							m_colors.emplace_back(color);
						}
					}
					else {
						oi.color = NoColor;
					}
					o.push_back(oi);
				}
				m_options.emplace_back(move(o));
			}
		}

		return true;
	}

private:
	// Write instance in Solnon format to the specified stream.
	// Return false if writing failed.
	bool toS(ostream& s, ostream& err) const {
		s << m_items.size() << ' ' << m_secItemCount << ' ' << m_options.size() << endl;
		for (const Item& itm : m_items) {
			s << itm.name << endl;
		}
		for (const Option& o : m_options) {
			bool firstOptItem(true);
			for (const OptionItem& oi : o) {
				if (firstOptItem) {
					firstOptItem = false;
				}
				else {
					s << ' ';
				}
				s << oi.item;
				if (oi.color != NoColor) {
					s << ':' << oi.color;
				}
			}
			s << endl;
		}
		return true;
	}

	// Write instance in Knuth format to the specified stream.
	// Return false if writing failed.
	bool toK(ostream& s, ostream& err) const {
		bool lastItemPrimary(true);
		// Need to reverse because secondary items are after primary
		for (auto it = m_items.crbegin(); it != m_items.crend(); ++it) {
			const Item& itm(*it);
			if (lastItemPrimary != itm.primary) {
				lastItemPrimary = itm.primary;
				s << "| ";
			}
			s << itm.name << ' ';
		}
		s << endl;
		for (const Option& o : m_options) {
			for (const OptionItem& oi : o) {
				s << m_items[oi.item].name;
				if (oi.color != NoColor) {
					s << ':' << m_colors[oi.color];
				}
				s << ' ';
			}
			s << endl;
		}
		return true;
	}

private:
	vector<Option> m_options; // List of options.
	vector<Item> m_items; // Names of items in set.
	vector<Color> m_colors; // Names of used colors.
	size_t m_secItemCount; // Number of secondary items.
};

// Entry point.
int main(int argc, char* argv[])
{
	// Read parameters
	bool k2s(false);
	bool valid(false);

	if (argc > 1) {
		const string mode(argv[1]);
		if (mode == "k2s") {
			k2s = true;
			valid = true;
		}
		else if (mode == "s2k") {
			k2s = false;
			valid = true;
		}
	}

	string inputFilepath;
	string outputFilepath;

	for (int i = 2; i < argc; ++i) {
		const string opt(argv[i]);

		if ((opt == "-i" || opt == "-o") && i + 1 < argc) {
			const string filePath(argv[i + 1]);
			++i;

			if (opt == "-i") {
				inputFilepath = filePath;
			}
			else {
				outputFilepath = filePath;
			}
		}
		else {
			valid = false;
			break;
		}
	}

	if (!valid) {
		cout << "Use: converter k2s|s2k [-i inputFilepath] [-o outputFilepath]" << endl
			<< "k2s = Knuth -> Solnon format" << endl
			<< "s2k = Solnon -> Knuth format" << endl;
		return -1;
	}

	ExactCoverInstance instance;

	// Load instance
	if (inputFilepath.empty()) {
		if (!instance.from(cin, k2s, cerr)) {
			return -2;
		}
	}
	else {
		ifstream in(inputFilepath);
		if (!in) {
			cerr << "Failed to open file '" << inputFilepath << "' for reading" << endl;
			return -3;
		}

		if (!instance.from(in, k2s, cerr)) {
			return -2;
		}
	}

	// Validate data
	if (!instance.validate(cerr)) {
		return -4;
	}

	// Write instance in new format
	if (outputFilepath.empty()) {
		if (!instance.to(cout, k2s, cerr)) {
			return -5;
		}
	}
	else {
		ofstream out(outputFilepath);
		if (!out) {
			cerr << "Failed to open file '" << outputFilepath << "' for writing" << endl;
			return -3;
		}

		if (!instance.to(out, k2s, cerr)) {
			return -5;
		}
	}

	return 0;
}
