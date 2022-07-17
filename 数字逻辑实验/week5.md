---
title: "点亮数字人生实验报告"
author: "李晨昊 2017011466"
date: "2019-3-29"
output:
  pdf_document:
    latex_engine: xelatex
    number_sections: yes
    toc: yes
  word_document:
    toc: yes
  html_document:
    toc: yes
header-includes: \usepackage{ctex}
---

\newpage

# 实验目的
1. 通过数码管点亮程序，熟悉 VHDL 语言，了解掌握硬件程序的编写规范。
2. 掌握 EDA 软件的使用方法和工作流程。
3. 进一步理解可编程芯片的工作原理。

# 实验内容
1. 同时点亮一个经过译码的数码管和一个未经过译码的数码管。数码管从0到f(带译码的显示0到9)。
2. 设计一个数码管显示实验，要求7段数码管有规律的显示数列(例如：奇数列，偶数列，自然数列等)，尽可能多地点亮数码管。要求试验中至少使用一个不带译码的数码管。

# 代码及工作原理
## 基本要求
```vhdl
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity digital_7 is
  port (
    key: in std_logic_vector(3 downto 0); -- 作为输入的四个开关
    display: out std_logic_vector(6 downto 0); -- 向带译码的数码管的输出信号
    display_4: out std_logic_vector(3 downto 0) -- 向不带译码的数码管的输出信号
  );
end digital_7;

architecture bhv of digital_7 is
begin
  display_4 <= key; -- 直接把输入信号输出到带译码的数码管

  process(key) -- 该进程对输入的变化敏感
  begin
    case(key) is -- 枚举输入的所有可能，确定不带译码的数码管的的每个位
      when "0000" => display <= "1111110"; -- 0
      when "0001" => display <= "0110000"; -- 1
      when "0010" => display <= "1101101"; -- 2
      when "0011" => display <= "1111001"; -- 3
      when "0100" => display <= "0110011"; -- 4
      when "0101" => display <= "1011011"; -- 5
      when "0110" => display <= "0011111"; -- 6
      when "0111" => display <= "1110000"; -- 7
      when "1000" => display <= "1111111"; -- 8
      when "1001" => display <= "1110011"; -- 9
      when "1010" => display <= "1110111"; -- A
      when "1011" => display <= "0011111"; -- B
      when "1100" => display <= "1001110"; -- C
      when "1101" => display <= "0111101"; -- D
      when "1110" => display <= "1001111"; -- E
      when "1111" => display <= "1000111"; -- F
      when others => display <= "0000000"; -- invalid
    end case;
  end process; 
end bhv; -- bhv
```

## 提高要求
```vhdl
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity digital7_ext is
  port (
    clk: in std_logic; -- 时钟输入信号
    rst: in std_logic; -- 复位输入信号
    nat: out std_logic_vector(3 downto 0); -- 向带译码的数码管输出自然数的信号
    odd: out std_logic_vector(3 downto 0); -- 向带译码的数码管输出奇数的信号
    eve: out std_logic_vector(6 downto 0) -- 向不带译码的数码管输出偶数的信号
  );
end digital7_ext;

architecture bhv of digital7_ext is
  signal sig_nat: std_logic_vector(3 downto 0) := "0000"; -- 维护目前的自然数状态
  signal sig_eve: std_logic_vector(3 downto 0) := "0000"; -- 维护目前的偶数状态
  signal cnt: integer := 0; -- 计数器，用于控制数码管变化的频率

begin
  process(clk)
  begin
    if clk'event and clk = '1' then -- 检测时钟上升沿
      if cnt = 1000000 then -- 计数器每变化1000000次才通知数码管变化一次，控制变化频率
        cnt <= 0;
        if rst = '1' then -- 同步复位
          sig_nat <= "0000";
          sig_eve <= "0000";        
        else
          if sig_nat = "1001" then -- 自然数达到9时，归零
            sig_nat <= "0000";
          else
            sig_nat <= sig_nat + 1;
          end if;
          if sig_eve = "1000" then -- 偶数达到8时，归零
            sig_eve <= "0000";
          else
            sig_eve <= sig_eve + 2; -- 偶数每次加2
          end if;
        end if;
      else 
        cnt <= cnt + 1;
      end if;
    end if;
  end process;
  
  nat <= sig_nat;
  odd <= sig_eve + 1; -- 奇数的输出可以直接由偶数的输出+1得到
  process(sig_eve) -- 对偶数先经过译码再输出
  begin
    case(sig_eve) is
      when "0000" => eve <= "1111110"; -- 0
      when "0001" => eve <= "0110000"; -- 1
      when "0010" => eve <= "1101101"; -- 2
      when "0011" => eve <= "1111001"; -- 3
      when "0100" => eve <= "0110011"; -- 4
      when "0101" => eve <= "1011011"; -- 5
      when "0110" => eve <= "0011111"; -- 6
      when "0111" => eve <= "1110000"; -- 7
      when "1000" => eve <= "1111111"; -- 8
      when "1001" => eve <= "1110011"; -- 9
      when "1010" => eve <= "1110111"; -- A
      when "1011" => eve <= "0011111"; -- B
      when "1100" => eve <= "1001110"; -- C
      when "1101" => eve <= "0111101"; -- D
      when "1110" => eve <= "1001111"; -- E
      when "1111" => eve <= "1000111"; -- F
      when others => eve <= "0000000"; -- invalid
    end case;
  end process;
end bhv; -- bhv
```

# 功能测试结果
## 基本要求

带译码和不带译码的数码管都能正确显示0-9的数字，二者的结果一致；对于A-F的数字，带译码的数码管设计时认为这样的输入不合法，因此全灭，不带译码的数码管因为我正确编写了处理的逻辑，依然可以正确输出字母。

## 提高要求

接入1MHz的时钟时，所有数字每隔1s跳动一次，三者分别按照0123456789，13579，02468的周期循环。接入更高频率的时钟时，数字变化的频率更高。

# 实验小结
1. 这是我第一次进行可编程器件，在编程之前搭建环境的时候遇到了很大的困难：Linux版本的quartus无法正常编译代码，不得已安装了Windows虚拟机，在其上安装了quartus。这一过程花费了很多时间。
2. 开始写代码之前我首先仔细阅读了书上的VHDL语言的介绍，也阅览了网上相关的资料，因此编程的过程较为顺利。
3. 通过这次实验，我对于硬件描述语言有了一定的理解，但是因为时间所限，有些遇到的问题没有深入研究。希望能在未来的实验中不断加深我的理解。