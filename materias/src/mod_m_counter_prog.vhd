library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.math_real.all;

entity mod_m_counter_prog is
    generic(M : natural -- Modulo
    );
    port(clk_i       : in std_logic;
        reset_i      : in std_logic;
        run_i        : in std_logic; --  Activador
        frec_i       : in std_logic_vector (M-1 downto 0);
        duty_cycle_i : in std_logic_vector (M-1 downto 0);
        out_o        : out std_logic
    );
end entity;

architecture rtl of mod_m_counter_prog is
    signal r_reg  : unsigned(M-1 downto 0):= (others => '0');
    signal r_next : unsigned(M-1 downto 0):= (others => '0');
begin
    NXT_STATE_PROC: process(clk_i)
    begin
        if rising_edge(clk_i) then
            if (reset_i = '1') then
                r_reg <= (others => '0');
            elsif run_i = '1' then
                r_reg <= r_next;
            end if;
        end if;
    end process;
    r_next <= (others => '0') when r_reg = unsigned(frec_i) else r_reg + 1;
    out_o <= '1' when (r_next < unsigned(duty_cycle_i)) and (run_i = '1') else '0';
end architecture;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.math_real.all;


entity led_controller is
    generic(M : natural := 3);
        port(clk_i : in std_logic;
            reset_i : in std_logic;
            run_i : in std_logic;
            frec_i: in std_logic_vector (M-1 downto 0);
            duty_cycle_i : in std_logic_vector (M-1 downto 0);
            out_o : out std_logic
            );
end entity;

architecture structural of led_controller is
    signal cycle_0 : std_logic_vector(M-1 downto 0):= "001";
    signal cycle_1 : std_logic_vector(M-1 downto 0):= (others => '1');
    signal count2 : std_logic;

begin
    CONT1: entity work.mod_m_counter_prog
    generic map(M => M)
    port map (clk_i => clk_i,
            reset_i => reset_i,
            run_i => run_i,
            frec_i => frec_i,
            duty_cycle_i=> cycle_0,
            out_o => count2
            );

    CONT2: entity work.mod_m_counter_prog
    generic map(M => M)
    port map (clk_i => clk_i,
            reset_i => reset_i,
            run_i => count2, -- creo que necesito la negacion.
            frec_i => cycle_1,
            duty_cycle_i => duty_cycle_i,
            out_o => out_o
            );
end architecture;

