#include <string>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	// Parse parameters
	int n(0);
	bool removeSymmetries(true);

	if (argc > 1) {
		try {
			n = stoi(argv[1]);
		}
		catch (...) {
			n = 0;
		}

		for (int i = 2; i < argc; ++i) {
			const string arg(argv[i]);
			if (arg == "-s") {
				removeSymmetries = false;
			}
			else {
				n = 0;
				break;
			}
		}
	}

	if (n <= 1) {
		cout << "Use: queens n (integer > 1)" << endl;
		return -1;
	}

	// We try to place n queens on n columns * n lines puzzle.
	// In chess the queen can move anywhere in vertical, horizontal, diagonal and
	// reverse diagonal directions.

	// We have n columns and n lines.
	// We want EXACTLY ONE queen per line and column so they are primary items.
	const int primaryItemCount(2 * n);

	// There are 2n-1 diagonals but the 2 in the extremities have only one cell
	// so we don't need to define them as constraints.
	const int diagonalCount(2 * n - 1 - 2);

	// Same number of reverse diagonals as diagonals.
	const int reverseDiagonalCount(diagonalCount);

	// Diagonals are secondary items as we can have AT MOST one queen per diagonal.
	const int secondaryItemCount(diagonalCount + reverseDiagonalCount);

	// Each option is a possible queen position (column, line) to which we add
	// diagonals constraints.
	int optionCount(n * n);

	if (removeSymmetries) {
		// Remove options for flip symmetry
		optionCount -= n / 2;
		if (n % 2 != 0) {
			--optionCount;
		}
	}

	// Write XCC header.
	const int itemCount(primaryItemCount + secondaryItemCount);
	cout << itemCount << ' ' << secondaryItemCount << ' ' << optionCount;

	int itemIndex(0);

	// Write diagonals names.
	// We need to keep offsets to write options after.
	const int diagonalItemOffset(itemIndex);
	for (int diag = 1; diag <= diagonalCount; ++diag, ++itemIndex) {
		cout << endl << "d" << (diag + 1);
	}

	// Write reverse diagonals names.
	const int reverseDiagonalItemOffset(itemIndex);
	for (int revDiag = 1; revDiag <= reverseDiagonalCount; ++revDiag, ++itemIndex) {
		cout << endl << "r" << (revDiag + 1);
	}

	// Write columns names.
	const int columnItemOffset(itemIndex);
	for (int col = 1; col <= n; ++col, ++itemIndex) {
		cout << endl << "c" << col;
	}

	// Write lines names.
	const int lineItemOffset(itemIndex);
	for (int line = 1; line <= n; ++line, ++itemIndex) {
		cout << endl << "l" << line;
	}

	// Write options.
	for (int col = 0; col < n; ++col) {
		for (int line = 0; line < n; ++line) {
			// Remove the flip symmetry
			if (removeSymmetries && line == 0 && col >= n / 2) {
				continue;
			}

			cout << endl << (columnItemOffset + col) << ' ' << (lineItemOffset + line);

			// Get diagonal from (column, line).
			// As said previously we don't need extremities diagonals as they have only one cell.
			const int diag(col + line);
			if (diag > 0 && diag <= diagonalCount) {
				// Get only write colour to get it working with XCC solver input format.
				cout << ' ' << (diagonalItemOffset + diag - 1) << ':' << col;
			}

			// Get reverse diagonal from (column, line).
			const int revDiag(n - 1 - col + line);
			if (revDiag > 0 && revDiag <= reverseDiagonalCount) {
				cout << ' ' << (reverseDiagonalItemOffset + revDiag - 1) << ':' << col;
			}
		}
	}

	return 0;
}
