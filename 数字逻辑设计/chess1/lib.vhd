library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

package chess_objects is
	constant X:integer :=4;
	constant Y:integer :=8;
	
	type piece is record
		piece_type: integer range 0 to 6;--0 king 6 soldier
		color: std_logic;--0 red 1 black
		opened: std_logic;
		exist: std_logic;
	end record;
	
	type piece_vector is array(0 to X-1) of piece;
	type piece_matrix is array(0 to Y-1) of piece_vector;
	
	type game_state is(RED_WIN,BLACK_WIN,TIE,PLAYING);
end package;