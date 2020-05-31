#include <iostream>
#include <chrono>

#include "common.h"
#include "DimacsParser.h"
#include "DPLL.h"

// helper function to show a formula
std::string show_formula(const formula &phi) {
  std::stringstream ss;
  ss << "(and" << std::endl;
  for (const auto &clause : phi.clauses) {
    ss << "  (or";
    for (literal l : clause) {
      if (POSITIVE(l)) ss << " " << l;
      else ss << " (not " << VAR(l) << ")";
    }
    ss << ")" << std::endl;
  }
  ss << ")";

  return ss.str();
}

// entry
int main(int argc, const char **argv) {
  if (argc < 2) {
    argc = 2;
//    argv[1] = "../queen.dimacs";
    argv[1] = "../tests/test9.dimacs";
//    std::cerr << "error: no input files" << std::endl;
//    return 1;
  }

  for (int i = 1; i < argc; i++) {
    std::string f(argv[i]);
    std::cout << f << std::endl;
    formula phi = DimacsParser::parse(f);
    // std::cout << show_formula(phi) << std::endl;

    // timer start
    auto start = std::chrono::steady_clock::now();
    DPLL solver(phi);
    bool sat = solver.check_sat();
    model m;
    if (sat) {
      m = solver.get_model();
    }
    auto end = std::chrono::steady_clock::now();
    // timer end

    if (sat) {
      std::cout << "  sat" << std::endl;
      // for (const auto &p : m) {
      //   std::cout << "    " << p.first << " -> " << p.second << std::endl;
      // }
      for (clause &cls : phi.clauses) {
        bool ok = false;
        for (int lit : cls) {
          auto it = m.find(VAR(lit));
          assert(it != m.end());
          if (it->second == (lit > 0)) {
            ok = true;
          }
        }
        assert(ok);
      }
    } else {
      std::cout << "  unsat" << std::endl;
    }

    auto duration = std::chrono::duration<float, std::milli>(end - start);
    std::cout << "  time: " << duration.count() << " ms" << std::endl;
  }

  return 0;
}
