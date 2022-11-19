
xterm_colors = []

# This is ripped out of pygments
# I leave out the 16 standard colors since people tend to re-define them to their liking.

# colors 16..232: the 6x6x6 color cube
_valuerange = (0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff)
for i in range(217):
	r = _valuerange[(i // 36) % 6]
	g = _valuerange[(i // 6) % 6]
	b = _valuerange[i % 6]
	xterm_colors.append((r, g, b))

# colors 233..253: grayscale
for i in range(1, 24):
	v = 8 + i * 10
	xterm_colors.append((v, v, v))

print('/* GENERATED HEADER FILE DO NOT EDIT */')
print('union rgb_c xterm256lut[256-16] = {')
for r,g,b in xterm_colors:
    print(f'    {{{{0x{b:02x}, 0x{g:02x}, 0x{r:02x}}}}},');
print('};');
