import time
import pyds

pyds.buffer_size(1000)

# P -> Q, P |- Q
mp = pyds.Rule("""
('P -> 'Q)
'P
----------
'Q
""")

# p -> (q -> p)
axiom1 = pyds.Rule("""
------------------
('p -> ('q -> 'p))
""")

# (p -> (q -> r)) -> ((p -> q) -> (p -> r))
axiom2 = pyds.Rule("""
--------------------------------------------------
(('p -> ('q -> 'r)) -> (('p -> 'q) -> ('p -> 'r)))
""")

# (!p -> !q) -> (q -> p)
axiom3 = pyds.Rule("""
----------------------------------
(((! 'p) -> (! 'q)) -> ('q -> 'p))
""")

premise = pyds.Rule("(! (! X))")
target = pyds.Rule("X")


def main():
    rules: dict[pyds.Rule, int] = {}
    facts: dict[pyds.Rule, int] = {}

    cycle = -1
    rules[mp] = cycle
    facts[axiom1] = cycle
    facts[axiom2] = cycle
    facts[axiom3] = cycle
    facts[premise] = cycle

    while True:
        temp_rules: set[pyds.Rule] = set()
        temp_facts: set[pyds.Rule] = set()

        for rule, r_cycle in rules.items():
            for fact, f_cycle in facts.items():
                if r_cycle != cycle and f_cycle != cycle:
                    continue
                candidate = rule @ fact
                if candidate is None:
                    continue
                if len(candidate) != 0:
                    # Rule
                    if candidate in rules or candidate in temp_rules:
                        continue
                    temp_rules.add(candidate)
                else:
                    # Fact
                    if candidate in facts or candidate in temp_facts:
                        continue
                    if hash(candidate) == hash(target):
                        print("Found!")
                        print(candidate)
                        return
                    temp_facts.add(candidate)

        cycle += 1
        for rule in temp_rules:
            rules[rule] = cycle
        for fact in temp_facts:
            facts[fact] = cycle


for i in range(10):
    begin = time.time()
    main()
    end = time.time()
    print(end - begin)
