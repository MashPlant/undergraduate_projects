#pragma once

#include "inst.hpp"

// both reservation stations for +-*/ and load buffers for LD share these properties
struct RSBase {
  bool busy; // if `busy == false`, all other fields' values are meaningless
  // max = 4, for */, so u8 is more than enough
  // set it to 255 ((u8) -1), when it hasn't started yet (no available fu or operands are not ready)
  // running <=> occupying a function unit <=> busy && remain_time != (u8) -1
  u8 remain_time;
  u32 issue_time; // used to identify logic order in reservation station
  u32 inst_idx; // used to report information (`comp_time`), not really useful for execution

  // set `this` to the just-issued state
  void issue(u32 clock, u32 pc) {
    busy = true;
    remain_time = (u8) -1;
    issue_time = clock;
    inst_idx = pc;
  }
};

struct ReservationStation : RSBase {
  Op op;
  bool is_q[2]; // `is_q[i] == true` <=> `qv[i] means q[i]`, for i in {0, 1}
  u32 qv[2];
  // when `op == Jump`, `qv[0]` is for cond register, `qv[1]` is for imm used for comparison, and `jump_offset` is used
  u32 jump_offset;
};

struct LoadBuffer : RSBase { u32 imm; };

struct RegState {
  bool is_rs;
  // `rs` can be index of reservation stations & load buffers
  // to distinguish these two, when it points to load buffers, the value range should start from `ARS + MRS`
  union { u32 rs, val; };
};

struct Tomasulo {
  constexpr static u32 ARS = 6, MRS = 3, LB = 3;
  constexpr static u32 ADD = 3, MULT = 2, LOAD = 2; // number of function units
  constexpr static u32 REG = 32;
  std::vector<Inst> insts;
  u32 pc, clock;
  ReservationStation rss[ARS + MRS];
  LoadBuffer lbs[LB];
  RegState regs[REG]; // in default initialization, `is_rs` will be assigned false, which what we expect

  // return `true` when  execution is not finished
  bool step() {
    ++clock; // initial value of `clock` is 0, now add it before any operation, so they will see initial clock == 1
    write_back();
    issue();
    exec();
    bool ret = pc < insts.size();
    for (u32 i = 0; i < ARS + MRS && !ret; ++i) { ret |= rss[i].busy; }
    for (u32 i = 0; i < LB && !ret; ++i) { ret |= lbs[i].busy; }
    return ret;
  }

  void write_back() {
    for (u32 i = 0; i < ARS + MRS; ++i) {
      ReservationStation &r = rss[i];
      if (r.busy && r.remain_time == 0) {
        switch (r.op) {
          case Add ... Div:
            cdb_broadcast(i, op_eval(r.op, r.qv[0], r.qv[1]));
            break;
          case Jump:
            if (r.qv[0] == r.qv[1]) { pc += r.jump_offset; }
            break;
            DEFAULT // it can't be Ld
        }
        r.busy = false;
      }
    }
    for (u32 i = 0; i < LB; ++i) {
      LoadBuffer &l = lbs[i];
      if (l.busy && l.remain_time == 0) {
        cdb_broadcast(i + ARS + MRS, l.imm);
        l.busy = false;
      }
    }
  }

  // `rs` have the same meaning as `RegState::rs`
  // iterate over all reservation stations and registers, fill the value when it is needed
  void cdb_broadcast(u32 rs, u32 val) {
    for (ReservationStation &r : rss) {
      if (r.is_q[0] && r.qv[0] == rs) { r.is_q[0] = false, r.qv[0] = val; }
      if (r.is_q[1] && r.qv[1] == rs) { r.is_q[1] = false, r.qv[1] = val; }
    }
    for (RegState &r : regs) {
      if (r.is_rs && r.rs == rs) { r.is_rs = false, r.val = val; }
    }
  }

  void exec() {
    for (u32 i = 0; i < ARS + MRS; ++i) {
      ReservationStation &r = rss[i];
      if (r.busy) {
        if (r.remain_time != (u8) -1) {
          if (--r.remain_time == 0) {
            if (insts[r.inst_idx].comp_time == 0) { insts[r.inst_idx].comp_time = clock; }
          }
        } else if (!r.is_q[0] && !r.is_q[1]) { // not running, but operands available
          u32 beg, end, cap, cnt = 0;
          if (i < ARS) { beg = 0, end = ARS, cap = ADD; } else { beg = ARS, end = ARS + MRS, cap = MULT; }
          // count occupied function units
          for (u32 j = beg; j < end; ++j) {
            ReservationStation &r1 = rss[j];
            cnt += r1.busy && (r1.remain_time != (u8) -1 || (!r1.is_q[0] && !r1.is_q[1] && r1.issue_time < r.issue_time));
          }
          // still exists an available function unit?
          if (cnt < cap) {
            r.remain_time = op_time(r.op, r.qv[0], r.qv[1]);
          }
        }
      }
    }
    for (LoadBuffer &l : lbs) {
      if (l.busy) {
        if (l.remain_time != (u8) -1) {
          if (--l.remain_time == 0) {
            if (insts[l.inst_idx].comp_time == 0) { insts[l.inst_idx].comp_time = clock; }
          }
        } else {
          u32 cnt = 0;
          for (LoadBuffer &l1 : lbs) {
            cnt += l1.busy && (l1.remain_time != (u8) -1 || l1.issue_time < l.issue_time);
          }
          if (cnt < LOAD) {
            l.remain_time = op_time(Ld, 0, 0); // `l` and `r` won't be used
          }
        }
      }
    }
  }

