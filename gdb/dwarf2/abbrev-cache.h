/* DWARF abbrev table cache

   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GDB_DWARF2_ABBREV_CACHE_H
#define GDB_DWARF2_ABBREV_CACHE_H

#include "dwarf2/abbrev.h"
#include <unordered_set>
#include "gdbtypes.h"

class abbrev_cache
{
public:
  abbrev_cache ();
  DISABLE_COPY_AND_ASSIGN (abbrev_cache);

  abbrev_table *find (struct dwarf2_section_info *section, sect_offset offset)
  {
    search_key key = { section, offset };

    return (abbrev_table *) htab_find_with_hash (m_tables.get (), &key,
						 to_underlying (offset));
  }

  void add (abbrev_table_up table);

private:

  static hashval_t hash_table (const void *item);
  static int eq_table (const void *lhs, const void *rhs);

  struct search_key
  {
    struct dwarf2_section_info *section;
    sect_offset offset;
  };

  /* Hash table of abbrev tables.  */
  htab_up m_tables;
};

#endif /* GDB_DWARF2_ABBREV_CACHE_H */
