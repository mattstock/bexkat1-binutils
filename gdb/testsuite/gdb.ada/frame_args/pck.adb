--  Copyright 2007-2023 Free Software Foundation, Inc.
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 3 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program.  If not, see <http://www.gnu.org/licenses/>.

package body Pck is

   procedure Break_Me is
   begin
      null;
   end Break_Me;

   procedure Call_Me (Int : Integer;
                      Flt : Float;
                      Bln : Boolean;
                      Ary : Arr;
                      Chr : Character;
                      Sad : System.Address;
                      Rec : Struct)
   is
   begin
      Break_Me;
   end Call_Me;

end Pck;

