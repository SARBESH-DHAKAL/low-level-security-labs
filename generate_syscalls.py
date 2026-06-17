#!/usr/bin/env python3

# Read syscalls.txt
with open('syscalls.txt', 'r') as f:
    lines = f.readlines()

print("struct syscall_entry syscall_table[] = {")

for line in lines:
    parts = line.strip().split()
    if len(parts) == 2:
        name = parts[0].replace('__NR_', '')
        number = parts[1]
        print(f'    {{{number}, "{name}"}},')

print('    {-1, NULL}')
print("};")
