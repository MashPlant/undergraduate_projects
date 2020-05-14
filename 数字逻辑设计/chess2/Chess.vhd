library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library work;
use work.chess_objects.all;

entity Chess is
	port(
		clk_0 : in std_logic;--100M
        reset_in : in std_logic;
        ps2_clk : inout std_logic;
        ps2_data : inout std_logic;
		hs,vs: out STD_LOGIC; 
		r,g,b: out STD_LOGIC_vector(2 downto 0);
		testx0: out std_logic_vector(6 downto 0);--debug
		mrst: in std_logic;
		err:out std_logic
	);
end Chess;

architecture mchess of Chess is

component ps2_mouse is
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
end component;

component jie_logic is
	port (
		clock, reset: in std_logic;
		is_click: in std_logic;
		click_x: in integer range 0 to JIE_X - 1;
		click_y: in integer range 0 to JIE_Y - 1;
		-- out_pieces: out jie_piece_matrix;
		out_exist_array: out exist_list_type;
		out_state: out game_state;
		out_has_select: out std_logic;
		out_select_x: out integer range 0 to JIE_X - 1;
		out_select_y: out integer range 0 to JIE_Y - 1;
		out_player: out std_logic
		--out_life_r, out_life_b: out integer range 0 to 60;
		--out_score: out integer range 1 to 2048
  );
end component;

component vga_rom is
	port(
		clk_0,reset: in std_logic;
		in_exist_array: in exist_list_type;
		in_state: in game_state;
		in_player: in std_logic;
		--in_life_r, in_life_b: in integer range 0 to 60;
		--in_score_times: in integer range 0 to 2047;
		in_has_select: in std_logic;
		in_select_x: in integer range 0 to JIE_X - 1;
		in_select_y: in integer range 0 to JIE_Y - 1;
		mousex,mousey:in std_logic_vector(9 downto 0);
		hs,vs: out STD_LOGIC; 
		r,g,b: out STD_LOGIC_vector(2 downto 0)
);
end component;

signal clk50: std_logic;
signal leftb,rightb,middleb: std_logic;
signal mousex,mousey: std_logic_vector(9 downto 0);
signal myx: integer range 0 to JIE_X - 1;
signal myy: integer range 0 to JIE_Y - 1;
-- signal pieces: jie_piece_matrix;
signal exist_array: exist_list_type;
signal state: game_state;
signal player: std_logic;
--signal life_r,life_b: integer range 0 to 60;
--signal score_times: integer range 1 to 2048; 
signal has_select:std_logic;
signal select_x: integer range 0 to JIE_X-1;
signal select_y: integer range 0 to JIE_Y-1;
signal mouserst: std_logic;

begin

--mouserst <= mrst or reset_in;

input: ps2_mouse port map(
							clk_in=>clk_0,
							reset_in=>mrst,
							ps2_clk=>ps2_clk,
							ps2_data=>ps2_data,
							left_button=>leftb,right_button=>rightb,middle_button=>middleb,
							mousex=>mousex,mousey=>mousey,
							myx=>myx,myy=>myy,
							testx0=>testx0,--debug
							error_no_ack=>err
						);

clogic: jie_logic port map(
						clock=>clk_0,
						reset=>reset_in,
						is_click=>leftb,
						click_x=>myx,click_y=>myy,
						out_exist_array => exist_array,
						out_state=>state,
						out_has_select=>has_select,
						out_select_x=>select_x,
						out_select_y=>select_y,
						out_player=>player
						--out_life_r=>life_r,out_life_b=>life_b,
						--out_score=>score_times
					);
					
output: vga_rom port map(
							clk_0=>clk_0,
							reset=>reset_in,
							in_exist_array => exist_array,
							in_state=>state,
							in_player=>player,
							--in_life_r=>life_r,in_life_b=>life_b,
							--in_score_times=>score_times,
							in_has_select=>has_select,
							in_select_x=>select_x,
							in_select_y=>select_y,
							mousex=>mousex,mousey=>mousey,
							hs=>hs,vs=>vs,
							r=>r,g=>g,b=>b
						);											
end mchess;