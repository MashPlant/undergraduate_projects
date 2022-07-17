---
title: "计数器的设计实验报告"
author: "李晨昊 2017011466"
date: "2019-5-11"
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
1. 掌握时序电路的基本分析和设计方法
2. 理解同步时序电路和异步时序电路的区别
3. 掌握计数器电路的设计原理，用硬件描述语言实现指定功能的计数器设计
4. 学会利用软件仿真实现对数字电路的逻辑功能进行验证分析

# 实验内容
1. 使用实验平台上两个未经译码处理的数码管显示计数，手动单次时钟进行计数，时钟上升沿计数一次，当计数到59的时候，要求两个数码管都能复位到00的状态，重新计数，实验还要求设置一个复位键，可以随时重新恢复到00的状态继续计数
2. 使用实验平台上的1MHz时钟，将计数器改成秒表，在秒表中使用开关控制秒表启动暂停

# 代码及说明
```vhdl
-- 门电路实现的D触发器
-- rst位为异步置0端，rst = '0'时置0
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity d_ff is
  port (
    clk: in  std_logic;
    rst: in  std_logic;
    d: in  std_logic;
    q, q_n: out std_logic
  );
end d_ff;

architecture arch of d_ff is
  signal g1, g3, g4, g5: std_logic := '1';
  signal g2, g6: std_logic := '0';
begin
  -- 按照"数字逻辑电路"课本P74图3-25(a)的D触发器来实现
  -- 简单起见，去掉了SD位，因为本次实验没有用到
  g1 <= not(d and rst and g3);
  g2 <= not(g1 and g4);
  g3 <= not(clk and g1 and g4);
  g4 <= not(rst and g2 and clk);
  g5 <= not(rst and g3 and g6);
  g6 <= not(g5 and g4);
  q_n <= g5;
  q <= g6;
end arch; -- arch
```

```vhdl
-- 4位同步计数器，计数范围为0-15
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity counter16 is
  port (
    clk, rst: in std_logic;
    qs: out std_logic_vector(3 downto 0)
  );
end counter16;

architecture arch of counter16 is    
  component d_ff is
    port (
      clk   : in  std_logic;
      rst : in  std_logic;
      d     : in  std_logic;
      q     : out std_logic;
      q_n   : out std_logic);
  end component;
  signal not_q: std_logic_vector(3 downto 0) := "1111";
  signal q: std_logic_vector(3 downto 0) := "0000";
begin
  -- 按照同步计数器两个状态之间的转移的逻辑公式来设计代码
  d0: d_ff port map(clk => clk, rst => rst, d => not q(0), q => q(0));
  d1: d_ff port map(clk => clk, rst => rst, d => q(1) xor q(0), q => q(1));
  d2: d_ff port map(clk => clk, rst => rst, d => q(2) xor (q(1) and q(0)),
    q => q(2));
  d3: d_ff port map(clk => clk, rst => rst, 
    d => q(3) xor (q(2) and q(1) and q(0)), q => q(3));
end arch;
```

```vhdl
-- 计数范围为0-59的计数器
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity counter60 is
  port (
    clk, rst: in std_logic;
    hi, lo: out std_logic_vector(6 downto 0)
  );
end counter60;

architecture arch of counter60 is
  component decode
  port (
    bin: in std_logic_vector(3 downto 0);
    raw: out std_logic_vector(6 downto 0)
  );
  end component;

  component counter16 is
    port (
      clk, rst: in std_logic;
      qs: out std_logic_vector(3 downto 0)
    );
  end component;

  signal hi_cnt: std_logic_vector(3 downto 0) := "0000";
  signal lo_cnt: std_logic_vector(3 downto 0) := "0000";
  signal hi_rst, lo_rst, hi_clk: std_logic := '0';
begin
  -- 个位和十位各用一个4位计数器来实现
  -- 之所以分开个位和十位，一方面是因为4位计数器范围有限
  -- 另一方面是为了方便把个位和十位分别转化为led管的显示数字
  map_cnt_lo: counter16 port map(clk => clk, rst => lo_rst, qs => lo_cnt);
  map_decode_lo: decode port map(bin => lo_cnt, raw => lo);
  map_cnt_hi: counter16 port map(clk => hi_clk, rst => hi_rst, qs => hi_cnt);
  map_decode_hi: decode port map(bin => hi_cnt, raw => hi);
  process(rst, lo_cnt)
  begin
    -- 如果个位计到10或者要求置0，则对个位异步置0
    if lo_cnt = "1010" or rst = '0' then
      lo_rst <= '0';
    else
      lo_rst <= '1';
    end if;
    -- 如果个位计到10，给十位的时钟发一个信号，这样十位可以计一次
    -- 这实际上是异步计数器的思路
    if lo_cnt = "1010" then
      hi_clk <= '1';
    else 
      hi_clk <= '0';
    end if;
  end process;
  process( rst, hi_cnt )
  begin
    -- 如果十位计到6或者要求置0，则对十位异步置0
    if hi_cnt = "0110" or rst = '0' then
      hi_rst <= '0';
    else
      hi_rst <= '1';
    end if;
  end process;
end arch;
```

