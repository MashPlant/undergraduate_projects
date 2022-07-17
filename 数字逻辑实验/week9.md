---
title: "四位加法器实验报告"
author: "李晨昊 2017011466"
date: "2019-5-1"
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
1. 掌握组合逻辑电路的基本分析和设计方法。
2. 理解半加器和全加器的工作原理并掌握利用全加器构成不同字长的加法器的各种方法。
3. 学习元件例化的方式进行硬件电路设计。
4. 学会利用软件仿真实现对数字逻辑电路的逻辑功能进行验证和分析。

# 实验内容
1. 设计实现逐次进位加法器,进行软件仿真并在实验平台上进行测试。
2. 设计实现超前进位加法器,进行软件仿真并在实验平台上进行测试。

# 代码及说明
## 一位全加器
```vhdl
entity one_adder is
  port (
    x, y, c0: in std_logic;
    s, c1: out std_logic;
    p, g: out std_logic
  );
end one_adder;

architecture arch of one_adder is
begin
  -- 以下按照课本上的公式计算四个输出
  s <= x xor y xor c0;
  c1 <= (x and y) or ((x or y) and c0);
  p <= x xor y;
  g <= x and y;
end arch;
```
实体`one_adder`实现了一位加法器，输入为参与加法的两个bit和前一位的进位bit，输出为本位的全加和，进位，pass和generate位。工作原理为按照逻辑公式直接计算需要输出的这四个bit。

## 四位逐次进位加法器
```vhdl
entity seq_adder is
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic
  ) ;
end seq_adder;

architecture arch of seq_adder is
  component one_adder
  port (
    x, y, c0: in std_logic;
    s, c1: out std_logic;
    p, g: out std_logic
  );
  end component;
  signal c: std_logic_vector(3 downto 1);

begin
  -- 一位全加器的p和g输出忽略不用
  add0: one_adder port map(x => x(0), y => y(0), c0 => c0, s => s(0), c1 => c(1));
  add1: one_adder port map(x => x(1), y => y(1), c0 => c(1), s => s(1), c1 => c(2));
  add2: one_adder port map(x => x(2), y => y(2), c0 => c(2), s => s(2), c1 => c(3));
  add3: one_adder port map(x => x(3), y => y(3), c0 => c(3), s => s(3), c1 => c4);
end arch;
```
实体`seq_adder`通过元件例化`one_adder`来实现了四位逐次进位加法器。工作原理为逐位计算每一位的全加和和进位，并且把本位的进位信息传递给下一位。

## 四位超前进位加法器
```vhdl
entity par_adder is
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic
  ) ;
end par_adder;

architecture arch of par_adder is
  component one_adder
  port (
    x, y, c0: in std_logic;
    s, c1: out std_logic;
    p, g: out std_logic
  );
  end component;
  
  signal c, p, g: std_logic_vector(3 downto 0);
begin
  -- 一位全加器c0锁定为0，c1输出忽略不用
  add0: one_adder port map(x => x(0), y => y(0), c0 => '0', p => p(0), g => g(0));
  add1: one_adder port map(x => x(1), y => y(1), c0 => '0', p => p(1), g => g(1));
  add2: one_adder port map(x => x(2), y => y(2), c0 => '0', p => p(2), g => g(2));
  add3: one_adder port map(x => x(3), y => y(3), c0 => '0', p => p(3), g => g(3));
  c(0) <= g(0) or (p(0) and c0);
  c(1) <= g(1) or (p(1) and g(0)) or (p(1) and p(0) and c0);
  c(2) <= g(2) or (p(2) and g(1)) or (p(2) and p(1) and g(0)) 
    or (p(2) and p(1) and p(0) and c0);
  c(3) <= g(3) or (p(3) and g(2)) or (p(3) and p(2) and g(1)) 
    or (p(3) and p(2) and p(1) and g(0)) or (p(3) and p(2) and p(1) and p(0) and c0);
  s(0) <= p(0) xor c0;
  s(1) <= p(1) xor c(0);
  s(2) <= p(2) xor c(1);
  s(3) <= p(3) xor c(2);
  c4 <= c(3);
end arch;
```
实体`par_adder`通过元件例化`one_adder`来实现了四位超前进位加法器。工作原理为同时计算四位的进位bit，每一个进位bit算出来后，就可以计算对应位的全加和。

