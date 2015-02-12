# Goes through coordinates of body pieces and find where they are
# relative to a base (head for example).
#
# The coordinate values were manually found out by looking at the image
# in gimp. There was a fully "mounted" character at ~2400,~370 on the image
# (y goes up from top->bottom in these coordinates). The values were looked up
# by hand and written here.
#
# If you suspect this is wrong, check tatermand's art and manually look at the
# image. The absolute values are not really important.

base = ("head", 2440, 399, 2496, 456)

base_x = abs(base[1] + base[3])/2
base_y = abs(base[2] + base[4])/2

pieces = [
  ("head", 2440, 399, 2496, 456),
  ("shoulder left", 2392, 407, 2463, 483),
  ("shoulder right", 2482, 403, 2542, 485),
  ("torso", 2419, 402, 2520, 502),
  ("arm left", 2372, 379, 2449, 495),
  ("weapon", 2460, 306, 2542, 455),
  ("arm right", 2480, 339, 2537, 452)
]

for piece in pieces:
  x = piece[1] + piece[3]
  x = abs(x/2)
  y = piece[2] + piece[4]
  y = abs(y/2)
  # y - base_y is negated so that y is "more" for up and "less" for
  # down (instead of "less" for up [0,0 is usually point UP and left in
  # screens] and "more" for down). This will make the computed delta
  # more useful. This is because if y is ABOVE base_y, then the
  # result will end up being positive. If negation wasn't there,
  # it'd have been negative.
  print "  //%s\n  float2{%s.0f, %s.0f}," % \
    (piece[0], x - base_x, -(y - base_y))
