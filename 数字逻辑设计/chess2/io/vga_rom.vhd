library ieee;
use ieee.std_logic_1164.all;

library work;
use work.chess_objects.all;

entity vga_rom is
port(
	clk_0,reset: in std_logic;
	-- in_pieces: in jie_piece_matrix;
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
end vga_rom;

architecture vga_rom of vga_rom is

component vga640480 is
	 port(
			address_board		:		  out	STD_LOGIC_VECTOR(15 DOWNTO 0);
			address_piece		:		  out	STD_LOGIC_VECTOR(14 DOWNTO 0);
			in_exist_array: in exist_list_type;
			-- in_pieces: in jie_piece_matrix;
			in_state: in game_state;
			in_player: in std_logic;
			--in_life_r, in_life_b: in integer range 0 to 60;
			--in_score_times: in integer range 0 to 2047;
			in_has_select: in std_logic;
			in_select_x: in integer range 0 to JIE_X - 1;
			in_select_y: in integer range 0 to JIE_Y - 1;
			mousex,mousey:in std_logic_vector(9 downto 0);
			reset       :         in  STD_LOGIC;
			clk50       :		  out std_logic; 
			q_board		    :		  in STD_LOGIC_vector(8 downto 0);
			q_piece		    :		  in STD_LOGIC_vector(8 downto 0);
			clk_0       :         in  STD_LOGIC; --100M����?����?��?
			hs,vs       :         out STD_LOGIC; --DD��?2??��3?��?2?D?o?
			r,g,b       :         out STD_LOGIC_vector(2 downto 0)
	  );
end component;

component board_rom IS
	PORT
	(
		address		: IN STD_LOGIC_VECTOR (15 DOWNTO 0);
		clock		: IN STD_LOGIC ;
		q		: OUT STD_LOGIC_VECTOR (8 DOWNTO 0)
	);
END component;

component piece_rom is
	PORT
	(
		address		: IN STD_LOGIC_VECTOR (14 DOWNTO 0);
		clock		: IN STD_LOGIC  := '1';
		q		: OUT STD_LOGIC_VECTOR (8 DOWNTO 0)
	);
end component;

signal address_board: std_logic_vector(15 downto 0);
signal address_piece: std_logic_vector(14 downto 0);
signal clk50: std_logic;
signal q_board: std_logic_vector(8 downto 0);
signal q_piece: std_logic_vector(8 downto 0);


begin

u1: vga640480 port map(
						address_board=>address_board, 
						address_piece=>address_piece,
						in_exist_array => in_exist_array,
						-- in_pieces=>in_pieces,
						in_state=>in_state,
						in_player=>in_player,
						--in_life_r=>in_life_r,in_life_b=>in_life_b,
						--in_score_times=>in_score_times,
						in_has_select=>in_has_select,
						in_select_x=>in_select_x,
						in_select_y=>in_select_y,
						mousex=>mousex,mousey=>mousey,
						reset=>reset, 
						clk50=>clk50, 
						q_board=>q_board,q_piece=>q_piece, 
						clk_0=>clk_0, 
						hs=>hs, vs=>vs, 
						r=>r, g=>g, b=>b
					);
u2: board_rom port map(	
						address=>address_board, 
						clock=>clk50, 
						q=>q_board
					);
u3: piece_rom port map(
						address=>address_piece,
						clock=>clk50,
						q=>q_piece
					);
end vga_rom;