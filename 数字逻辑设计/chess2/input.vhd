library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library work;
use work.chess_objects.all;

entity input is
		port(
		clock, reset: in std_logic;
		click: out std_logic;
		game_state: in game_state;
		ps2_clk: inout std_logic;
		ps2_data: inout std_logic;
		mousex, mousey: out std_logic_vector(9 downto 0); -- raw mouse coordinate  
		click_x:out integer range 0 to X-1;
		click_y:out integer range 0 to Y-1
	);
end entity;