```vhdl
-- 译码模块
-- 直接从第一次实验的代码里复制来即可
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity decode is
  port (
    bin: in std_logic_vector(3 downto 0);
    raw: out std_logic_vector(6 downto 0)
  );
end decode;

architecture arch of decode is
begin
  handle_bin : process( bin )
  begin
    case(bin) is
      when "0000" => raw <= "1111110"; -- 0
      when "0001" => raw <= "0110000"; -- 1
      when "0010" => raw <= "1101101"; -- 2
      when "0011" => raw <= "1111001"; -- 3
      when "0100" => raw <= "0110011"; -- 4
      when "0101" => raw <= "1011011"; -- 5
      when "0110" => raw <= "0011111"; -- 6
      when "0111" => raw <= "1110000"; -- 7
      when "1000" => raw <= "1111111"; -- 8
      when "1001" => raw <= "1110011"; -- 9
      when "1010" => raw <= "1110111"; -- A
      when "1011" => raw <= "0011111"; -- B
      when "1100" => raw <= "1001110"; -- C
      when "1101" => raw <= "0111101"; -- D
      when "1110" => raw <= "1001111"; -- E
      when "1111" => raw <= "1000111"; -- F
      when others => raw <= "0000000"; -- invalid
    end case;
  end process ; -- handle_bin
end arch ; -- arch
```

```vhdl
-- 分频模块
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity freq_divider is
  port (
    clk, pause: in std_logic;
    div_clk: out std_logic
  );
end freq_divider;

architecture arch of freq_divider is
  signal cnt: integer := 0;
begin
  handle_clk : process(clk)
  begin
    if clk'event and clk = '1' then
      -- 计数器达到999999时，证明外部时钟上升沿到来了1M次
      -- 给分频后的时钟发一个pause信号
      -- 如果pause为1，表示不处于暂停状态，则输出时钟会得到一个上升沿
      if cnt = 999999 then
        cnt <= 0;
        div_clk <= pause;
      else
        cnt <= cnt + 1;
        div_clk <= '0';
      end if;
    end if;
  end process; -- handle_clk
end arch;
```

```vhdl
-- 总控代码
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity counterd is
  port (
    clk, rst: in std_logic;
    pause: in std_logic;
    hi, lo: out std_logic_vector(6 downto 0)
  );
end counterd;

architecture arch of counterd is
  component counter60 is
    port (
      clk, rst: in std_logic;
      hi, lo: out std_logic_vector(6 downto 0)
    );
  end component;

  component freq_divider is
    port (
      clk, pause: in std_logic;
      div_clk: out std_logic
    );
  end component;

  signal div_clk: std_logic;
begin
  div_clk_map: freq_divider port map(clk, pause, div_clk);
  -- 这里给div_clk或者clk分别对应于自动计时和手动计时
  counter_map: counter60 port map(div_clk, rst, hi, lo);
end arch ; -- arch
```
# 仿真

\begin{center}
\includegraphics[width=0.9\linewidth]{week11/simu.png}
\end{center}

可以看出正确的输出了0-59的数字。

# 实验小结
本次实验给我带来了很大的困难，我花了很多时间在调试代码上。其实所谓调试，基本上就是换一个看起来等价的写法，再尝试一次。最终得到了这个版本的代码，我也不敢说它一定是一个正确的实现，只是看起来能work了而已。我认为这个问题的根源在于电路设计本身的复杂性，一旦必须手动实现D触发器这种底层的东西，就很有可能碰到很多以前从没有关注过的问题，例如电路的门延时对于电路的功能的影响；另一方面，这也反映出我对数字电路的理解还不够深。我也希望在未来的实验和实践当中，我能对这些东西有一个更深刻的理解。