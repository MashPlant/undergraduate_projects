//
// DPLL algorithm.
//

#ifndef DPLL_DPLL_H
#define DPLL_DPLL_H

#include "common.h"
#include "bitset.hpp"
#include <algorithm>
#include <memory>

using u32 = unsigned;

struct Var {
  u32 id : 31;
  u32 flag : 1;
};

struct VarInfo {
  enum {
    Undef = 0, Pos = 1, Neg = -1
  } state;
  std::vector<u32> pos_clauses;
  std::vector<u32> neg_clauses;
};

class DPLL {
public:
  DPLL(const formula &phi) : bad(false), clauses(phi.clauses.size()), vars(phi.num_variable),
                             each_len(bs::word_len(phi.num_variable)),
                             decisions(std::make_unique<u32[]>(phi.num_variable * each_len)) {
    for (u32 i = 0; i < phi.clauses.size(); ++i) {
      const clause &cls = phi.clauses[i];
      std::vector<Var> cls_vars(cls.size());
      for (u32 j = 0u; j < cls.size(); ++j) {
        cls_vars[j] = Var{.id = (u32) std::abs(cls[j]) - 1, .flag = cls[j] > 0};
      }
      clauses[i] = std::move(cls_vars);
    }
    while (!bad) {
      auto it = std::find_if(clauses.begin(), clauses.end(),
                             [](std::vector<Var> &cls) { return cls.size() <= 1; });
      if (it == clauses.end()) { break; }
      if (it->size() == 1) {
        Var lit = (*it)[0];
        clauses.erase(it);
        vars[lit.id].state = lit.flag ? VarInfo::Pos : VarInfo::Neg;
        auto new_end = std::remove_if(clauses.begin(), clauses.end(), [lit](std::vector<Var> &cls) {
          bool has_pos = false;
          auto new_end = std::remove_if(cls.begin(), cls.end(), [lit, &has_pos](Var x) {
            if (x.id == lit.id && x.flag == lit.flag) { has_pos = true; }
            return x.id == lit.id;
          });
          cls.erase(new_end, cls.end());
          return has_pos;
        });
        clauses.erase(new_end, clauses.end());
      } else {
        bad = true;
      }
    }
    for (u32 i = 0; i < clauses.size(); ++i) {
      for (Var lit : clauses[i]) {
        (lit.flag ? vars[lit.id].pos_clauses : vars[lit.id].neg_clauses).push_back(i);
      }
    }
  }

  bool check_sat() {
    if (bad) { return false; }
    std::vector<Var> seq;
    std::vector<std::pair<Var, u32>> stk;
    while (true) {
      auto it = std::find_if(vars.begin(), vars.end(), [](VarInfo &v) { return v.state == VarInfo::Undef; });
      if (it == vars.end()) { break; }
      stk.push_back({Var{.id = (u32) (it - vars.begin()), .flag = 0}, -1u});
      while (!stk.empty()) {
        auto[assign, cls] = stk.back();
        auto[target, pos] = assign;
        stk.pop_back();
        if (vars[target].state != VarInfo::Undef) {
          if ((vars[target].state == VarInfo::Pos) != pos) {
            u32 *d = var_decisions(target);
            for (Var lit : clauses[cls]) {
              bs::disjunction(d, var_decisions(lit.id), each_len);
            }
            u32 last = -1u, l = -1u;
            for (u32 i = seq.size() - 1; ~i; --i) {
              if (bs::get(d, seq[i].id)) {
                if (last == -1u) { last = i, l = seq[i].id; } else { break; }
              } else if (seq[i].flag) {
                last = i;
              }
            }
            if (last == -1u) {
              return false;
            }
            auto l_state = vars[l].state;
            for (u32 i = last; i < seq.size(); ++i) {
              vars[seq[i].id].state = VarInfo::Undef;
            }
            seq.erase(seq.begin() + last, seq.end());
            stk.clear();
            u32 new_cls_id = clauses.size();
            stk.push_back({Var{.id = l, .flag = l_state == VarInfo::Neg}, new_cls_id});
            std::vector<Var> new_cls;
            for (u32 i = 0; i < vars.size(); ++i) {
              if (bs::get(d, i)) {
                auto s = i == l ? l_state : vars[i].state;
                new_cls.push_back(Var{.id = i, .flag = s == VarInfo::Neg});
              }
            }
            clauses.push_back(std::move(new_cls));
            continue;
          }
        } else {
          vars[target].state = pos ? VarInfo::Pos : VarInfo::Neg;
          seq.push_back(Var{.id = target, .flag = cls == -1u});
          u32 *d = var_decisions(target);
          bs::clear(d, each_len);
          if (cls == -1u) {
            bs::set(d, target);
          } else {
            for (Var lit : clauses[cls]) {
              bs::disjunction(d, var_decisions(lit.id), each_len);
            }
          }
          for (u32 cls : pos ? vars[target].neg_clauses : vars[target].pos_clauses) {
            Var unit;
            u32 count = 0;
            for (Var lit : clauses[cls]) {
              if (vars[lit.id].state == VarInfo::Undef) {
                if (++count != 1) { break; }
                unit = lit;
              } else if ((vars[lit.id].state == VarInfo::Pos) == lit.flag) {
                count = 2;
                break;
              }
            }
            if (count == 1) {
              stk.push_back({unit, cls});
            }
          }
        }
      }
    }
    for (u32 i = 0; i < vars.size(); ++i) {
      result.insert({i + 1, vars[i].state == VarInfo::Pos});
    }
    return true;
  }

  model get_model() { return result; }

private:
  u32 *var_decisions(u32 id) {
    return decisions.get() + id * each_len;
  }

  bool bad;
  std::vector<std::vector<Var>> clauses;
  std::vector<VarInfo> vars;
  u32 each_len;
  std::unique_ptr<u32[]> decisions;
  model result;
};


#endif //DPLL_DPLL_H
