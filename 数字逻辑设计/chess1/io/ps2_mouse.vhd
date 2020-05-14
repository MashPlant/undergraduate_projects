--------------------------------------------------------------------------------------------
--��Ŀ����	:	Mousee Picture Viewer
--�ļ���	:	ps2.vhd
--ģ�鹦��	��	ps/2���Э���ʵ�֣�����ʱ��ʵ�������ƶ�����������������
---------------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity myencoder is
	port(
		inputv:in std_logic_vector(9 downto 0);
		output0:out std_logic_vector(6 downto 0)
		--,output1,output2,output3
);
end myencoder;

architecture en of myencoder is

signal b0,b1,b2,b3:integer range 0 to 9 :=0;
signal input:integer;

begin
	input<=conv_integer(inputv);
	b0<=input mod 10;
	process(input)
	begin
	case b0 is
		when 0 => output0 <= "1111110" ;
		when 1 => output0 <= "1100000" ;
		when 2 => output0 <= "1011101" ;
		when 3 => output0 <= "1111001" ;
		when 4 => output0 <= "1100011" ;
		when 5 => output0 <= "0111011" ;
		when 6 => output0 <= "0111111" ;
		when 7 => output0 <= "1101000" ;
		when 8 => output0 <= "1111111" ;
		when 9 => output0 <= "1111011" ;
		when others => output0 <= "0000000" ;
	end case ;
	
	-- case b1 is
	-- 	when 0 => output1 <= "1111110" ;
	-- 	when 1 => output1 <= "1100000" ;
	-- 	when 2 => output1 <= "1011101" ;
	-- 	when 3 => output1 <= "1111001" ;
	-- 	when 4 => output1 <= "1100011" ;
	-- 	when 5 => output1 <= "0111011" ;
	-- 	when 6 => output1 <= "0111111" ;
	-- 	when 7 => output1 <= "1101000" ;
	-- 	when 8 => output1 <= "1111111" ;
	-- 	when 9 => output1 <= "1111011" ;
	-- 	when others => output1 <= "0000000" ;
	-- end case ;
	
	-- case b2 is
	-- 	when 0 => output2 <= "1111110" ;
	-- 	when 1 => output2 <= "1100000" ;
	-- 	when 2 => output2 <= "1011101" ;
	-- 	when 3 => output2 <= "1111001" ;
	-- 	when 4 => output2 <= "1100011" ;
	-- 	when 5 => output2 <= "0111011" ;
	-- 	when 6 => output2 <= "0111111" ;
	-- 	when 7 => output2 <= "1101000" ;
	-- 	when 8 => output2 <= "1111111" ;
	-- 	when 9 => output2 <= "1111011" ;
	-- 	when others => output2 <= "0000000" ;
	-- end case ;
	
	-- case b3 is
	-- 	when 0 => output3 <= "1111110" ;
	-- 	when 1 => output3 <= "1100000" ;
	-- 	when 2 => output3 <= "1011101" ;
	-- 	when 3 => output3 <= "1111001" ;
	-- 	when 4 => output3 <= "1100011" ;
	-- 	when 5 => output3 <= "0111011" ;
	-- 	when 6 => output3 <= "0111111" ;
	-- 	when 7 => output3 <= "1101000" ;
	-- 	when 8 => output3 <= "1111111" ;
	-- 	when 9 => output3 <= "1111011" ;
	-- 	when others => output3 <= "0000000" ;
	-- end case ;
	end process;
end en;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity ps2_mouse is
  port(
        clk_in : in std_logic;
        reset_in : in std_logic;
        ps2_clk : inout std_logic;
        ps2_data : inout std_logic;
        left_button : out std_logic;
        right_button : out std_logic;
	    middle_button : out std_logic;
        mousex: buffer std_logic_vector(9 downto 0);
        mousey: buffer std_logic_vector(9 downto 0);
        myx: buffer integer;
        myy:buffer integer;
        testx0: out std_logic_vector(6 downto 0);--debug
        error_no_ack : out std_logic 
);
       
