#!/usr/bin/env bash

# redirect file args to stdin
cat $* | \

sed -E '
# replace multi-line comments that span a single line
s%/\*.*\*/%%g

# remove trailing whitespace
s/[[:space:]]+$//

# move trailing /* to the beginning of the next line
s%([^[:space:]]]?)[[:space:]]*/\*$%\1\n/*%g

# replace any newly formed single-line comments
t nextline
b end
:nextline
n
s%/\*\n%/*%g
s%/\*.*\*/%%g
:end
' | \

awk -- "$(cat <<EOF
BEGIN {
    # LIMIT is number of lines above which errors are reported (config in env)
    LIMIT = ENVIRON["LIMIT"]
    if (!LIMIT) LIMIT = 30
    indent = 0
    failed = 0
}

# count non-empty lines
/./ { block_lines++ }

# don't count single-line comments without preceding code
/^[ \t]*\/\// { block_lines-- }

# don't count multi-line comments (check if first and last line contain code)
/\/\*/  { comment_start = NR }
/\*\/./ { comment_start++ }
/\*\//  { block_lines -= NR - comment_start + 1 }

# count lines within top-level braces
/\{/ && indent++ == 0 {
    block_start = NR
    block_lines = 0
}
/\}/ && --indent == 0 && --block_lines > LIMIT {
    print block_lines "-line block at line " block_start
    failed = 1
}

# exit with status 1 if any blocks where bigger than the limit
END { exit failed }
EOF
)"
