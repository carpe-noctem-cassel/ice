#script (python) 

import gringo

def main(prg):
    prg.ground("pigeon", [])
    for i in range(0,2):
        prg.assignExternal(gringo.Fun("p"), True)
        prg.solve()
        prg.assignExternal(gringo.Fun("p"), False)
        f = prg.asolve()
        f.wait(0.01)
        f.interrupt()

#end.

#const n = 190.

#program pigeon.
#external p.

1 { p(X); q(X) } 1 :- X = 1..n, not p.
:- not n+1 { p(1..n); q(1..n) }, not p.
