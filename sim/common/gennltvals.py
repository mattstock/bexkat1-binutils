#!/usr/bin/env python3
# Copyright (C) 1996-2021 Free Software Foundation, Inc.
#
# This file is part of the GNU simulators.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Helper to generate nltvals.def.

nltvals.def is a file that describes various newlib/libgloss target values used
by the host/target interface.  This needs to be rerun whenever the newlib source
changes.  Developers manually run it.

If the path to newlib is not specified, it will be searched for in:
- the root of this source tree
- alongside this source tree
"""

import argparse
from pathlib import Path
import re
import subprocess
import sys
from typing import Iterable, List, TextIO


PROG = Path(__file__).name

# Unfortunately, many newlib/libgloss ports have seen fit to define their own
# syscall.h file.  This means that system call numbers can vary for each port.
# Support for all this crud is kept here, rather than trying to get too fancy.
# If you want to try to improve this, please do, but don't break anything.
#
# If a target isn't listed here, it gets the standard syscall.h file (see
# libgloss/syscall.h) which hopefully new targets will use.
#
# NB: New ports should use libgloss, not newlib.
TARGET_DIRS = {
    'cr16': 'libgloss/cr16/sys',
    'd10v': 'newlib/libc/sys/d10v/sys',
    'i960': 'libgloss/i960',
    'mcore': 'libgloss/mcore',
    'riscv': 'libgloss/riscv/machine',
    'sh': 'newlib/libc/sys/sh/sys',
    'v850': 'libgloss/v850/sys',
}


# The header for the generated def file.
FILE_HEADER = f"""\
/* Newlib/libgloss macro values needed by remote target support.  */
/* This file is machine generated by {PROG}.  */\
"""


def gentvals(output: TextIO, cpp: str, srctype: str, srcdir: Path,
             headers: Iterable[str],
             pattern: str,
             target: str = None):
    """Extract constants from the specified files using a regular expression.

    We'll run things through the preprocessor.
    """
    headers = tuple(headers)

    # Require all files exist in order to regenerate properly.
    for header in headers:
        fullpath = srcdir / header
        assert fullpath.exists(), f'{fullpath} does not exist'

    if target is not None:
        print(f'#ifdef NL_TARGET_{target}', file=output)
    print(f'#ifdef {srctype}_defs', file=output)

    print('\n'.join(f'/* from {x} */' for x in headers), file=output)

    if target is None:
        print(f'/* begin {srctype} target macros */', file=output)
    else:
        print(f'/* begin {target} {srctype} target macros */', file=output)

    # Extract all the symbols.
    srcfile = ''.join(f'#include <{x}>\n' for x in headers)
    syms = set()
    define_pattern = re.compile(r'^#\s*define\s+(' + pattern + ')')
    for header in headers:
        with open(srcdir / header, 'r', encoding='utf-8') as fp:
            data = fp.read()
        for line in data.splitlines():
            m = define_pattern.match(line)
            if m:
                syms.add(m.group(1))
    for sym in sorted(syms):
        srcfile += f'#ifdef {sym}\nDEFVAL {{ "{sym}", {sym} }},\n#endif\n'

    result = subprocess.run(
        f'{cpp} -E -I"{srcdir}" -', shell=True, check=True, encoding='utf-8',
        input=srcfile, capture_output=True)
    for line in result.stdout.splitlines():
        if line.startswith('DEFVAL '):
            print(line[6:].rstrip(), file=output)

    print(f'#undef {srctype}_defs', file=output)
    if target is None:
        print(f'/* end {srctype} target macros */', file=output)
    else:
        print(f'/* end {target} {srctype} target macros */', file=output)
        print('#endif', file=output)
    print('#endif', file=output)


def gen_common(output: TextIO, newlib: Path, cpp: str):
    """Generate the common C library constants.

    No arch should override these.
    """
    gentvals(output, cpp, 'errno', newlib / 'newlib/libc/include',
             ('errno.h', 'sys/errno.h'), 'E[A-Z0-9]*')

    gentvals(output, cpp, 'signal', newlib / 'newlib/libc/include',
             ('signal.h', 'sys/signal.h'), r'SIG[A-Z0-9]*')

    gentvals(output, cpp, 'open', newlib / 'newlib/libc/include',
             ('fcntl.h', 'sys/fcntl.h', 'sys/_default_fcntl.h'), r'O_[A-Z0-9]*')


def gen_targets(output: TextIO, newlib: Path, cpp: str):
    """Generate the target-specific lists."""
    for target, subdir in sorted(TARGET_DIRS.items()):
        gentvals(output, cpp, 'sys', newlib / subdir, ('syscall.h',),
                 r'SYS_[_a-zA-Z0-9]*', target=target)

    # Then output the common syscall targets.
    gentvals(output, cpp, 'sys', newlib / 'libgloss', ('syscall.h',),
             r'SYS_[_a-zA-Z0-9]*')


def gen(output: TextIO, newlib: Path, cpp: str):
    """Generate all the things!"""
    print(FILE_HEADER, file=output)
    gen_common(output, newlib, cpp)
    gen_targets(output, newlib, cpp)


def get_parser() -> argparse.ArgumentParser:
    """Get CLI parser."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        '-o', '--output', type=Path,
        help='write to the specified directory')
    parser.add_argument(
        '--cpp', type=str, default='cpp',
        help='the preprocessor to use')
    parser.add_argument(
        '--srcroot', type=Path,
        help='the root of this source tree')
    parser.add_argument(
        'newlib', nargs='?', type=Path,
        help='path to the newlib+libgloss source tree')
    return parser


def parse_args(argv: List[str]) -> argparse.Namespace:
    """Process the command line & default options."""
    parser = get_parser()
    opts = parser.parse_args(argv)

    if opts.output is None:
        # Default to where the script lives.
        opts.output = Path(__file__).resolve().parent

    if opts.srcroot is None:
        opts.srcroot = Path(__file__).resolve().parent.parent.parent
    else:
        opts.srcroot = opts.srcroot.resolve()

    if opts.newlib is None:
        # Try to find newlib relative to our source tree.
        if (opts.srcroot / 'newlib').is_dir():
            # If newlib is manually in the same source tree, use it.
            if (opts.srcroot / 'libgloss').is_dir():
                opts.newlib = opts.srcroot
            else:
                opts.newlib = opts.srcroot / 'newlib'
        elif (opts.srcroot.parent / 'newlib').is_dir():
            # Or see if it's alongside the gdb/binutils repo.
            opts.newlib = opts.srcroot.parent / 'newlib'
    if opts.newlib is None or not opts.newlib.is_dir():
        parser.error('unable to find newlib')

    return opts


def main(argv: List[str]) -> int:
    """The main entry point for scripts."""
    opts = parse_args(argv)

    output = (opts.output / 'nltvals.def').open('w', encoding='utf-8')

    gen(output, opts.newlib, opts.cpp)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
