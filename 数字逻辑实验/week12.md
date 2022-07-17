---
title: "串行密码锁实验报告"
author: "李晨昊 2017011466"
date: "2019-5-20"
output:
  pdf_document:
    latex_engine: xelatex
    number_sections: yes
    toc: yes
  word_document:
    toc: yes
  html_document:
    toc: yes
header-includes: \usepackage{ctex} \usepackage{graphicx}
---

\newpage

# 实验目的
1. 学习使用状态机控制电路工作，在不同状态下完成相应的功能
2. 进一步掌握时序逻辑电路的基本分析和设计方法
3. 学会利用仿真软件实现对数字电路的逻辑功能进行验证和分析

# 实验内容
1. 基本要求：设计一个4位16进制串行密码锁，其具体功能包括：
   1. 设置密码：用户可串行设置4位16进制密码
   2. 验证密码：用户串行输入密码，如果密码符合则点亮开锁灯，若不符合则点亮错误灯
2. 提高要求:
   1. 密码预置：为管理员创建万用密码以备管理。
   2. 系统报警：开锁3次失败后点亮报警灯，并锁定密码锁，只有输入管理员密码才可开锁，并解除报警

# 代码及注释

```vhdl
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity password is
  port (
    clk, rst: in std_logic;
    code: in std_logic_vector(3 downto 0);
    mode: in std_logic_vector(1 downto 0);
    out_unlock: out std_logic;
    out_err: out std_logic;
    out_warn: out std_logic;
    -- 除了要求输出的几个信号灯之外，这里还额外输出了当前失败次数
    -- 这样既方便调试，从用户使用的角度来讲也是更加人性化的
    out_fail_cnt: out integer range 0 to 3
  );
end password;

architecture arch of password is
  -- 定义了5+6+6=17个状态
  -- SETX表示设置密码过程中的状态
  -- CHECKX表示输入密码过程中的状态
  -- MONITORX表示输入管理员密码过程中的状态
  -- 跨越这三类状态间的转换仅限于rst按下时：
  -- 1. 失败3次时，由CHECKX转为MONITORX
  -- 2. 未失败3次且mode="00"由CHECKX转为SET0
  -- 3. 未失败3次视mode由MONITORX转为CHECK0或SET0
  -- 除此之外的状态转换都限制在本类内部
  type state_t is (
    SET0, SET1, SET2, SET3, SET_OK,
    CHECK0, CHECK1, CHECK2, CHECK3, CHECK_OK, CHECK_FAIL,
    MONITOR0, MONITOR1, MONITOR2, MONITOR3, MONITOR_OK, MONITOR_FAIL
  );
  signal state: state_t := CHECK0;
  type all_code_t is array(0 to 3) of std_logic_vector(3 downto 0);
  -- 管理员密码设定为FFFF
  constant monitor_code: all_code_t := ("1111", "1111", "1111", "1111");
  -- 初始，密码设定为0000
  signal set_code: all_code_t := (others => "0000");
  signal fail_cnt: integer range 0 to 3 := 0;
begin
  out_fail_cnt <= fail_cnt;

  handle_clk : process(clk, rst, fail_cnt, mode)
  begin
    if rst = '1' then
      out_err <= '0';
      out_unlock <= '0';
      if fail_cnt = 3 then
        state <= MONITOR0;
      else
        if mode = "00" then
          state <= SET0;
        elsif mode = "01" then
          state <= CHECK0;
        end if;
      end if;
    elsif rising_edge(clk) then
      case(state) is
        -- 分别设定每位密码
        when SET0 => set_code(0) <= code; state <= SET1;
        when SET1 => set_code(1) <= code; state <= SET2;
        when SET2 => set_code(2) <= code; state <= SET3;
        when SET3 => set_code(3) <= code; state <= SET_OK;
        when SET_OK => NULL;
        -- 分别检查每位密码，一旦失败都会执行相同的动作
        -- 有些代码冗余，我试图将它们提取到一个process里
        -- 但是发生了一些我无法解释的错误，因此就维持这个样子了
        when CHECK0 =>
          if set_code(0) = code then
            state <= CHECK1;
          else
            state <= CHECK_FAIL;
            out_err <= '1';
            if fail_cnt /= 3 then
              if fail_cnt = 2 then
                out_warn <= '1';
              end if;
              fail_cnt <= fail_cnt + 1;
            end if;
          end if;
        when CHECK1 =>
          if set_code(1) = code then
            state <= CHECK2;
          else
            state <= CHECK_FAIL;
            out_err <= '1';
            if fail_cnt /= 3 then
              if fail_cnt = 2 then
                out_warn <= '1';
              end if;
              fail_cnt <= fail_cnt + 1;
            end if;
          end if;
        when CHECK2 =>
          if set_code(2) = code then
            state <= CHECK3;
          else
            state <= CHECK_FAIL;
            out_err <= '1';
            if fail_cnt /= 3 then
              if fail_cnt = 2 then
                out_warn <= '1';
              end if;
              fail_cnt <= fail_cnt + 1;
            end if;
          end if;
        when CHECK3 =>
          if set_code(3) = code then
            state <= CHECK_OK;
            fail_cnt <= 0;
            out_unlock <= '1';
          else
            state <= CHECK_FAIL;
            out_err <= '1';
            if fail_cnt /= 3 then
              if fail_cnt = 2 then
                out_warn <= '1';
              end if;
              fail_cnt <= fail_cnt + 1;
            end if;
          end if;
        when CHECK_OK | CHECK_FAIL => NULL;
        -- 分别检查每位管理员密码
        when MONITOR0 =>
          if monitor_code(0) = code then
            state <= MONITOR1;
          else
            state <= MONITOR_FAIL;
          end if;
        when MONITOR1 =>
          if monitor_code(1) = code then
            state <= MONITOR2;
          else
            state <= MONITOR_FAIL;
          end if;
        when MONITOR2 =>
          if monitor_code(2) = code then
            state <= MONITOR3;
          else
            state <= MONITOR_FAIL;
          end if;
        when MONITOR3 =>
          if monitor_code(3) = code then
            state <= MONITOR_OK;
            -- 管理员密码输入正确时，清零fail_cnt
            -- 这样下次按下rst时，就会重新回到CHECK0或者SET0状态
            fail_cnt <= 0;
            out_warn <= '0';
          else
            state <= MONITOR_FAIL;
          end if;
        when MONITOR_OK | MONITOR_FAIL => NULL;
        end case;
    end if;
  end process; -- handle_clk
end arch; -- arch
```