end ps2_mouse;

architecture behavioral of ps2_mouse is
component myencoder
	port(
		inputv:in std_logic_vector(9 downto 0);
		output0:out std_logic_vector(6 downto 0)
);
end component;

constant x_max:  integer:=800;
constant y_max:  integer:=600;


constant total_bits     : integer:=33; -- number of bits in one full packet
constant watchdog       : integer:=100;-- number of sys_clks for 400usec.
constant debounce_timer : integer:=2;  -- number of sys_clks for debounce:2

type m1statetype is ( m1_clk_h,
                      m1_falling_edge,
                      m1_falling_wait,
                      m1_clk_l,
                      m1_rising_edge,
                      m1_rising_wait );

type m2statetype is ( m2_reset,
                      m2_wait,
                      m2_gather,
                      m2_use,
                      m2_hold_clk_l,
                      m2_data_low_1,
                      m2_data_high_1,
                      m2_data_low_2,
                      m2_data_high_2,
                      m2_data_low_3,
                      m2_data_high_3,
                      m2_error_no_ack,
                      m2_await_response );

signal m1_state, m1_next_state : m1statetype;--the two states
signal m2_state, m2_next_state : m2statetype;

signal watchdog_timer_done,debounce_timer_done : std_logic;--signals of command from host to mouse 
signal q : std_logic_vector(total_bits-1 downto 0);--bit sequence
signal bitcount : std_logic_vector(5 downto 0);--bit count

signal watchdog_timer_count : std_logic_vector(8 downto 0); --wait time
signal debounce_timer_count : std_logic_vector(1 downto 0); --debounce time
signal ps2_clk_hi_z : std_logic;     -- without keyboard, high z equals 1 due to pullups.
signal ps2_data_hi_z : std_logic;    -- without keyboard, high z equals 1 due to pullups.

signal clean_clk : std_logic;        -- debounced output from m1, follows ps2_clk.
signal rise,n_rise : std_logic;      -- output from m1 state machine.
signal fall,n_fall : std_logic;      -- output from m1 state machine.

signal output_strobe : std_logic;    -- latches data into the output registers(ѡͨ����)
signal packet_good : std_logic;      -- check whether the data is valid 
signal clk,reset : std_logic;  
signal count : std_logic_vector(20 downto 0);  

begin

--myx<=CONV_INTEGER(mousex);
--myy<=CONV_INTEGER(mousey);

displayx: myencoder port map(mousex,testx0);
--displayy: myencoder port map(myy,testy0,testy1,testy2,testy3);

reset<=not reset_in;
ps2_clk <= '0' when ps2_clk_hi_z='0' else 'Z';
ps2_data <= '0' when ps2_data_hi_z='0' else 'Z';




	process(clk_in)
	begin
		if reset='1' then
			count<=(others=>'0');
			clk<='0';		
		elsif clk_in'EVENT and clk_in = '1' then
			if count < 200 then
				count <= count + 1;				
			else
				clk <= not clk;
				count<=(others=>'0');
			end if;
		end if;
	end process;

