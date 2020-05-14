library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; 

library work;
use work.chess_objects.all;

entity tb is
end tb;

architecture arch of tb is
  function player2str(p: std_logic) return string is
    variable ret: string(1 to 1);
  begin
    case(p) is
      when '0' => ret := "r";
      when '1' => ret := "b";
      when others => report "Err";
    end case;
    return ret;
  end player2str;

  function piece2str(p: piece) return string is
    variable piece_type: string(1 to 1);
    variable color: string(1 to 1);
    variable opened: string(1 to 1);
    
  begin
    if p.exist = '0' then
      return "----- ";
    end if;
    case(p.piece_type) is
      when 0 => piece_type := "S";
      when 1 => piece_type := "s";
      when 2 => piece_type := "x";
      when 3 => piece_type := "c";
      when 4 => piece_type := "m";
      when 5 => piece_type := "p";
      when 6 => piece_type := "b";
    end case;
    case(p.color) is
      when '0' => color := "r";
      when '1' => color := "b";
      when others => report "Err";
    end case;
    case(p.opened) is
      when '0' => opened := "-";
      when '1' => opened := "+";
      when others => report "Err";
    end case;
    return piece_type & "(" & color & opened & ") ";
  end piece2str;

  component logic
  port (
    clock, reset: in std_logic;
    is_click: in std_logic;
    click_x: in integer range 0 to X - 1;
    click_y: in integer range 0 to Y - 1;
    out_pieces: out piece_matrix;
    out_state: out game_state;
    out_player: out std_logic;
    out_life_r, out_life_b: out integer range 0 to 60;
    out_score_times: out integer range 0 to 2047
  );
  end component;
  signal reset: std_logic;
  signal is_click: std_logic;
  signal click_x: integer range 0 to X - 1;
  signal click_y: integer range 0 to Y - 1;
  signal out_pieces: piece_matrix;
  signal out_state: game_state;
  signal out_player: std_logic;
  signal out_life_r, out_life_b: integer range 0 to 60;

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
  map1: logic port map('0', reset, is_click, click_x, click_y, out_pieces, out_state, out_player, out_life_r, out_life_b);
  
  process
  begin
    -- for iter in 0 to 10 loop
    make_edge(reset);
    -- for i in 0 to Y - 1 loop
    --   report piece2str(out_pieces(i)(0)) & piece2str(out_pieces(i)(1)) & piece2str(out_pieces(i)(2)) & piece2str(out_pieces(i)(3));
    -- end loop;
    -- report "cur player: " & player2str(out_player) & " red life: " & integer'image(out_life_r) & " black life: " & integer'image(out_life_b);
    -- report "";
    
    -- is_click <= '0';
    click_x <= 1;
    click_y <= 1;
    make_edge(is_click);

    click_x <= 3;
    click_y <= 1;
    make_edge(is_click);

    click_x <= 3;
    click_y <= 3;
    make_edge(is_click);

    click_x <= 3;
    click_y <= 1;
    make_edge(is_click);

    click_x <= 3;
    click_y <= 3;
    make_edge(is_click);

    -- for i in 0 to Y - 1 loop
    --   report piece2str(out_pieces(i)(0)) & piece2str(out_pieces(i)(1)) & piece2str(out_pieces(i)(2)) & piece2str(out_pieces(i)(3));
    -- end loop;
    -- report "cur player: " & player2str(out_player) & " red life: " & integer'image(out_life_r) & " black life: " & integer'image(out_life_b);
    -- report "";
    

    -- click_x <= 1;
    -- click_y <= 0;
    -- make_edge(is_click);

    for i in 0 to Y - 1 loop
      report piece2str(out_pieces(i)(0)) & piece2str(out_pieces(i)(1)) & piece2str(out_pieces(i)(2)) & piece2str(out_pieces(i)(3));
    end loop;
    report "cur player: " & player2str(out_player) & " red life: " & integer'image(out_life_r) & " black life: " & integer'image(out_life_b);
    report "";

    -- end loop;
    report "---------END OF TEST---------" severity failure;
  end process;

  -- report "i value: " & integer'image(i);
end arch ; -- arch