sen.setmode(1, 4)
r1 = 0
g1 = 0
b1 = 0

h = 0
s = 0
v = 0

r = 0
g = 0
b = 0
sen.setmode(4, 4)
while (true) {
    printupd()
    sen.read.rgb(4, r, g, b)
    print("r", r)
    print("g", g)
    print("b", b)
}