#include <set>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct Option {
	string shape;
	vector<int> positions;
};

struct Point {
	int x;
	int y;

	bool operator==(const Point& p) const { return p.x == x && p.y == y; }
	bool operator<(const Point& p) const { return y < p.y || (y == p.y && x < p.x); }
};

class Shape {
public:
	Shape(const string& name, const initializer_list<Point>& points)
		: m_points(points),
		m_width(0),
		m_height(0),
		m_name(name) {
		for (const Point& pt : m_points) {
			if (pt.x >= m_width) {
				m_width = pt.x + 1;
			}
			if (pt.y >= m_height) {
				m_height = pt.y + 1;
			}
		}
	}

public:
	void rotate() {
		set<Point> points;
		for (const Point& pt : m_points) {
			points.insert({ m_height - pt.y - 1, pt.x });
		}
		m_points = move(points);
		swap(m_width, m_height);
	}
	void mirror() {
		set<Point> points;
		for (const Point& pt : m_points) {
			points.insert({ m_width - pt.x - 1, pt.y });
		}
		m_points = move(points);
	}

public:
	void print() const {
		for (int y = 0; y < m_height; ++y) {
			for (int x = 0; x < m_width; ++x) {
				if (find(m_points.cbegin(), m_points.cend(), Point{ x, y }) != m_points.cend()) {
					cout << 'X';
				}
				else {
					cout << ' ';
				}
			}
			cout << endl;
		}
	}
	vector<Shape> getOrientations(int width, int height) const {
		vector<Shape> shapes;
		Shape s(*this);
		if (s.m_width <= width && s.m_height <= height) {
			shapes.push_back(s);
		}
		s.mirror();
		if (s.m_width <= width && s.m_height <= height && find(shapes.cbegin(), shapes.cend(), s) == shapes.cend()) {
			shapes.push_back(s);
		}
		s = *this;
		for (int r = 0; r < 3; ++r) {
			s.rotate();
			if (s.m_width <= width && s.m_height <= height && find(shapes.cbegin(), shapes.cend(), s) == shapes.cend()) {
				shapes.push_back(s);
			}
			Shape m(s);
			m.mirror();
			if (m.m_width <= width && m.m_height <= height && find(shapes.cbegin(), shapes.cend(), m) == shapes.cend()) {
				shapes.push_back(m);
			}
		}
		return shapes;
	}
	const string& getName() const {
		return m_name;
	}
	int getWidth() const {
		return m_width;
	}
	int getHeight() const {
		return m_height;
	}
	const set<Point>& getPoints() const {
		return m_points;
	}
	int getLeftSpace(int height) const {
		if (height == m_height) {
			vector<bool> full(m_height, false);
			size_t fullCount(0);
			int space(0);
			for (int x = 0; x < m_width; ++x) {
				for (int y = 0; y < m_height; ++y) {
					if (!full[y]) {
						if (find(m_points.cbegin(), m_points.cend(), Point{ x, y }) == m_points.cend()) {
							++space;
						}
						else {
							full[y] = true;
							++fullCount;
							if (fullCount >= full.size()) {
								return space;
							}
						}
					}
				}
			}
		}

		return -1;
	}
	int getRightSpace(int height) const {
		if (height == m_height) {
			vector<bool> full(m_height, false);
			size_t fullCount(0);
			int space(0);
			for (int x = m_width - 1; x >= 0; --x) {
				for (int y = 0; y < m_height; ++y) {
					if (!full[y]) {
						if (find(m_points.cbegin(), m_points.cend(), Point{ x, y }) == m_points.cend()) {
							++space;
						}
						else {
							full[y] = true;
							++fullCount;
							if (fullCount >= full.size()) {
								return space;
							}
						}
					}
				}
			}
		}

		return -1;
	}

public:
	void operator=(const Shape& s) {
		if (&s != this) {
			m_points = s.m_points;
			m_width = s.m_width;
			m_height = s.m_height;
			m_name = s.m_name;
		}
	}

public:
	bool operator==(const Shape& s) const {
		return m_name == s.m_name && m_width == s.m_width && m_height == s.m_height && m_points == s.m_points;
	}

private:
	set<Point> m_points;
	int m_width;
	int m_height;
	string m_name;
};

