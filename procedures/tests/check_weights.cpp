// Checks of the units and rules of decision 0013: rules read from their text and written back; thresholds at exact fractions; a count by
// provider, each provider weighing one whatever the number of its agents (Proposition 6 under provision, on random electorates); a count
// by weight in exact shares of declared weights, equal weights giving the count by origin; a double majority held to a computation apart
// on 500 random electorates; every rule monotone on random electorates (Proposition 33); and the sensitivity of a weighted decision to
// the declared relation of control.

#include <thesmos/procedures/procedures.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace thesmos;
using kernel::Kind;
using kernel::Parties;
using procedures::Ballot;
using procedures::Rational;
using procedures::Unit;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }

// A random electorate: communities, persons and organisations as origins, each running 0 to 3 operatives on one of three providers,
// every operative and every origin casting a ballot or not.
struct Electorate {
  Parties ps;
  std::vector<Ballot> ballots;
  procedures::Weights weights;
};
Electorate electorate(calx::random::Stream& r) {
  Electorate e;
  const char* providers[] = {"lab-1", "lab-2", "lab-3"};
  const long long n = between(r, 2, 6);
  for (long long i = 0; i < n; i++) {
    const std::string o = "o" + std::to_string(i);
    const Kind k = i % 3 == 0 ? Kind::community : i % 3 == 1 ? Kind::person : Kind::organisation;
    e.ps.add({o, k});
    e.weights[o] = Rational(between(r, 0, 5), between(r, 1, 3));
    if (r.below(2)) e.ballots.push_back({o, o, r.below(2) == 0});
    const long long agents = between(r, 0, 3);
    for (long long j = 0; j < agents; j++) {
      const std::string a = o + "/a" + std::to_string(j);
      e.ps.add({a, Kind::operative, o, providers[r.below(3)]});
      e.ballots.push_back({a, o, r.below(2) == 0});
    }
  }
  return e;
}
// A double majority computed apart: the origins with a positive weight that cast a ballot, each origin's fraction in favour; the members'
// share is their mean, the weight's share their weighted mean.
bool double_majority_apart(const Electorate& e, const procedures::Rule& rule) {
  std::map<std::string, std::pair<long long, long long>> by;
  for (const auto& b : e.ballots) {
    const std::string o = kernel::origin(e.ps, b.bearer);
    if (!(Rational(0) < e.weights.at(o))) continue;
    by[o].first += b.yes, by[o].second += 1;
  }
  if (by.empty()) return false;
  Rational members, weight, total;
  for (const auto& [o, g] : by) {
    const Rational f = Rational(g.first) / Rational(g.second);
    members = members + f;
    weight = weight + e.weights.at(o) * f;
    total = total + e.weights.at(o);
  }
  members = members / Rational(static_cast<long long>(by.size()));
  weight = weight / total;
  return rule.members.met(members) && rule.weight.met(weight);
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. Rules read and written ----
  {
    bool back = true;
    for (const char* t : {"any one", "majority", "two thirds", "unanimity", "consensus as the absence of a formal objection", "at least 3/5", "more than 1/2", "at least 1",
                          "double majority of members at least 1/2 and weight at least 2/3", "double majority of members more than 1/2 and weight at least 13/20"}) {
      const auto r = procedures::read_rule(t);
      back = back && r && procedures::text(*r) == t && procedures::read_rule(procedures::text(*r)).has_value();
    }
    bool refused = true;
    for (const char* t : {"at least 3/2", "at least 1/0", "at least three fifths", "more than -1/2", "double majority of members 1/2", "double majority of members at least 1/2 and weight 2",
                          "two-thirds", "at least 3/5 or so"})
      refused = refused && !procedures::read_rule(t);
    const auto object = procedures::double_majority("1/2", "at least 2/3");
    s.require("rules are read from their text and written back to it: the five named rules, thresholds at least or more than a fraction, and double majorities; "
              "a fraction above one, a zero denominator, words for numbers and a double majority missing its weight are refused; the object form reads into the text",
              back && refused && object && *object == "double majority of members at least 1/2 and weight at least 2/3");
  }

  // ---- 2. Thresholds at exact fractions ----
  {
    Parties ps;
    for (int i = 0; i < 5; i++) ps.add({"v" + std::to_string(i), Kind::person});
    std::map<std::string, bool> three{{"v0", true}, {"v1", true}, {"v2", true}, {"v3", false}, {"v4", false}};
    const auto c = procedures::count(ps, three, Unit::person);
    std::map<std::string, bool> halves{{"v0", true}, {"v1", false}};
    const auto h = procedures::count(ps, halves, Unit::person);
    std::map<std::string, bool> two_of_three{{"v0", true}, {"v1", true}, {"v2", false}};
    const auto t = procedures::count(ps, two_of_three, Unit::person);
    s.require("thresholds at exact fractions: three of five reach at least 3/5 and not more than 3/5; one of two reaches at least 1/2 and is no majority; two of three reach two thirds",
              c.share() == Rational(3, 5) && procedures::passes(c, "at least 3/5") && !procedures::passes(c, "more than 3/5") && procedures::passes(h, "at least 1/2") &&
                  !procedures::passes(h, "majority") && procedures::passes(t, "two thirds") && !procedures::passes(t, "more than 2/3"));
  }

  // ---- 3. A count by provider ----
  {
    Parties ps;
    ps.add({"ana", Kind::person});
    ps.add({"bea", Kind::person});
    for (int i = 0; i < 3; i++) ps.add({"ana/" + std::to_string(i), Kind::operative, "ana", "lab-1"});
    ps.add({"bea/0", Kind::operative, "bea", "lab-2"});
    std::map<std::string, bool> votes{{"ana/0", true}, {"ana/1", true}, {"ana/2", true}, {"bea/0", false}, {"ana", false}};
    const auto by_provider = procedures::count(ps, votes, Unit::provider), by_operative = procedures::count(ps, votes, Unit::operative), by_origin = procedures::count(ps, votes, Unit::origin);
    s.require("by provider, three agents of one provider weigh one, beside another provider's agent and a person who is a provider of judgement of her own: 1/3 where by "
              "operative it is 3/5 and by origin (Ana's agents and Ana dividing) 3/8",
              by_provider.share() == Rational(1, 3) && by_provider.counted == 3 && by_operative.share() == Rational(3, 5) && by_origin.share() == Rational(3, 8),
              by_provider.share().str() + " " + by_origin.share().str());
    calx::random::Stream r(20261127, 13, 0, 0);
    bool alike = true, bounded = true;
    for (int trial = 0; trial < 300; trial++) {
      const Electorate e = electorate(r);
      const auto base = procedures::count(e.ps, e.ballots, Unit::provider);
      std::map<std::string, std::pair<long long, long long>> sense;  // provider -> (in favour, voters)
      for (const auto& b : e.ballots) {
        auto& g = sense[kernel::origin(e.ps, b.voter, kernel::Criterion::provision)];
        g.first += b.yes, g.second += 1;
      }
      for (const auto& [p, g] : sense) {
        if (p.rfind("lab-", 0) != 0) continue;  // a person or an organisation casting its own ballot is its own provider; only labs run more agents
        const bool unanimous = g.first == 0 || g.first == g.second;
        Electorate grown = e;  // the provider runs more agents, voting as its agents do (in favour if any of them is)
        for (long long j = 0, extra = between(r, 1, 4); j < extra; j++) {
          const std::string a = "copy-" + p + "-" + std::to_string(j);
          grown.ps.add({a, Kind::operative, "o0", p});
          grown.ballots.push_back({a, "o0", g.first > 0});
        }
        const auto after = procedures::count(grown.ps, grown.ballots, Unit::provider);
        if (unanimous) alike = alike && after.share() == base.share();
        else {
          const Rational diff = after.share() < base.share() ? base.share() - after.share() : after.share() - base.share();
          bounded = bounded && !(Rational(1, base.counted) < diff);
        }
      }
    }
    s.require("on 300 random electorates a provider whose agents vote alike keeps the share by provider unchanged when it runs any number more agents voting as they do, "
              "and a divided provider's added agents move the share by at most 1/P (Proposition 6 under provision)",
              alike && bounded);
  }

  // ---- 4. A count by weight ----
  {
    Parties ps;
    for (const char* c : {"north", "south", "east", "west"}) ps.add({c, Kind::community});
    for (int i = 0; i < 2; i++) ps.add({"east/envoy-" + std::to_string(i), Kind::operative, "east", "lab-1"});
    procedures::Weights w{{"north", Rational(3)}, {"south", Rational(2)}, {"east", Rational(1)}};
    std::vector<Ballot> ballots{{"north", "north", true}, {"south", "south", false}, {"east/envoy-0", "east", true}, {"east/envoy-1", "east", false}, {"west", "west", true}};
    const auto c = procedures::count(ps, ballots, Unit::weight, w);
    s.require("by weight, each origin weighs its declared weight divided among its voters, and an origin with none is not counted: (3 + 1/2) of 6, that is 7/12",
              c.share() == Rational(7, 12) && c.counted == 3 && c.total == Rational(6) && c.against == 2, c.share().str());
    calx::random::Stream r(20261127, 14, 0, 0);
    bool equal = true;
    for (int trial = 0; trial < 300; trial++) {
      Electorate e = electorate(r);
      procedures::Weights ones;
      for (const auto& [o, x] : e.weights) ones[o] = Rational(7);
      const auto a = procedures::count(e.ps, e.ballots, Unit::weight, ones), b = procedures::count(e.ps, e.ballots, Unit::origin);
      equal = equal && a.share() == b.share() && a.counted == b.counted;
    }
    s.require("on 300 random electorates equal weights give the count by origin exactly", equal);
  }

  // ---- 5. Double majorities ----
  {
    const auto rule = *procedures::read_rule("double majority of members at least 1/2 and weight at least 2/3");
    Parties ps;
    for (const char* c : {"a", "b", "c", "d"}) ps.add({c, Kind::community});
    procedures::Weights w{{"a", Rational(10)}, {"b", Rational(1)}, {"c", Rational(1)}, {"d", Rational(1)}};
    std::vector<Ballot> big{{"a", "a", true}, {"b", "b", false}, {"c", "c", false}, {"d", "d", false}};
    std::vector<Ballot> many{{"a", "a", false}, {"b", "b", true}, {"c", "c", true}, {"d", "d", true}};
    std::vector<Ballot> both{{"a", "a", true}, {"b", "b", true}, {"c", "c", false}, {"d", "d", false}};
    const auto t1 = procedures::decide(ps, big, Unit::weight, rule, w), t2 = procedures::decide(ps, many, Unit::weight, rule, w), t3 = procedures::decide(ps, both, Unit::weight, rule, w);
    s.require("a double majority: the largest member alone holds 10/13 of the weight and 1/4 of the members, and fails; the three others hold 3/4 of the members and 3/13 of "
              "the weight, and fail; the largest with one other hold 1/2 of the members and 11/13 of the weight, and pass",
              !t1.passed && t1.count.share() == Rational(10, 13) && t1.members.share() == Rational(1, 4) && !t2.passed && t2.members.share() == Rational(3, 4) && t3.passed);
    calx::random::Stream r(20261127, 15, 0, 0);
    bool agree = true;
    long long passed = 0;
    for (int trial = 0; trial < 500; trial++) {
      Electorate e = electorate(r);
      procedures::Rule q;
      q.form = procedures::Rule::Form::double_majority;
      q.members = {Rational(between(r, 1, 3), 4), r.below(2) == 0};
      q.weight = {Rational(between(r, 1, 5), 6), r.below(2) == 0};
      const bool mine = procedures::decide(e.ps, e.ballots, Unit::weight, q, e.weights).passed, apart = double_majority_apart(e, q);
      agree = agree && mine == apart;
      passed += mine;
    }
    s.require("on 500 random electorates a double majority decides as a computation apart from the counts does (" + std::to_string(passed) + " passed)", agree && passed > 20 && passed < 480);
  }

  // ---- 6. Monotonicity (Proposition 33) ----
  {
    calx::random::Stream r(20261127, 16, 0, 0);
    bool monotone = true;
    long long changed = 0;
    const std::vector<std::string> rules = {"any one", "majority", "two thirds", "unanimity", "at least 3/5", "more than 1/2", "double majority of members at least 1/2 and weight more than 1/2"};
    for (int trial = 0; trial < 400; trial++) {
      Electorate e = electorate(r);
      if (e.ballots.empty()) continue;
      const auto& text = rules[r.below(rules.size())];
      const auto rule = *procedures::read_rule(text);
      const Unit units[] = {Unit::person, Unit::origin, Unit::operative, Unit::community, Unit::provider, Unit::weight};
      const Unit unit = rule.form == procedures::Rule::Form::double_majority ? Unit::weight : units[r.below(6)];
      const bool before = procedures::decide(e.ps, e.ballots, unit, rule, e.weights).passed;
      for (auto& b : e.ballots)
        if (!b.yes) {
          b.yes = true;
          break;
        }
      const bool after = procedures::decide(e.ps, e.ballots, unit, rule, e.weights).passed;
      monotone = monotone && (!before || after);
      changed += before != after;
    }
    s.require("on 400 random electorates, under every unit and every rule but consensus, a ballot turned in favour never makes a passing decision fail (Proposition 33; " +
                  std::to_string(changed) + " decisions turned to pass)",
              monotone && changed > 0);
  }

  // ---- 7. Sensitivity of a weighted decision ----
  {
    Parties ps;
    ps.add({"big", Kind::community});
    ps.add({"small", Kind::community});
    for (int i = 0; i < 3; i++) ps.add({"agent-" + std::to_string(i), Kind::operative, "small", "lab-1"});
    procedures::Weights w{{"big", Rational(2)}, {"small", Rational(1)}, {"agent-0", Rational(1)}, {"agent-1", Rational(1)}, {"agent-2", Rational(1)}};
    std::vector<Ballot> ballots{{"big", "big", false}, {"agent-0", "agent-0", true}, {"agent-1", "agent-1", true}, {"agent-2", "agent-2", true}};
    const auto sens = procedures::sensitivity(ps, ballots, Unit::weight, "majority", w);
    s.require("a weighted decision that passes only when the small community's agents are taken for origins is seen to turn on the undeclared relation of control: "
              "declared, small's 1 of 3 fails; undeclared, the three agents' 3 of 5 passes",
              sens.turns() && !sens.passes_declared && sens.passes_undeclared && sens.declared.share() == Rational(1, 3) && sens.undeclared.share() == Rational(3, 5),
              sens.declared.share().str() + " " + sens.undeclared.share().str());
  }
  return s.done();
}
