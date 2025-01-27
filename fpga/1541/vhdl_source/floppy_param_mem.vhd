-------------------------------------------------------------------------------
--
-- (C) COPYRIGHT 2006, Gideon's Logic Architectures
--
-------------------------------------------------------------------------------
-- Title      : Floppy Parameter memory
-------------------------------------------------------------------------------
-- File       : floppy.vhd
-- Author     : Gideon Zweijtzer  <gideon.zweijtzer@gmail.com>
-------------------------------------------------------------------------------
-- Description: This module implements the emulator of the floppy drive.
-------------------------------------------------------------------------------
 
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

library unisim;
    use unisim.vcomponents.all;

library work;    
    use work.endianness_pkg.all;
    use work.io_bus_pkg.all;
    
entity floppy_param_mem is
generic (
    g_big_endian    : boolean );
port (
    clock       : in  std_logic;
    reset       : in  std_logic;
    
    io_req      : in  t_io_req;
    io_resp     : out t_io_resp;

    track       : in  unsigned(6 downto 0);
    side        : in  std_logic := '0';
    bit_time    : out unsigned(9 downto 0);
    track_start : out std_logic_vector(25 downto 0);
    max_offset  : out std_logic_vector(13 downto 0) );

end floppy_param_mem;

architecture gideon of floppy_param_mem is
    signal toggle     : std_logic;
    signal param_addr : std_logic_vector(8 downto 0);
    signal param_data : std_logic_vector(31 downto 0);
    signal ram_data   : std_logic_vector(31 downto 0);

    signal cpu_ram_en   : std_logic;
    signal cpu_ram_en_d : std_logic;
    signal cpu_rdata    : std_logic_vector(7 downto 0);
begin
    cpu_ram_en   <= io_req.read or io_req.write;
    cpu_ram_en_d <= cpu_ram_en when rising_edge(clock); 
    io_resp.ack  <= cpu_ram_en_d;
    io_resp.data <= cpu_rdata when cpu_ram_en_d = '1' else X"00"; 
    
    ram: RAMB16_S9_S36
    port map (
		CLKA  => clock,
		SSRA  => reset,
		ENA   => cpu_ram_en,
		WEA   => io_req.write,
        ADDRA => std_logic_vector(io_req.address(10 downto 0)),
		DIA   => io_req.data,
		DIPA  => "0",
		DOA   => cpu_rdata,
		DOPA  => open,
    
		CLKB  => clock,
		SSRB  => reset,
		ENB   => '1',
		WEB   => '0',
        ADDRB => param_addr,
		DIB   => X"00000000",
		DIPB  => X"0",
		DOB   => ram_data,
		DOPB  => open );

    param_addr <= side & std_logic_vector(track) & toggle;
    param_data <= byte_swap(ram_data, g_big_endian);

    process(clock)
    begin
        if rising_edge(clock) then
            if toggle='1' then -- even addresses (one clock later)
                track_start <= param_data(track_start'range);
            else
                max_offset  <= param_data(max_offset'range);
                bit_time    <= unsigned(param_data(bit_time'high+16 downto 16));
            end if;
            if reset='1' then
                toggle <= '0';
            else
                toggle <= not toggle;
            end if;
        end if;
    end process;
end gideon;