  void issue() {
    for (u32 i = 0; i < ARS; ++i) {
      // when exists an unfinished JUMP, can't issue
      if (rss[i].busy && rss[i].op == Jump) { return; }
    }
    if (pc >= insts.size()) { return; }
    Inst &in = insts[pc];
    switch (in.op) {
      case Add ... Div: {
        u32 beg, end;
        if (in.op == Add || in.op == Sub) { beg = 0, end = ARS; } else { beg = ARS, end = ARS + MRS; }
        for (u32 i = beg; i < end; ++i) {
          ReservationStation &r = rss[i];
          if (!r.busy) {
            r.issue(clock, pc);
            r.op = in.op;
            // though the name is `val`, the valid value can also be `rs`
            // technically it is UB, but no practical compiler will find trouble with me on this
            r.is_q[0] = regs[in.l].is_rs, r.qv[0] = regs[in.l].val;
            r.is_q[1] = regs[in.r].is_rs, r.qv[1] = regs[in.r].val;
            regs[in.dst].is_rs = true;
            regs[in.dst].rs = i;
            issue_inst(in);
            break;
          }
        }
        break;
      }
      case Ld:
        for (u32 i = 0; i < LB; ++i) {
          if (!lbs[i].busy) {
            lbs[i].issue(clock, pc);
            lbs[i].imm = in.imm;
            regs[in.dst].is_rs = true;
            regs[in.dst].rs = i + ARS + MRS; // need to distinguish from reservation station id
            issue_inst(in);
            break;
          }
        }
        break;
      case Jump:
        for (u32 i = 0; i < ARS; ++i) {
          ReservationStation &r = rss[i];
          if (!rss[i].busy) {
            r.issue(clock, pc);
            r.op = Jump;
            r.is_q[0] = regs[in.cond].is_rs, r.qv[0] = regs[in.cond].val;
            r.is_q[1] = false, r.qv[1] = in.cmp;
            // when jumping, pc have already incremented, so need to -1 here to compensate
            r.jump_offset = in.offset - 1;
            issue_inst(in);
            break;
          }
        }
        break;
        DEFAULT
    }
  }

  void issue_inst(Inst &in) {
    ++pc;
    if (in.issue_time == 0) { in.issue_time = clock; }
  }

  // will only print the first `n_reg` registers (of course, no range check)
  void display(u32 n_reg = 8, FILE *f = stdout) {
    constexpr const char *RS_NAMES[] = {"Ars1", "Ars2", "Ars3", "Ars4", "Ars5", "Ars6", "Mrs1", "Mrs2", "Mrs3", "LB1", "LB2", "LB3"};
    constexpr const char *NO_YES[] = {"No", "Yes"};
    fputs("     Busy OP   Vj         Vk         Qj         Qk         Remain\n", f);
    for (u32 i = 0; i < ARS + MRS; ++i) {
      ReservationStation &r = rss[i];
      fprintf(f, "%s %-5s%-5s", RS_NAMES[i], NO_YES[r.busy], op_name(r.op));
      if (r.busy && !r.is_q[0]) { fprintf(f, "0x%-9X", r.qv[0]); } else { fputs("-          ", f); }
      if (r.busy && !r.is_q[1]) { fprintf(f, "0x%-9X", r.qv[1]); } else { fputs("-          ", f); }
      if (r.busy && r.is_q[0]) { fprintf(f, "%-11s", RS_NAMES[r.qv[0]]); } else { fputs("-          ", f); }
      if (r.busy && r.is_q[1]) { fprintf(f, "%-11s", RS_NAMES[r.qv[1]]); } else { fputs("-          ", f); }
      if (r.busy && r.remain_time != (u8) -1) { fprintf(f, "%d\n", r.remain_time); } else { fputs("-\n", f); }
    }
    fputs("    Busy Imm        Remain\n", f);
    for (u32 i = 0; i < LB; ++i) {
      LoadBuffer &l = lbs[i];
      fprintf(f, "%s %-5s", RS_NAMES[i + ARS + MRS], NO_YES[l.busy]);
      if (l.busy) { fprintf(f, "0x%-9X", l.imm); } else { fputs("-          ", f); }
      if (l.busy && l.remain_time != (u8) -1) { fprintf(f, "%d\n", l.remain_time); } else { fputs("-\n", f); }
    }
    for (u32 i = 0; i < n_reg; ++i) {
      fprintf(f, "R%02d        ", i);
    }
    fputc('\n', f);
    for (u32 i = 0; i < n_reg; ++i) {
      RegState &r = regs[i];
      if (r.is_rs) { fprintf(f, "%-11s", RS_NAMES[r.rs]); } else { fprintf(f, "0x%-9X", r.val); }
    }
    fputc('\n', f);
  }
};