## 总控代码
```vhdl
entity adder is
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic;
    kind: in std_logic -- 0 for seq, 1 for par
  );
end adder;

architecture arch of adder is
  component seq_adder
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic
  );
  end component;

  component par_adder
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic
  );
  end component;

  signal s0, s1: std_logic_vector(3 downto 0);
  signal c40, c41 : std_logic;
  signal sys : std_logic_vector(4 downto 0);
begin
  add0: seq_adder port map(x => x, y => y, c0 => c0, s => s0, c4 => c40);
  add1: par_adder port map(x => x, y => y, c0 => c0, s => s1, c4 => c41);
  -- 如果kind为0则选择逐次进位加法器，否则选择超前进位加法器
  s <= s0 when kind = '0' else s1;
  c4 <= c40 when kind = '0' else c41;
end arch ; -- arch
```
总控代码负责根据用户选择的加法器种类来选择输出哪个加法器的结果，不过因为二者的实现都是正确的，所以大体上输出不会有什么不同。

# 功能仿真
我使用了`ghdl`工具来功能仿真，可以很容易地通过编写`vhdl`代码来测试实现正确性。

功能仿真代码如下：

```vhdl
entity tb is
end tb;

architecture arch of tb is
  component adder
  port (
    x, y: in std_logic_vector(3 downto 0);
    c0: in std_logic;
    s: out std_logic_vector(3 downto 0);
    c4: out std_logic;
    kind: in std_logic -- 0 for seq, 1 for par
  );
  end component;

  signal x, y, s_par, s_seq, s_builtin: std_logic_vector(3 downto 0);
  signal c0, c4_par, c4_seq, c4_builtin: std_logic;
  signal tmp: std_logic_vector(4 downto 0);
begin
  seq: adder port map(x => x, y => y, c0 => c0, s => s_seq, c4 => c4_seq, 
    kind => '0');
  par: adder port map(x => x, y => y, c0 => c0, s => s_par, c4 => c4_par, 
    kind => '1');
  -- 用内置的加法计算x+y+c0，为了得到进位，需把加法扩展成五位加，则最高位为进位
  tmp <= "00000" + x + y + c0;
  s_builtin <= tmp(3 downto 0);
  c4_builtin <= tmp(4);
  
  process
  begin
    -- 遍历加法器输入的16*16*2=512种情况
    for i in 0 to 15 loop
      for j in 0 to 15 loop
        for c in std_logic range '0' to '1' loop
          x <= conv_std_logic_vector(i, 4);
          y <= conv_std_logic_vector(j, 4);
          c0 <= c;
          wait for 20 ns;
          -- 检查结果正确性
          -- 之所以写的这么繁琐，不能直接用=号，与运算符重载的名字冲突有关
          assert ieee.std_logic_unsigned."="(s_seq, s_builtin) 
            report "seq s error!" severity error;
          assert c4_seq = c4_builtin report "seq c4 error!" severity error;
          assert ieee.std_logic_unsigned."="(s_par, s_builtin) 
            report "par s error!" severity error;
          assert c4_seq = c4_builtin report "par c4 error!" severity error;
        end loop;
      end loop;
    end loop;
  end process;
end arch;

```

我还编写了一个`makefile`，可以一键执行功能仿真：

```makefile
all:
	ghdl -a --ieee=synopsys *.vhd # 编译所有vhdl文件到.o文件
	ghdl -e --ieee=synopsys tb # 将tb编译成可执行文件
	ghdl -r tb --stop-time=10000ns --vcd=led.vcd # 执行仿真
	gtkwave led.vcd # 使用gtkwave查看仿真波形图

clean:
	rm *.o tb
```

仿真顺利结束，任何一个`assert`都没有失败。得到波形图如下：

\begin{center}
\includegraphics[width=0.9\linewidth]{week9/function.png}
\end{center}

# 时序仿真
1. 四位逐次进位加法器

\begin{center}
\includegraphics[width=0.9\linewidth]{week9/seq.png}
\end{center}

2. 四位超前进位加法器

\begin{center}
\includegraphics[width=0.9\linewidth]{week9/par.png}
\end{center}

3. vhdl自带+号

\begin{center}
\includegraphics[width=0.9\linewidth]{week9/builtin.png}
\end{center}

可见三者的差异不是很大。

# 总结
本次CPLD进行的相对顺利，没有碰到什么奇怪的问题。在学习了相关的语法和用法后，基本上可以很容易地写出相关代码。我是用了`ghdl`工具进行了功能仿真，可以非常快速地验证正确性；我还是用了`quartus`进行时序仿真，为了进行时序仿真，需要手动建立波形等，工作量还是稍大一些，而时序仿真的输出结果也不像功能仿真那样理想，这体现了电路设计的复杂性，并不像理论结果那样显然。