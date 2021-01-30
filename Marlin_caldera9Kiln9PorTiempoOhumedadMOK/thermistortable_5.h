

/** Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

// Termistor de calentador de agua

const short temptable_5[][2] PROGMEM = {
{ OV( 520), 117},
{ OV( 600), 107},  
{ OV( 677), 97},  
{ OV( 713), 92},  
{ OV( 749), 87},  
{ OV( 778), 82},  
{ OV( 809), 77},  
{ OV( 838), 72},  
{ OV( 862), 67},  
{ OV( 884), 62},  
{ OV( 905), 57},  
{ OV( 925), 52},  
{ OV( 940), 47},  
{ OV( 956), 41},  
{ OV( 968), 36},  
{ OV( 978), 31},
{ OV( 992), 25},
{ OV(1003), 19},
{ OV(1010), 5},
{ OV(1020), 0}
};