# 仿真结果

使用以下的vhdl代码描述了测试的逻辑：

```vhdl
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; 

entity tb is
end tb;

architecture arch of tb is
  component password is
    port (
      clk, rst: in std_logic;
      code: in std_logic_vector(3 downto 0);
      mode: in std_logic_vector(1 downto 0);
      out_unlock: out std_logic;
      out_err: out std_logic;
      out_warn: out std_logic;
      out_fail_cnt: out integer range 0 to 3
    );
  end component;

  signal clk, rst: std_logic;
  signal code: std_logic_vector(3 downto 0);
  signal mode: std_logic_vector(1 downto 0);
  signal unlock: std_logic;
  signal err: std_logic;
  signal warn: std_logic;
  signal fail_cnt: integer range 0 to 3;
  
  procedure make_edge(signal s: out std_logic) is
  begin
    s <= '0';
    wait for 1 ns;
    s <= '1';
    wait for 1 ns;
    s <= '0';
    wait for 1 ns;
  end make_edge;
begin
  map_password: password port map(clk, rst, code, mode, 
    unlock, err, warn, fail_cnt);

  process
  begin
    -- 设定密码为1234
    mode <= "00"; make_edge(rst);
    code <= "0001"; make_edge(clk);
    code <= "0010"; make_edge(clk);
    code <= "0011"; make_edge(clk);
    code <= "0100"; make_edge(clk);
    
    -- 连续3次输入错误密码1235
    for i in 0 to 2 loop
      mode <= "01"; make_edge(rst);
      code <= "0001"; make_edge(clk);
      code <= "0010"; make_edge(clk);
      code <= "0011"; make_edge(clk);
      code <= "0101"; make_edge(clk);
    end loop ;
      
    -- 输入1234，然而已经没用了
    mode <= "01"; make_edge(rst);
    code <= "0001"; make_edge(clk);
    code <= "0010"; make_edge(clk);
    code <= "0011"; make_edge(clk);
    code <= "0100"; make_edge(clk);

    -- 输入管理员密码FFFF
    mode <= "01"; make_edge(rst);
    code <= "1111"; make_edge(clk);
    code <= "1111"; make_edge(clk);
    code <= "1111"; make_edge(clk);
    code <= "1111"; make_edge(clk);

    -- 输入1234，成功解锁
    mode <= "01"; make_edge(rst);
    code <= "0001"; make_edge(clk);
    code <= "0010"; make_edge(clk);
    code <= "0011"; make_edge(clk);
    code <= "0100"; make_edge(clk);

    report "-------END OF TEST-------" severity failure;
  end process;
end arch ; -- arch
```

测试结果如下：

\begin{center}
\includegraphics[width=0.9\linewidth]{week12/simu.png}
\end{center}

# 实验总结
本次实验进行的比上次要顺利一些，没有发生太多玄学问题，但是当我尝试把一部分公共代码提取出来成为process时，仍然发生了我难以理解的错误，例如初始值设定不正确，或者错误2次就报警等，因此还是只能维持这个样子。我原先以这两种代码应该是等价的，仿真的结果也确实如此，但是实际测试时，事实证明确实不是。看来我对于电路设计和vhdl的理解仍然很浅薄，但这也是本学期的最后一次实验了，我也只能期望在未来的学习生活中能够学到更多相关的知识了。