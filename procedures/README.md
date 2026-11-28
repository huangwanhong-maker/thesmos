# procedures: Units of Count and Rules

Status: `draft` (decisions 0003 and 0013).

## 1. Concept

Shares in favour by person, origin, operative or community, exact in rationals; the rules any one, majority, two thirds, unanimity and consensus as the absence of a formal objection; a decision's sensitivity to the declared relation of control (Proposition 6 as corrected).

Since decision 0013 two more units and two more forms of rule. By provider, each provider (the voter's origin under provision: an operative's declared provider, any other party itself) weighs one, divided among its voters, so that a provider's agents carry one voice however many they are. By weight, each origin weighs its declared weight, an exact rational, divided among its voters, and an origin with none is not counted. A rule may be a threshold at an exact fraction (`at least 3/5`, `more than 1/2`) or a double majority of members and weight (`double majority of members at least 1/2 and weight at least 2/3`), passed when the origins counted by weight, each weighing one, and their weights both reach their thresholds. Every rule reads the ballots cast, and every rule is monotone (Proposition 33).

## 2. Content

`Unit`, `Weights`, `Count`, `count`, `share`, `Threshold`, `Rule`, `read_rule`, `text`, `double_majority`, `known_rule`, `passes`, `Tally`, `decide`, `Sensitivity`, `sensitivity`.

## 3. Checks

`tests/check_procedures.cpp`: copies of one origin; the counterexample of a divided origin and the bound 1/O on 500 random electorates; units and rules. `tests/check_weights.cpp` (decision 0013): rules read and written back; thresholds at exact fractions; the count by provider and Proposition 6 under provision on random electorates; the count by weight, equal weights against the count by origin; double majorities against a computation apart on 500 random electorates; monotone rules on 400; the sensitivity of a weighted decision.
