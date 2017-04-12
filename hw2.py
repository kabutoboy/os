from numpy import random
from numpy import mean

def belady():
    global refstr, nframes
    refstr = [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]
    print("Testing Belady's anomaly with ref str", refstr)
    print("Outputs nframes, nfaults")
    for nframes in range(1, 11):
        print(nframes, fifo())
def test():
    global refstr, nframes
    upbnd = 20
    reflen = 100
    resfifo = [None] * upbnd
    reslru = [None] * upbnd
    resopt = [None] * upbnd
    for nframes in range(1, upbnd+1):
        i = nframes - 1
        T = upbnd
        resfifo[i] = [0] * T
        reslru[i] = [0] * T
        resopt[i] = [0] * T
        for t in range(T):
            # print("Reference string is %s" % (refstr))
            # refstr = list(random.randint(upbnd, size=reflen))
            refstr = [sorted((0, int(x), upbnd-1))[1] for x in random.normal(loc=upbnd/2, scale=upbnd/5, size=reflen)]
            # print(refstr)
            resfifo[i][t] = fifo()
            reslru[i][t] = lru()
            resopt[i][t] = opt()
    print("Outputs nframes then nfaults for FIFO, LRU, OPT respectively")
    for nframes in range(1, upbnd+1):
        pass
        # print("num of frames is %d" % (nframes))
        i = nframes - 1
        a, b, c = mean(resfifo[i]), mean(reslru[i]), mean(resopt[i])
        print(nframes, a, b, c, 'lru/fifo', b/a*100)

def fifo():
    global refstr, nframes
    reflen = len(refstr)
    page = [-1] * nframes
    age = [0] * nframes
    nfaults = 0
    for i in range(reflen):
        age = [x+1 for x in age]
        r = refstr[i]
        if r not in page:
            nfaults += 1
            if -1 in page:
                k = page.index(-1)
            else:
                k = age.index(max(age))
            page[k] = r
            age[k] = 0
        # print('page', page, 'age', age)
    return nfaults

def lru():
    global refstr, nframes
    reflen = len(refstr)
    page = [-1] * nframes
    nfaults = 0
    for i in range(reflen):
        r = refstr[i]
        if r not in page:
            nfaults += 1
            if -1 in page:
                page[page.index(-1)] = r
            else:
                k = 0
                oldest = 0
                for j in range(nframes):
                    s = page[j]
                    age = list(reversed(refstr[:i])).index(s)
                    # print("%d age %d" % (s, age))
                    if age > oldest:
                        k, oldest = j, age
                        # print("this is oldest")
                page[k] = r
        # print(page)
    return nfaults

def opt():
    global refstr, nframes
    reflen = len(refstr)
    page = [-1] * nframes
    nfaults = 0
    for i in range(reflen):
        r = refstr[i]
        if r not in page:
            nfaults += 1
            if -1 in page:
                page[page.index(-1)] = r
            else:
                # print("page fault at iteration %d addr %d" % (i, r))
                k = 0
                longest = 0
                for j in range(nframes):
                    s = page[j]
                    try:
                        period = refstr[i:].index(s)
                    except ValueError:
                        period = 999
                    # print("finding %d from %i on, period is %d" % (s, i, period))
                    if period > longest:
                        # print("this is max")
                        k, longest = j, period
                page[k] = r
        # print(page)
        pass
    return nfaults

belady()
test()
