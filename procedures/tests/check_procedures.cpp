// Checks of decision rules and counts by origin (Proposition 6 as corrected): an origin weighs one whatever its voters; copies of
// an origin whose voters vote alike change nothing; copies added to a divided origin move the share by at most 1/O; under a count
// by operative seven identities against two hold 7/9; undeclared control makes copies origins; the rules and their sensitivity.

#include <thesmos/procedures/procedures.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>

using namespace thesmos;
using kernel::Kind;
using kernel::Parties;
using procedures::Rational;
using procedures::Unit;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    Parties ps;
    ps.add({"x", Kind::organisation});
    ps.add({"y", Kind::person});
    ps.add({"z", Kind::person});
    bool same = true, seven = false;
    for (int copies : {1, 2, 7, 50}) {
      Parties p = ps;
      std::map<std::string, bool> votes{{"y", false}, {"z", false}};
      for (int i = 0; i < copies; i++) {
        p.add({"x/" + std::to_string(i), Kind::operative, "x"});
        votes["x/" + std::to_string(i)] = true;
      }
      same = same && procedures::share(p, votes, Unit::origin) == Rational(1, 3);
      if (copies == 7) seven = procedures::share(p, votes, Unit::operative) == Rational(7, 9);
    }
    s.require("an origin whose voters vote alike weighs one however many operatives it runs: the share by origin stays 1/3", same);
    s.require("seven identities of one operator against two give 7/9 by operative (Political Authority, Equation 8)", seven);
    Parties hidden;
    hidden.add({"y", Kind::person});
    hidden.add({"z", Kind::person});
    std::map<std::string, bool> votes{{"y", false}, {"z", false}};
    for (int i = 0; i < 7; i++) {
      hidden.add({"u" + std::to_string(i), Kind::operative});
      votes["u" + std::to_string(i)] = true;
    }
    s.require("with control undeclared the seven count as seven origins: the count rests on the declaration", procedures::share(hidden, votes, Unit::origin) == Rational(7, 9));
  }
  {
    // The reviewer's counterexample, and the bound that replaces the false statement.
    Parties ps;
    ps.add({"x", Kind::organisation});
    ps.add({"y", Kind::person});
    ps.add({"x/0", Kind::operative, "x"});
    ps.add({"x/1", Kind::operative, "x"});
    std::map<std::string, bool> votes{{"y", true}, {"x/0", true}, {"x/1", false}};
    const Rational before = procedures::share(ps, votes, Unit::origin);
    ps.add({"x/2", Kind::operative, "x"});
    votes["x/2"] = true;
    const Rational after = procedures::share(ps, votes, Unit::origin);
    s.require("a divided origin that adds a voter moves the share (3/4 to 5/6): the corrected Proposition 6 bounds the move by 1/O",
              before == Rational(3, 4) && after == Rational(5, 6) && !(after - before > Rational(1, 2)));
    calx::random::Stream r(17, 5, 0, 0);
    int over = 0;
    for (int trial = 0; trial < 500; trial++) {
      Parties p;
      std::map<std::string, bool> v;
      const long long origins = between(r, 2, 8);
      for (long long o = 0; o < origins; o++) {
        const std::string id = "o" + std::to_string(o);
        p.add({id, Kind::organisation});
        for (long long k = 0, n = between(r, 1, 5); k < n; k++) {
          const std::string a = id + "/" + std::to_string(k);
          p.add({a, Kind::operative, id});
          v[a] = between(r, 0, 1);
        }
      }
      const Rational base = procedures::share(p, v, Unit::origin);
      const std::string target = "o" + std::to_string(between(r, 0, origins - 1));
      for (long long k = 0, n = between(r, 1, 20); k < n; k++) {
        const std::string a = target + "/extra" + std::to_string(k);
        p.add({a, Kind::operative, target});
        v[a] = between(r, 0, 1);
      }
      Rational move = procedures::share(p, v, Unit::origin) - base;
      if (move < Rational(0)) move = -move;
      over += move > Rational(1) / Rational(origins);
    }
    s.require("copies added to one origin never move the share by more than 1/O (500 random electorates)", over == 0);
  }
  {
    Parties ps;
    ps.add({"a", Kind::person});
    ps.add({"b", Kind::person});
    ps.add({"lab", Kind::organisation});
    ps.add({"lab/1", Kind::operative, "lab"});
    ps.add({"commons", Kind::community});
    ps.add({"commons/envoy", Kind::operative, "commons"});
    const std::map<std::string, bool> votes{{"a", true}, {"b", false}, {"lab/1", true}, {"commons/envoy", true}};
    s.require("by person only persons' origins count (1/2); by community only communities' (1); by origin all four origins (3/4)",
              procedures::share(ps, votes, Unit::person) == Rational(1, 2) && procedures::share(ps, votes, Unit::community) == Rational(1) &&
                  procedures::share(ps, votes, Unit::origin) == Rational(3, 4));
    const auto c = procedures::count(ps, votes, Unit::origin);
    s.require("majority and two thirds pass at 3/4, unanimity and consensus fail on one objection, any one passes",
              procedures::passes(c, "majority") && procedures::passes(c, "two thirds") && !procedures::passes(c, "unanimity") &&
                  !procedures::passes(c, "consensus as the absence of a formal objection") && procedures::passes(c, "any one"));
    const std::map<std::string, bool> split{{"a", false}, {"b", false}, {"lab/1", true}};
    Parties copies = ps;
    std::map<std::string, bool> many = split;
    for (int i = 2; i < 6; i++) {
      copies.add({"lab/" + std::to_string(i), Kind::operative, "lab"});
      many["lab/" + std::to_string(i)] = true;
    }
    const auto sens = procedures::sensitivity(copies, many, Unit::origin, "majority");
    s.require("a decision that turns on a declared relation of control is reported as turning (1/3 declared, 5/7 undeclared)",
              sens.turns() && !sens.passes_declared && sens.passes_undeclared && sens.declared.share() == Rational(1, 3) && sens.undeclared.share() == Rational(5, 7));
  }
  return s.done();
}
