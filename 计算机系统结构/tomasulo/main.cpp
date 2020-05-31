#include <chrono>
#include "tomasulo.hpp"

i32 main(i32 argc, char **argv) {
  if (argc != 4) {
    printf("Usage: %s <nel file> <print state (0/1~32, number of registers to print)> <print log (0/1)>\n", argv[0]);
    return 1;
  }
  u32 n_reg = atoi(argv[2]);
  n_reg = n_reg < 32 ? n_reg : 32;
  bool print_log = atoi(argv[3]) == 1;
  Tomasulo t{Inst::parse_file(argv[1])};
  auto beg = std::chrono::high_resolution_clock::now();
  while (true) {
    bool finished = !t.step();
    if (n_reg != 0) {
      printf("Cycle %d:\n", t.clock);
      t.display(n_reg);
    }
    if (finished) { break; }
  }
  fprintf(stderr, "Executed %d cycle(s) in %.2lfms\n", t.clock,
          std::chrono::duration<double, std::ratio<1, 1000>>(std::chrono::high_resolution_clock::now() - beg).count());
  if (print_log) {
    for (Inst &in : t.insts) {
      printf("%d %d %d\n", in.issue_time, in.comp_time, in.comp_time + 1);
    }
  }
}