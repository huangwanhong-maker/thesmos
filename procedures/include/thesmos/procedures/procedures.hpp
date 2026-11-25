// Procedures: decision rules with their unit of count, exact shares, and a decision's sensitivity to the partition of origins.
//
// A rule of aggregation declares its unit of count. By operative, each voter counts one. By origin, each origin (under the
// declared relation of control) weighs one, divided equally among its voters, so that an origin's weight in favour is the
// share of its voters in favour. By person, the same over origins that are persons, others left uncounted; by community, over
// origins that are communities (the Political Authority and Evaluative Authority papers' two-level weighting).
//
// Proposition 6, as corrected after the independent reading of 2026-11-25. (a) Every origin weighs one, whatever the number of
// its voters. (b) The share in favour is unchanged when an origin whose voters vote alike replaces them by any number of voters
// that vote as they did. (c) When an origin's voters divide, adding voters to it moves its own weight in favour within [0, 1]
// and so moves the share by at most 1/O, O the number of origins counted; under a count by operative an origin with n of N
// voters holds n/N. The count rests on the declared relation of control: undeclared, copies count as origins.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::procedures {

using kernel::Kind;
using kernel::Parties;
using morphe::arithmetic::Rational;

enum class Unit { person, origin, operative, community };

inline std::optional<Unit> unit_named(std::string_view w) {
  if (w == "person") return Unit::person;
  if (w == "origin") return Unit::origin;
  if (w == "operative") return Unit::operative;
  if (w == "community") return Unit::community;
  return std::nullopt;
}

struct Count {
  Rational favour;        // the weight in favour
  long long counted = 0;  // how many units were counted (voters by operative, origins otherwise)
  long long against = 0;  // how many counted units hold at least one vote against
  Rational share() const { return counted == 0 ? Rational(0) : favour / Rational(counted); }
};

// A ballot: who cast it, the party that answers for it (the root of the voter's mandate, or the voter), and its sense. By origin,
// a ballot counts for the origin of the party that answers for it, so that an envoy who votes under a community's mandate votes
// for the community.
struct Ballot {
  std::string voter, bearer;
  bool yes = false;
};

// The weight in favour among ballots under a unit of count.
inline Count count(const Parties& ps, const std::vector<Ballot>& ballots, Unit unit) {
  Count c;
  if (unit == Unit::operative) {
    for (const Ballot& b : ballots) {
      c.counted++;
      if (b.yes) c.favour = c.favour + Rational(1);
      else c.against++;
    }
    return c;
  }
  std::map<std::string, std::pair<long long, long long>> groups;  // origin -> (in favour, voters)
  for (const Ballot& b : ballots) {
    const bool yes = b.yes;
    const std::string o = kernel::origin(ps, b.bearer.empty() ? b.voter : b.bearer);
    const Kind k = ps.at(o).kind;
    if (unit == Unit::person && k != Kind::person) continue;
    if (unit == Unit::community && k != Kind::community) continue;
    auto& g = groups[o];
    g.first += yes ? 1 : 0;
    g.second += 1;
  }
  for (const auto& [o, g] : groups) {
    c.counted++;
    c.favour = c.favour + Rational(g.first) / Rational(g.second);
    if (g.first < g.second) c.against++;
  }
  return c;
}

// The same, for votes cast in each voter's own name (party -> in favour).
inline Count count(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit) {
  std::vector<Ballot> ballots;
  for (const auto& [voter, yes] : votes) ballots.push_back({voter, voter, yes});
  return count(ps, ballots, unit);
}

inline Rational share(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit) { return count(ps, votes, unit).share(); }

// The rules the kernel knows: any one; majority (more than half); two thirds (at least); unanimity; consensus as the absence of
// a formal objection (no counted unit against).
inline bool known_rule(std::string_view rule) {
  return rule == "any one" || rule == "majority" || rule == "two thirds" || rule == "unanimity" || rule.rfind("consensus", 0) == 0;
}

inline bool passes(const Count& c, std::string_view rule) {
  if (c.counted == 0) return false;
  const Rational s = c.share();
  if (rule == "any one") return s > Rational(0);
  if (rule == "majority") return s > Rational(1, 2);
  if (rule == "two thirds") return !(s < Rational(2, 3));
  if (rule == "unanimity") return s == Rational(1);
  if (rule.rfind("consensus", 0) == 0) return c.against == 0;
  return false;
}

// A decision read under the declared relation of control and with every control relation removed, so that a result which turns
// on an undeclared relation is seen to be arguable.
struct Sensitivity {
  Count declared, undeclared;
  bool passes_declared = false, passes_undeclared = false;
  bool turns() const { return passes_declared != passes_undeclared; }
};

inline Sensitivity sensitivity(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit, std::string_view rule) {
  Sensitivity s;
  s.declared = count(ps, votes, unit);
  Parties bare = ps;
  for (const auto& [id, p] : ps.all()) bare.at(id).controller.clear();
  s.undeclared = count(bare, votes, unit);
  s.passes_declared = passes(s.declared, rule);
  s.passes_undeclared = passes(s.undeclared, rule);
  return s;
}

}  // namespace thesmos::procedures
