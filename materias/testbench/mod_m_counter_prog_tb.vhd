library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.math_real.all;

entity mod_m_counter_prog_tb IS
end entity;

architecture rtl of mod_m_counter_prog_tb is
    constant M : natural := 3;


    --Inputs
    signal clk : std_logic := '1';
    signal reset: std_logic := '0';
    signal run : std_logic := '1';
    signal frec : unsigned(M-1 downto 0):= (others => '0');
    signal duty_cycle : unsigned(M-1 downto 0):= (others => '0');
    --Outputs
    signal output: std_logic;
    -- Clock period definitions
    constant clk_period : time := 1 us;


begin
    -- Clock process definitions

    clk_process : process
    begin
        clk <= '0';
        wait for clk_period/2;
        clk <= '1';
        wait for clk_period/2;
    end process;

 -- Instantiate the Unit Under Test (UUT)
    uut : entity work.led_controller
    generic map(M => M )
    port map (clk_i => clk,
              reset_i => reset,
              run_i => run,
              frec_i =>  std_logic_vector(frec),
              duty_cycle_i => std_logic_vector(duty_cycle),
              out_o=> output
          );

   -- reset <= '1', '0' after 5 us;
   -- run <= '0', '1' after 20 us;
signals_change: process
begin
    duty_cycle <= "100";
    wait for 50 us;
    frec <= "001";
    wait for 50 us;
    frec <= "010";
    wait for 50 us;
    frec <= "011";
    end process;

end architecture;



--library IEEE;
--use IEEE.std_logic_1164.all;
--use IEEE.numeric_std.all;
--use IEEE.math_real.all;
--
--entity mod_m_counter_prog_tb IS
--end entity;
--
--architecture rtl of mod_m_counter_prog_tb is
--    constant M : natural := 3;
--
--
--    --Inputs
--    signal clk : std_logic := '1';
--    signal reset: std_logic := '1';
--    signal run : std_logic := '1';
--    signal max_count : unsigned(M-1 downto 0):= (others => '1');
--    signal duty_cycle : unsigned(M-1 downto 0):= (others => '0');
--    --Outputs
--    signal output: std_logic;
--    -- Clock period definitions
--    constant clk_period : time := 1 us;
--
--
--begin
--    -- Clock process definitions
--
--    clk_process : process
--    begin
--        clk <= '0';
--        wait for clk_period/2;
--        clk <= '1';
--        wait for clk_period/2;
--    end process;
--
-- -- Instantiate the Unit Under Test (UUT)
--    uut : entity work.mod_m_counter_prog
--    generic map(M => M )
--    port map (clk_i => clk,
--              reset_i => reset,
--              run_i => run,
--              max_count_i =>  std_logic_vector(max_count),
--              duty_cycle_i => std_logic_vector(duty_cycle),
--              out_o=> output
--          );
--
--    reset <= '1', '0' after 5 us;
--    run <= '0', '1' after 20 us;
--    signals_change: process
--    begin
--    for i in 0 to M-1 loop
--         duty_cycle <= duty_cycle + 1;
--         wait for 30 us;
--    end loop;
--
--    end process;
--end architecture;