---------------m1 state
m1state: process (reset, clk)
begin 


  if (reset='1') then
    rise <= '0';
    fall <= '0';
    m1_state <= m1_clk_h;
  elsif (clk'event and clk='1') then 
    m1_state <= m1_next_state;
    rise <= n_rise;
    fall <= n_fall;
  end if;
end process;

-- state transition logic
m1statetr: process (m1_state, ps2_clk, debounce_timer_done)
begin
  -- output signals default to this value, unless changed in a state condition.
  clean_clk <= '0';
  n_rise <= '0';
  n_fall <= '0';

  case m1_state is
    when m1_clk_h =>
      clean_clk <= '1';
      if (ps2_clk='0') then
        m1_next_state <= m1_falling_edge;
      else 
        m1_next_state <= m1_clk_h;
      end if;

    when m1_falling_edge =>
      n_fall <= '1';
      m1_next_state <= m1_falling_wait;

    when m1_falling_wait =>
      if (debounce_timer_done='1') then
        m1_next_state <= m1_clk_l;
      else 
        m1_next_state <= m1_falling_wait;
      end if;
---------------------------------------------------the follows are almost the same to above
    when m1_clk_l =>
      if (ps2_clk='1') then
        m1_next_state <= m1_rising_edge;
      else 
        m1_next_state <= m1_clk_l;
      end if;

    when m1_rising_edge =>
      n_rise <= '1';
      m1_next_state <= m1_rising_wait;

    when m1_rising_wait =>
      clean_clk <= '1';
      if (debounce_timer_done='1') then
        m1_next_state <= m1_clk_h;
      else
        m1_next_state <= m1_rising_wait;
      end if;
---------------------------------------------------
    when others => m1_next_state <= m1_clk_h;
  end case;
end process;


------------------m2 state
m2state: process (reset, clk)
begin 
  if (reset='1') then
     m2_state <= m2_reset;
  elsif (clk'event and clk='1') then 
     m2_state <= m2_next_state;
  end if;
end process;

--m2 state transition logic
m2statetr: process (m2_state, fall, watchdog_timer_done, bitcount)
begin
  -- output signals default to this value, unless changed in a state condition.
  ps2_clk_hi_z <= '1';
  ps2_data_hi_z <= '1';
  error_no_ack <= '0';
  output_strobe <= '0';

  case m2_state is
    when m2_reset =>    -- after reset, send command to mouse.
      m2_next_state <= m2_hold_clk_l;

    when m2_wait =>
      if (fall='1') then
        m2_next_state <= m2_gather;
      else
        m2_next_state <= m2_wait;
      end if;

    when m2_gather =>
      if watchdog_timer_done='1' and bitcount=total_bits then
        m2_next_state <= m2_use;
      else
        m2_next_state <= m2_gather;
      end if;

    when m2_use =>
      output_strobe <= '1';
      m2_next_state <= m2_wait;

--------------------------------------------------------------------------
-- the following 9 states are used to send host command to mouse
-- for enable the stream mode, then wait the response from mouse
-- Due to the protocol, we must send an "0xF4" to enable it!!

    when m2_hold_clk_l =>
      ps2_clk_hi_z <= '0';   -- this starts the watchdog timer,����ʱ������!
      if (watchdog_timer_done='1') then
        m2_next_state <= m2_data_low_1;
      else 
        m2_next_state <= m2_hold_clk_l;
      end if;

    when m2_data_low_1 =>
      ps2_data_hi_z <= '0';  -- forms start bit, d[0] and d[1]
      if (fall='1' and (bitcount = 2)) then
        m2_next_state <= m2_data_high_1;
      else 
        m2_next_state <= m2_data_low_1;
      end if;

    when m2_data_high_1 =>
      ps2_data_hi_z <= '1';  -- forms d[2]
      if (fall='1' and (bitcount = 3)) then
        m2_next_state <= m2_data_low_2;
      else 
        m2_next_state <= m2_data_high_1;
      end if;

    when m2_data_low_2 =>
      ps2_data_hi_z <= '0';  -- forms d[3]
      if (fall='1' and (bitcount = 4)) then
          m2_next_state <= m2_data_high_2;
      else 
         m2_next_state <= m2_data_low_2;
      end if;

    when m2_data_high_2 =>
      ps2_data_hi_z <= '1';  -- forms d[4],d[5],d[6],d[7]
      if (fall='1' and (bitcount = 8)) then
          m2_next_state <= m2_data_low_3;
      else 
         m2_next_state <= m2_data_high_2;
      end if;

    when m2_data_low_3 =>
      ps2_data_hi_z <= '0';  -- forms parity bit
      if (fall='1') then
        m2_next_state <= m2_data_high_3;
      else 
        m2_next_state <= m2_data_low_3;
      end if;

    when m2_data_high_3 =>
        ps2_data_hi_z <= '1';  -- allow mouse to pull low (ack pulse)
      if (fall='1' and (ps2_data='1')) then
        m2_next_state <= m2_error_no_ack;
      elsif (fall='1' and (ps2_data='0')) then
        m2_next_state <= m2_await_response;
      else 
        m2_next_state <= m2_data_high_3;
      end if;
-------------------------------------------------------------------------
    when m2_error_no_ack =>
      error_no_ack <= '1';
      m2_next_state <= m2_error_no_ack;
    
    when m2_await_response =>
      m2_next_state <= m2_use;

    when others => m2_next_state <= m2_wait;
  end case;
end process;

-------------------------------------------------------------------------
-------------------------------------------------------------------------
bitcnt: process (reset, clk)
begin
  if (reset='1') then
     bitcount <= (others=>'0');
  elsif (clk'event and clk='1') then
    if (fall='1') then
      bitcount <= bitcount + 1;
    elsif (watchdog_timer_done='1') then
      bitcount <= (others=>'0');
	end if;
  end if;
end process;
-------------------------------------------------------------------------
dataseq: process (reset, clk)--��λ�Ĵ��������벢��33λ���ݣ���3��
begin
  if (reset='1') then
    q <= (others=>'0');

  elsif (clk'event and clk='1') then
    if (fall='1') then
      q <= ps2_data & q(total_bits-1 downto 1);
	end if;
  end if;
end process;
-------------------------------------------------------------------------
watchcount: process (reset, rise, fall, clk)--��ʱ400us���������ͼ�ʱ
begin
  if (reset='1' or rise='1' or fall='1') then
    watchdog_timer_count <= (others=>'0');
  elsif (clk'event and clk='1') then
    if (watchdog_timer_done='0') then
      watchdog_timer_count <= watchdog_timer_count + 1;
	end if;
  end if;
end process;

watchdog_timer_done <= '1' when (watchdog_timer_count=watchdog-1) else '0';
-------------------------------------------------------------------------
deboucount: process (reset, rise, fall, clk)
begin
  if (reset='1' or rise='1' or fall='1') then
    debounce_timer_count <= (others=>'0');
  elsif (clk'event and clk='1') then
    if (debounce_timer_done='0') then
      debounce_timer_count <= debounce_timer_count + 1;
    end if;
  end if;
end process;

debounce_timer_done <= '1' when(debounce_timer_count=debounce_timer-1) else '0';
-------------------------------------------------------------------------
button: process (reset, clk) --��������
begin
  if (reset='1') then
    left_button <= '0';
    right_button <= '0';
	middle_button <= '0';
  elsif (clk'event and clk='1') then
    if (output_strobe='1') then
      left_button <= q(1);
      right_button <= q(2);
	  middle_button <= q(3);
	  
	  myx<=(CONV_INTEGER(mousey)-160)/37;
	  myy<=(CONV_INTEGER(mousex)-167)/37;
	  
    end if;
  end if;   
  
  
              
end process;

x: process (reset, clk)  --����X����
begin
  if (reset='1') then
 	mousex <= CONV_STD_LOGIC_VECTOR(x_max/2,10);  -- 400
  elsif (clk'event and clk='1') then
    if (output_strobe='1') then
      if ((mousex >= x_max and q(5)='0') or (mousex <= 1 and q(5)='1')) then
        mousex <= mousex;
      else
        mousex <= mousex + (q(5) & q(5) & q(19 downto 12));
      end if;
    end if;
  end if;
  
  --myx<=CONV_INTEGER(mousex);

  
end process;

y: process (reset, clk) --����Y����
begin
  if (reset='1') then
	 mousey <= CONV_STD_LOGIC_VECTOR(y_max/2,10);  -- 300
  elsif (clk'event and clk='1') then
    if (output_strobe='1') then
      if ((mousey >= y_max and q(6)='1') or (mousey <= 1 and q(6)='0')) then
        mousey <= mousey;
      else
        mousey <= mousey + (not (q(6) & q(6) & q(30 downto 23)) + "1");
      end if;
    end if;
  end if;
  
  --myy<=CONV_INTEGER(mousey);
  
end process;


end behavioral;
