import os
import re
import subprocess

# Path to QMK keymap used for firmware size calculations
MY_KEYMAP_PATH = os.getenv('MY_KEYMAP_PATH')
README_PATH = os.path.join(MY_KEYMAP_PATH, 'qmk-vim/README.org')

# Split readme into sections: before feature macros table, table itself, and after table
BEFORE, MACROS, AFTER = 0, 1, 2
lines = [[], [], []]
position = BEFORE
with open(README_PATH) as f:
    for line in f:
        if ((position == BEFORE and line.startswith('| ='))
                or (position == MACROS and not line.startswith('|'))):
            position += 1
        lines[position].append(line)

# Compile and parse output for firmware size
def get_firmware_size():
    process = subprocess.run(
        ['qmk', 'compile', '-kb', 'uno', '-km', 'qmk-vim-update-readme'], capture_output=True)
    matches = re.search(r'The firmware size is fine - (\d+)', process.stdout.decode('utf-8'))
    return int(matches[1])

# Determine firmware size without any macros defined
baseline = get_firmware_size()

# Iterate over rows in table
for i in range(len(lines[MACROS])):
    # Parse macro name
    line = lines[MACROS][i]
    cells = line.split('|')
    macro = cells[1].strip('= ')
    # Add macro to config
    with open(os.path.join(MY_KEYMAP_PATH, 'config.h'), 'w') as f:
        f.write(f'#define {macro}')
    # Determine firmware size difference
    size = get_firmware_size()
    size_diff = baseline - size
    if size_diff >= 0:
        new_val_str = f' +{size_diff} B'
    else:
        new_val_str = f' {size_diff} B'
    # Update table
    cells[3] = new_val_str.ljust(len(cells[3]), ' ')
    lines[MACROS][i] = '|'.join(cells)

# Overwrite readme with new values
with open(README_PATH, 'w') as f:
    f.write(''.join(line for section in lines for line in section))