const vector<Shape> SHAPES{
	{ "R", { { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 0 } } },
	{ "O", { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 } } },
	{ "Q", { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 3 } } },
	{ "S", { { 0, 2 }, { 0, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 } } },
	{ "P", { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 0 }, { 1, 1 } } },
	{ "T", { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 0 } } },
	{ "U", { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 0 }, { 2, 1 } } },
	{ "V", { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 0 }, { 2, 0 } } },
	{ "W", { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 }, { 2, 2 } } },
	{ "X", { { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 1 } } },
	{ "Y", { { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } } },
	{ "Z", { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 2 } } }
};

constexpr int SHAPES_SIZE(5);

void writePentomino(ostream& out, int width, int height, bool removeSymmetries, bool optimizations) {
	vector<string> shapes;

	// Create the options now.
	vector<Option> options;

	for (const Shape& s : SHAPES) {
		vector<Shape> orientations;

		if (removeSymmetries && s.getName()[0] == 'V') {
			orientations = { s };
		}
		else {
			orientations = s.getOrientations(width, height);
		}

		for (const Shape& o : orientations) {
			const int leftSpace(o.getLeftSpace(height));
			const int rightSpace(o.getRightSpace(height));

			for (int y = 0; y <= height - o.getHeight(); ++y) {
				for (int x = 0; x <= width - o.getWidth(); ++x) {
					if (optimizations) {
						// Optimization: isolated area must be a multiple of 5
						if (leftSpace >= 0) {
							const int leftArea(x * height + leftSpace);
							if ((leftArea % SHAPES_SIZE) != 0) {
								continue;
							}
						}
						if (rightSpace >= 0) {
							const int rightArea((width - x - o.getWidth()) * height + rightSpace);
							if ((rightArea % SHAPES_SIZE) != 0) {
								continue;
							}
						}
					}

					Option opt;
					opt.shape = o.getName();
					for (const Point& pt : o.getPoints()) {
						opt.positions.push_back((x + pt.x) + (y + pt.y) * width);
					}
					options.emplace_back(move(opt));

					if (find(shapes.cbegin(), shapes.cend(), s.getName()) == shapes.cend()) {
						shapes.push_back(s.getName());
					}
				}
			}
		}
	}

	const size_t primaryItemCount(shapes.size() + width * height);
	constexpr size_t secondaryItemCount(0);
	const size_t optionCount(options.size());

	// Write XCC header.
	const size_t itemCount(primaryItemCount);
	out << itemCount << ' ' << secondaryItemCount << ' ' << optionCount;

	// Write shape items.
	for (const string& s : shapes) {
		out << endl << s;
	}

	// Write positions.
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			out << endl << (x + y * width);
		}
	}

	// Write options.
	for (const Option& opt : options) {
		const size_t shapeIndex(distance(shapes.cbegin(), find(shapes.cbegin(), shapes.cend(), opt.shape)));
		out << endl << shapeIndex;
		for (int pos : opt.positions) {
			out << ' ' << ((int)shapes.size() + pos);
		}
	}
}

int main(int argc, char* argv[])
{
	// Parse parameters
	int width(0);
	int height(0);
	bool removeSymmetries(true);
	bool optimizations(true);

	if (argc > 2) {
		try {
			width = stoi(argv[1]);
			height = stoi(argv[2]);
		}
		catch (...) {
			width = 0;
			height = 0;
		}

		for (int i = 3; i < argc; ++i) {
			const string arg(argv[i]);
			if (arg == "-s") {
				removeSymmetries = false;
			}
			else if (arg == "-o") {
				optimizations = false;
			}
			else {
				width = 0;
				height = 0;
				break;
			}
		}
	}

	if (width <= 0 || height <= 0) {
		cout << "Use: pentomino width height [-o] [-s]" << endl;
		return -1;
	}

	// Check the surface
	const int surface((int)(SHAPES_SIZE * SHAPES.size()));

	if (width * height != surface) {
		cout << "Surface must be equal to " << surface << endl;
		return -2;
	}

	writePentomino(cout, width, height, removeSymmetries, optimizations);

	return 0;
}
