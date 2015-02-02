# Goes through coordinates of body pieces and find where they are
# relative to the head.

head = ("head", 2440, 399, 2496, 456)

head_x = abs(2440 + 2496)/2
head_y = abs(399 + 456)/2

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
  # y - head_y is negated so that y is "more" for up and "less" for
  # down (instead of "less" for up [0,0 is usually point UP and left in
  # screens] and "more" for down). This will make the computed delta
  # more useful. This is because if y is ABOVE head_y, then the
  # result will end up being positive. If negation wasn't there,
  # it'd have been negative.
  print "  //%s\n  { Float2(%s.0f, %s.0f) }," % \
    (piece[0], x - head_x, -(y - head_y))
