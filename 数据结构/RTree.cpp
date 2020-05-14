#include <cmath>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;

#define PI 3.14159265
#define DEG2RAD(degree) (degree) * (PI / 180.0)
#define RAD2DEG(degree) (degree) * (180.0 / PI)
#define EARTH_RADIUS (6378.137 * 1000)
#define LBS_LON_MIN 116
#define LBS_LON_MAX 117
#define LBS_LAT_MIN 39
#define LBS_LAT_MAX 41

const double min_cos_lat2 = cos(DEG2RAD(LBS_LAT_MAX)) * cos(DEG2RAD(LBS_LAT_MAX));
const double max_cos_lat2 = cos(DEG2RAD(LBS_LAT_MIN)) * cos(DEG2RAD(LBS_LAT_MIN));

inline double lbs_distance(double lon1, double lat1, double lon2, double lat2) {
  double a = lat1 - lat2;
  double b = lon1 - lon2;

  double s = asin(sqrt(pow(sin(a / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(b / 2), 2)));
  return s;
}

inline double sin_lower_bound(double x) {
  return x - x * x * x * (1.0 / 6.0);
}

inline double lbs_distance_lower_bound(double lon1, double lat1, double lon2, double lat2) {
  double a = lat1 - lat2;
  double b = lon1 - lon2;

  double s = sqrt(pow(sin_lower_bound(a / 2), 2) +
                  min_cos_lat2 * pow(sin_lower_bound(b / 2), 2));
  return s;
}

inline double lbs_distance_upper_bound(double lon1, double lat1, double lon2, double lat2) {
  double a = lat1 - lat2;
  double b = lon1 - lon2;

  double s = asin(sqrt(pow(a / 2, 2) + max_cos_lat2 * pow(b / 2, 2)));
  return s;
}

inline double min_dist(double p_lon, double p_lat,
                       double r_lon1, double r_lat1,
                       double r_lon2, double r_lat2) {
  return lbs_distance_lower_bound(p_lon, p_lat,
                                  p_lon > r_lon2 ? r_lon2 : p_lon < r_lon1 ? r_lon1 : p_lon,
                                  p_lat > r_lat2 ? r_lat2 : p_lat < r_lat1 ? r_lat1 : p_lat);
}

inline double min_max_dist(double p_lon, double p_lat,
                           double r_lon1, double r_lat1,
                           double r_lon2, double r_lat2) {
  return std::min(lbs_distance_upper_bound(p_lon, p_lat, p_lon - r_lon1 < r_lon2 - p_lon ? r_lon1 : r_lon2,
                                           p_lat - r_lat1 < r_lat2 - p_lat ? r_lat2 : r_lat1),
                  lbs_distance_upper_bound(p_lon, p_lat, p_lon - r_lon1 < r_lon2 - p_lon ? r_lon2 : r_lon1,
                                           p_lat - r_lat1 < r_lat2 - p_lat ? r_lat1 : r_lat2));
}

struct Point {
  double x, y;

  bool operator==(const Point &rhs) const {
    return x == rhs.x && y == rhs.y;
  }

  double dist(const Point &rhs) const {
    return lbs_distance(x, y, rhs.x, rhs.y);
  }

  double min_dist(const Point &rhs) const {
    return lbs_distance_lower_bound(x, y, rhs.x, rhs.y);
  }
};

struct Data {
  Point node;
  int id;

  const Point &p() const {
    return node;
  }
};

struct Rect {
  Point min, max;

  Rect() = default;

  Rect(double min_x, double min_y, double max_x, double max_y) : min{min_x, min_y}, max{max_x, max_y} {}

  Rect(const Point &min, const Point &max) : min(min), max(max) {}

  Rect(const Point &p) : min(p), max(p) {}

  bool overlap(const Rect &r) const {
    return min.x <= r.max.x
           && max.x >= r.min.x
           && max.y >= r.min.y
           && min.y <= r.max.y;
  }

  bool contain(const Point &p) const {
    return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y;
  }

  Rect extend(const Point &p) const {
    return {std::min(min.x, p.x), std::min(min.y, p.y), std::max(max.x, p.x), std::max(max.y, p.y)};
  }

  Rect extend(const Rect &r) const {
    return {std::min(min.x, r.min.x), std::min(min.y, r.min.y), std::max(max.x, r.max.x), std::max(max.y, r.max.y)};
  }

  double area() const {
    return (max.y - min.y) * (max.x - min.x);
  }

  double min_dist(const Point &p) const {
    return ::min_dist(p.x, p.y, min.x, min.y, max.x, max.y);
//    double dx = std::max(fabs(p.x - (min.x + max.x) * 0.5) - (max.x - min.x) * 0.5, 0.0);
//    double dy = std::max(fabs(p.y - (min.y + max.y) * 0.5) - (max.y - min.y) * 0.5, 0.0);
//    return dx * dx + dy * dy;
  }

  double min_max_dist(const Point &p) const {
    return ::min_max_dist(p.x, p.y, min.x, min.y, max.x, max.y);
//    double near_dx = p.x - min.x < max.x - p.x ? p.x - min.x : max.x - p.x;
//    double far_dx = p.x - min.x < max.x - p.x ? max.x - p.x : p.x - min.x;
//    double near_dy = p.y - min.y < max.y - p.y ? p.y - min.y : max.y - p.y;
//    double far_dy = p.y - min.y < max.y - p.y ? max.y - p.y : p.y - min.y;
//    return std::min(near_dx * near_dx + far_dy * far_dy, far_dx * far_dx + near_dy * near_dy);
  }
};

const int M = 64;
const int M_2 = M / 2;
const int m = 4;

class RTree {
  struct Node {
    int num;
    const int h; // leaf <-> h == 0
    Node *p;
    Rect r;

    bool leaf() const {
      return h == 0;
    }
  };

  struct Internal : Node {
    Node *ch[M];

    Internal(Node *p = nullptr, int num = 0, int h = 0) : Node{num, h, p, Rect{}} {}
  };

  struct Leaf : Node {
    Data data[M];

    Leaf(Node *p = nullptr, int num = 0, int h = 0) : Node{num, h, p, Rect{}} {}
  };

  // short vec, 16 is quite enough(can hold at least m^16 = 2^32 entries)
  struct Vec {
    Node *data[16];
    int size = 0;

    void push(Node *x) { data[size++] = x; }

    Node *operator[](int index) { return data[index]; }
  };

  Node *root;

  static Rect rect_from_data(Data *first, Data *last) {
    Rect r{first->node.x, first->node.y, first->node.x, first->node.y};
    while (++first < last) {
      r = r.extend(first->node);
    }
    return r;
  }

  static Rect rect_from_ch(Node **first, Node **last) {
    Rect r = (*first)->r;
    while (++first < last) {
      r = r.extend((*first)->r);
    }
    return r;
  }

  // p->ch[index]->num == M, p->num < M
  // split p->ch[index] into p->ch[index] & p->ch[p->num], then inc p->num
  static void split(Internal *p, int index) {
    Node *x = p->ch[index];
    x->num = 0;
    if (x->leaf()) {
      Leaf *y = static_cast<Leaf *>(x);
      Leaf *z = new Leaf(p, 0, y->h);
      int r1_init = -1, r2_init = -1;
      double min_waste = 1e20;
      for (int i = 0; i < M; ++i) {
        for (int j = i + 1; j < M; ++j) {
          double waste = Rect(y->data[i].p()).extend(y->data[j].p()).area();
          if (waste < min_waste) {
            r1_init = i, r2_init = j;
            min_waste = waste;
          }
        }
      }
      std::swap(y->data[y->num++], y->data[r1_init]);
      z->data[z->num++] = y->data[r2_init];
      y->data[r2_init] = y->data[1];
      y->r = y->data[0].p();
      z->r = z->data[0].p();
      for (int i = 2; i < M; ++i) {
        Rect ry_ex = y->r.extend(y->data[i].p()), rz_ex = z->r.extend(y->data[i].p());
        if (z->num + M - i >= m && (ry_ex.area() - y->r.area() < rz_ex.area() - z->r.area()
                                    || y->num + M - i == m)) {
          y->r = ry_ex;
          y->data[y->num++] = y->data[i];
        } else {
          z->r = rz_ex;
          z->data[z->num++] = y->data[i];
        }
      }
      p->ch[p->num++] = z;
    } else {
      Internal *y = static_cast<Internal *>(x);
      Internal *z = new Internal(p, 0, y->h);
      int r1_init = -1, r2_init = -1;
      double min_waste = 1e20;
      for (int i = 0; i < M; ++i) {
        for (int j = i + 1; j < M; ++j) {
          double waste = y->ch[i]->r.extend(y->ch[j]->r).area();
          if (waste < min_waste) {
            r1_init = i, r2_init = j;
            min_waste = waste;
          }
        }
      }
      std::swap(y->ch[y->num++], y->ch[r1_init]);
      (z->ch[z->num++] = y->ch[r2_init])->p = z;
      y->ch[r2_init] = y->ch[1];
      y->r = y->ch[0]->r;
      z->r = z->ch[0]->r;
      for (int i = 2; i < M; ++i) {
        Rect ry_ex = y->r.extend(y->ch[i]->r), rz_ex = z->r.extend(y->ch[i]->r);
        if (z->num + M - i >= m && (ry_ex.area() - y->r.area() < rz_ex.area() - z->r.area()
                                    || y->num + M - i == m)) {
          y->r = ry_ex;
          y->ch[y->num++] = y->ch[i];
        } else {
          z->r = rz_ex;
          (z->ch[z->num++] = y->ch[i])->p = z;
        }
      }
      p->ch[p->num++] = z;
    }
  }

  template<typename F>
  static void visit(Node *x, const Rect &bound, F f) {
    if (!x->r.overlap(bound)) {
      return;
    }
    if (x->leaf()) {
      Leaf *y = static_cast<Leaf *>(x);
      for (int i = 0; i < y->num; ++i) {
        if (bound.contain(y->data[i].p())) {
          f(y->data[i]);
        }
      }
    } else {
      Internal *y = static_cast<Internal *>(x);
      for (int i = 0; i < y->num; ++i) {
        visit(y->ch[i], bound, f);
      }
    }
  }

  static Node *erase(Node *x, const Point &p) {
    if (x->r.contain(p)) {
      if (x->leaf()) {
        Leaf *y = static_cast<Leaf *>(x);
        for (int i = 0; i < y->num; ++i) {
          if (y->data[i].p() == p) {
            memmove(y->data + i, y->data + i + 1, (--y->num - i) * sizeof(Data));
            y->r = rect_from_data(y->data, y->data + y->num);
            return y;
          }
        }
      } else {
        Internal *y = static_cast<Internal *>(x);
        for (int i = 0; i < y->num; ++i) {
          if (Node *ret = erase(y->ch[i], p)) {
            y->r = rect_from_ch(y->ch, y->ch + y->num);
            return ret;
          }
        }
      }
    }
    return nullptr;
  }

  // root will not be pushed into rm_stack
  static void condense_tree(Node *rm_node, Vec &rm_stack) {
    while (rm_node && rm_node->p && rm_node->num < m) {
      rm_stack.push(rm_node);
      Internal *p = static_cast<Internal *>(rm_node->p);
      int index = -1;
      for (int i = 0; i < p->num; ++i) {
        if (p->ch[i] == rm_node) {
          index = i;
          break;
        }
      }
      memmove(p->ch + index, p->ch + index + 1, (--p->num - index) * sizeof(Node *));
      rm_node = p;
    }
  }

  void insert_node(Node *n) {
    if (root->num == M) {
      Internal *x = new Internal(nullptr, 1, root->h + 1);
      x->r = root->r;
      (x->ch[0] = root)->p = x;
      split(x, 0);
      root = x;
    }
    Node *x = root;
    while (x->h > n->h + 1) {
      Internal *y = static_cast<Internal *>(x);
      int index = -1;
      double min_expand = 1e20;
      for (int i = 0; i < y->num; ++i) {
        double expand = y->ch[i]->r.extend(n->r).area() - y->ch[i]->r.area();
        if (expand < min_expand) {
          min_expand = expand;
          index = i;
        }
      }
      if (y->ch[index]->num == M) {
        split(y, index);
      }
      y->r = y->r.extend(n->r);
      x = y->ch[index];
    }
    Internal *y = static_cast<Internal *>(x);
    (y->ch[y->num++] = n)->p = y;
    y->r = y->r.extend(n->r);
  }

  static void nearest(const Node *x, const Point &p, const Data *&ans, double &ans_dist) {
    if (x->leaf()) {
      const Leaf *y = static_cast<const Leaf *>(x);
      for (int i = 0; i < y->num; ++i) {
        if (y->data[i].p().min_dist(p) <= ans_dist) {
          ans_dist = y->data[i].p().dist(p);
          ans = y->data + i;
        }
      }
    } else {
      double min_min_max_dist = 1e20;
      const Internal *y = static_cast<const Internal *>(x);
      for (int i = 0; i < y->num; ++i) {
        min_min_max_dist = std::min(min_min_max_dist, y->ch[i]->r.min_max_dist(p));
      }
      ans_dist = std::min(ans_dist, min_min_max_dist);
      for (int i = 0; i < y->num; ++i) {
        if (y->ch[i]->r.min_dist(p) < ans_dist) {
          nearest(y->ch[i], p, ans, ans_dist);
        }
      }
    }
  }

  static void clear(Node *x) {
    if (!x->leaf()) {
      Internal *y = static_cast<Internal *>(x);
      for (int i = 0; i < y->num; ++i) {
        clear(y->ch[i]);
      }
    }
    // assume Data doesn't have non-trivial dtor
    // so Node doesn't virtual dtor either, delete == free
    delete x;
  }

public:
  void insert(const Data &data) {
    if (root->num == M) {
      Internal *x = new Internal(nullptr, 1, root->h + 1);
      x->r = root->r;
      (x->ch[0] = root)->p = x;
      split(x, 0);
      root = x;
    }
    Node *x = root;
    while (!x->leaf()) {
      Internal *y = static_cast<Internal *>(x);
      int index = -1;
      double min_expand = 1e20;
      for (int i = 0; i < y->num; ++i) {
        double expand = y->ch[i]->r.extend(data.p()).area() - y->ch[i]->r.area();
        if (expand < min_expand) {
          min_expand = expand;
          index = i;
        }
      }
      if (y->ch[index]->num == M) {
        split(y, index);
      }
      y->r = y->r.extend(data.p());
      x = y->ch[index];
    }
    Leaf *y = static_cast<Leaf *>(x);
    y->data[y->num++] = data;
    y->r = y->r.extend(data.p());
  }

  // if entries are duplicate, will only remove one
  void erase(const Point &p) {
    Vec rm_stack;
    Node *rm_node = erase(root, p);
    condense_tree(rm_node, rm_stack);
    for (int i = 0; i < rm_stack.size; ++i) {
      Node *x = rm_stack[i];
      if (x->leaf()) {
        Leaf *y = static_cast<Leaf *>(x);
        for (int j = 0; j < y->num; ++j) {
          insert(y->data[j]);
        }
      } else {
        Internal *y = static_cast<Internal *>(x);
        for (int j = 0; j < y->num; ++j) {
          insert_node(y->ch[j]);
        }
      }
      delete x;
    }
    if (root->num == 1 && !root->leaf()) {
      Node *old_root = root;
      root = static_cast<Internal *>(root)->ch[0];
      root->p = nullptr;
      delete old_root;
    }
  }

  template<typename F>
  void visit(const Rect &bound, F f) const {
    visit(root, bound, f);
  }

  const Data &nearest(const Point &p) const {
    const Data *ans = nullptr;
    double ans_dist = 1e20;
    nearest(root, p, ans, ans_dist);
    return *ans;
  }

  RTree() : root(new Leaf) {}

  RTree(const RTree &) = delete;

  RTree &operator=(const RTree &) = delete;

  ~RTree() {
    clear(root);
  }
};

const int N = 100000;
Point data[N];

int main() {
  auto now = high_resolution_clock::now;
  auto beg = now();
  RTree tr;
  std::mt19937 gen;
  std::uniform_real_distribution<> d;
  for (int i = 0; i < N; ++i) {
    data[i].x = DEG2RAD(LBS_LON_MIN + d(gen) * (LBS_LON_MAX - LBS_LON_MIN));
    data[i].y = DEG2RAD(LBS_LAT_MIN + d(gen) * (LBS_LAT_MAX - LBS_LAT_MIN));
    tr.insert({data[i], i});
  }
  for (int i = 0; i < N; ++i) {
    tr.erase(data[i]);
    data[i].x = DEG2RAD(LBS_LON_MIN + d(gen) * (LBS_LON_MAX - LBS_LON_MIN));
    data[i].y = DEG2RAD(LBS_LAT_MIN + d(gen) * (LBS_LAT_MAX - LBS_LAT_MIN));
    tr.insert({data[i], i});
  }
  cout << duration<double>(now() - beg).count() << endl;
  beg = now();
  double _ = 0.0;
  for (int i = 0; i < 10000; ++i) {
    _ += tr.nearest({DEG2RAD(LBS_LON_MIN + d(gen) * (LBS_LON_MAX - LBS_LON_MIN)),
                     DEG2RAD(LBS_LAT_MIN + d(gen) * (LBS_LAT_MAX - LBS_LAT_MIN))}).p().x;
  }
  cout << duration<double>(now() - beg).count() << endl;
  cout << RAD2DEG(_);
}
