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
const target_hash = target.data_string();

function main() {
    let rules = {};
    let facts = {};

    let cycle = -1;
    rules[mp.data_string()] = [mp, cycle];
    facts[axiom1.data_string()] = [axiom1, cycle];
    facts[axiom2.data_string()] = [axiom2, cycle];
    facts[axiom3.data_string()] = [axiom3, cycle];
    facts[premise.data_string()] = [premise, cycle];

    while (true) {
        let temp_rules = {};
        let temp_facts = {};

        for (let r_hash in rules) {
            for (const f_hash in facts) {
                const [rule, r_cycle] = rules[r_hash];
                const [fact, f_cycle] = facts[f_hash];
                if (r_cycle != cycle && f_cycle != cycle) {
                    continue;
                }
                const candidate = rule.match(fact);
                if (candidate === null) {
                    continue;
                }
                const candidate_hash = candidate.data_string();
                if (candidate.length() != 0) {
                    // rule
                    if (candidate_hash in rules || candidate_hash in temp_rules) {
                        continue;
                    }
                    temp_rules[candidate_hash] = candidate;
                } else {
                    // fact
                    if (candidate_hash in facts || candidate_hash in temp_facts) {
                        continue;
                    }
                    if (candidate_hash == target_hash) {
                        console.log("Found!");
                        console.log(candidate.toString());
                        return;
                    }
                    temp_facts[candidate_hash] = candidate;
                }
            }
        }

        cycle++;
        for (let r_hash in temp_rules) {
            const rule = temp_rules[r_hash];
            rules[rule.data_string()] = [rule, cycle];
        }
        for (let f_hash in temp_facts) {
            const fact = temp_facts[f_hash];
            facts[fact.data_string()] = [fact, cycle];
        }
    }
}

for (let i = 0; i < 10; i++) {
    const begin = new Date();
    main();
    const end = new Date();
    console.log("Time taken: " + (end - begin) / 1000 + "s");
}
