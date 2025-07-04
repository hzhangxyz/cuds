/* jshint esversion:6 */

import {
    buffer_size,
    rule_t,
} from "../jsds/jsds.mjs";

buffer_size(1000);

// P -> Q, P |- Q
const mp = new rule_t(`
('P -> 'Q)
'P
----------
'Q
`);

// p -> (q -> p)
const axiom1 = new rule_t(`
------------------
('p -> ('q -> 'p))
`);

// (p -> (q -> r)) -> ((p -> q) -> (p -> r))
const axiom2 = new rule_t(`
--------------------------------------------------
(('p -> ('q -> 'r)) -> (('p -> 'q) -> ('p -> 'r)))
`);

// (!p -> !q) -> (q -> p)
const axiom3 = new rule_t(`
----------------------------------
(((! 'p) -> (! 'q)) -> ('q -> 'p))
`);

const premise = new rule_t(`(! (! X))`);
const target = new rule_t(`X`);

function array_equal(a, b) {
    if (a.length !== b.length) {
        return false;
    }
    for (let i = 0; i < a.length; i++) {
        if (a[i] !== b[i]) {
            return false;
        }
    }
    return true;
}

function rule_equal(a, b) {
    return array_equal(a.data(), b.data());
}

function rule_in_array(rule, array) {
    for (let i = 0; i < array.length; i++) {
        if (rule_equal(rule, array[i])) {
            return true;
        }
    }
    return false;
}

function rule_in_map(rule, map) {
    for (let [key, value] of map) {
        if (rule_equal(rule, key)) {
            return true;
        }
    }
    return false;
}

function main() {
    let rules = {};
    let facts = {};

    let cycle = -1;
    rules[mp.data()] = [mp, cycle];
    facts[axiom1.data()] = [axiom1, cycle];
    facts[axiom2.data()] = [axiom2, cycle];
    facts[axiom3.data()] = [axiom3, cycle];
    facts[premise.data()] = [premise, cycle];

    while (true) {
        let temp_rules = {};
        let temp_facts = {};

        for (let r_data in rules) {
            for (const f_data in facts) {
                const [rule, r_cycle] = rules[r_data];
                const [fact, f_cycle] = facts[f_data];
                if (r_cycle != cycle && f_cycle != cycle) {
                    continue;
                }
                const candidate = rule.match(fact);
                if (candidate === null) {
                    continue;
                }
                if (candidate.length() != 0) {
                    // rule
                    if (candidate.data() in rules || candidate.data() in temp_rules) {
                        continue;
                    }
                    temp_rules[candidate.data()] = candidate;
                } else {
                    // fact
                    if (candidate.data() in facts || candidate.data() in temp_facts) {
                        continue;
                    }
                    if (rule_equal(candidate, target)) {
                        console.log("Found!");
                        console.log(candidate.toString());
                        return;
                    }
                    temp_facts[candidate.data()] = candidate;
                }
            }
        }

        cycle++;
        for (let r_hash in temp_rules) {
            const rule = temp_rules[r_hash];
            rules[rule.data()] = [rule, cycle];
        }
        for (let f_hash in temp_facts) {
            const fact = temp_facts[f_hash];
            facts[fact.data()] = [fact, cycle];
        }
    }
}

for (let i = 0; i < 10; i++) {
    const begin = new Date();
    main();
    const end = new Date();
    console.log("Time taken: " + (end - begin) / 1000 + "s");
}
