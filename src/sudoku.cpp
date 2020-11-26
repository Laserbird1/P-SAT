#include <string>
#include <vector>
#include <iostream>

using namespace std;

typedef pair<int, int> Item;

struct Option
{
	size_t lineColumn;
	size_t columnNumber;
	size_t lineNumber;
	size_t boxNumber;
};

// Get box index from (column, line) position.
int getBox(int column, int line, int n)
{
	return ((line - 1) / n) * n + (column - 1) / n;
}

int main(int argc, char* argv[])
{
	// Parse n parameter
	int n(3);

	if (argc > 1) {
		try {
			n = stoi(argv[1]);
		}
		catch (...) {
			n = 0;
		}
	}

	if (n <= 1) {
		cout << "Use: sudoku n (integer > 1)" << endl;
		return -1;
	}

	const int numberCount(n * n);
	const int boxCount(numberCount);
	const int lineCount(numberCount);
	const int columnCount(numberCount);

	// Create list of each kind of items.
	vector<Item> lineColumns;
	vector<Item> columnNumbers;
	vector<Item> lineNumbers;
	vector<Item> boxNumbers;

	// Create all items.
	for (int line = 1; line <= lineCount; ++line) {
		for (int column = 1; column <= columnCount; ++column) {
			lineColumns.push_back({ column, line });
		}
	}
	for (int number = 1; number <= numberCount; ++number) {
		for (int line = 1; line <= lineCount; ++line) {
			lineNumbers.push_back({ line, number });
		}
		for (int column = 1; column <= columnCount; ++column) {
			columnNumbers.push_back({ column, number });
		}
		for (int box = 0; box < boxCount; ++box) {
			boxNumbers.push_back({ box, number });
		}
	}

	// Remove items already defined.
	int inputColumn(0);
	while (cin >> inputColumn && inputColumn > 0 && inputColumn <= columnCount) {
		int line(0);
		int number(0);
		cin >> line >> number;

		if (line <= 0 || line > lineCount) {
			cerr << "Invalid line " << line << endl;
			continue;
		}
		if (number <= 0 || number > numberCount) {
			cerr << "Invalid number " << number << endl;
			continue;
		}

		auto itLc(find(lineColumns.begin(), lineColumns.end(), Item{ inputColumn, line }));
		if (itLc == lineColumns.end()) {
			cerr << "Position " << inputColumn << ", " << line << " already entered" << endl;
			continue;
		}
		auto itLn(find(lineNumbers.begin(), lineNumbers.end(), Item{ line, number }));
		if (itLn == lineNumbers.end()) {
			cerr << "Number " << number << " already entered for line " << line << endl;
			continue;
		}
		auto itCn(find(columnNumbers.begin(), columnNumbers.end(), Item{ inputColumn, number }));
		if (itCn == columnNumbers.end()) {
			cerr << "Number " << number << " already entered for column " << inputColumn << endl;
			continue;
		}

		const int box(getBox(inputColumn, line, n));
		auto itBn(find(boxNumbers.begin(), boxNumbers.end(), Item{ box, number }));
		if (itBn == boxNumbers.end()) {
			cerr << "Number " << number << " already entered for box " << box << endl;
			continue;
		}

		lineColumns.erase(itLc);
		lineNumbers.erase(itLn);
		columnNumbers.erase(itCn);
		boxNumbers.erase(itBn);
	}

	const size_t primaryItemCount(lineColumns.size() + columnNumbers.size() + lineNumbers.size() + boxNumbers.size());
	constexpr size_t secondaryItemCount(0);

	// Create the options now.
	vector<Option> options;

	for (size_t lcIndex = 0; lcIndex < lineColumns.size(); ++lcIndex) {
		const Item& pos(lineColumns[lcIndex]);
		const int box(getBox(pos.first, pos.second, n));

		for (int number = 1; number <= numberCount; ++number) {
			auto itCn(find(columnNumbers.cbegin(), columnNumbers.cend(), Item{ pos.first, number }));
			if (itCn == columnNumbers.cend()) {
				continue;
			}
			auto itLn(find(lineNumbers.cbegin(), lineNumbers.cend(), Item{ pos.second, number }));
			if (itLn == lineNumbers.cend()) {
				continue;
			}
			auto itBn(find(boxNumbers.cbegin(), boxNumbers.cend(), Item{ box, number }));
			if (itBn == boxNumbers.end()) {
				continue;
			}

			Option opt;
			opt.lineColumn = lcIndex;
			opt.columnNumber = distance(columnNumbers.cbegin(), itCn);
			opt.lineNumber = distance(lineNumbers.cbegin(), itLn);
			opt.boxNumber = distance(boxNumbers.cbegin(), itBn);
			options.emplace_back(opt);
		}
	}

	const size_t optionCount(options.size());

	// Write XCC header.
	const size_t itemCount(primaryItemCount);
	cout << itemCount << ' ' << secondaryItemCount << ' ' << optionCount;

	size_t itemIndex(0);

	// Write (column, index) items.
	// We need to keep offsets to write options after.
	const size_t lcItemIndex(itemIndex);
	itemIndex += lineColumns.size();
	for (const Item& lc : lineColumns) {
		cout << endl << 'c' << lc.first << 'l' << lc.second;
	}

	// Write (column, number) items.
	const size_t cnItemIndex(itemIndex);
	itemIndex += columnNumbers.size();
	for (const Item& cn : columnNumbers) {
		cout << endl << 'c' << cn.first << 'n' << cn.second;
	}

	// Write (line, number) items.
	const size_t lnItemIndex(itemIndex);
	itemIndex += lineNumbers.size();
	for (const Item& ln : lineNumbers) {
		cout << endl << 'l' << ln.first << 'n' << ln.second;
	}

	// Write (box, number) items.
	const size_t bnItemIndex(itemIndex);
	itemIndex += boxNumbers.size();
	for (const Item& bn : boxNumbers) {
		cout << endl << 'b' << bn.first << 'n' << bn.second;
	}

	// Write options.
	for (const Option& opt : options) {
		cout << endl << (lcItemIndex + opt.lineColumn)
			<< ' ' << (cnItemIndex + opt.columnNumber)
			<< ' ' << (lnItemIndex + opt.lineNumber)
			<< ' ' << (bnItemIndex + opt.boxNumber);
	}

	return 0;
}
