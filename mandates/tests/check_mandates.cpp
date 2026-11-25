// Checks of origins and mandates: chains that only narrow (400 random chains), envelopes charged along the chain (200 random
// trees) with the counterexample of independent ceilings, the exposure of a late revocation (2,000 random cases), and the faults
// of a declared relation of control.

#include <thesmos/mandates/mandates.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace thesmos;
using kernel::Kind;
using kernel::Parties;
using mandates::Mandate;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }

std::set<std::string> narrowed(calx::random::Stream& r, const std::set<std::string>& s) {
  std::vector<std::string> v(s.begin(), s.end());
  if (v.size() > 1 && between(r, 0, 1)) v.erase(v.begin() + between(r, 0, static_cast<long long>(v.size()) - 1));
  return {v.begin(), v.end()};
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Origins ----
  {
    Parties ps;
    ps.add({"lab", Kind::organisation});
    ps.add({"lab/agent", Kind::operative, "lab", "provider/model"});
    ps.add({"lab/agent/helper", Kind::operative, "lab/agent"});
    ps.add({"stray", Kind::operative});
    s.require("the origin of an operative is the first party along control that is not an operative",
              kernel::origin(ps, "lab/agent/helper") == "lab" && kernel::origin(ps, "lab") == "lab");
    s.require("an operative whose controller is undeclared is its own origin, by declaration", kernel::origin(ps, "stray") == "stray");
    s.require("under provision the origin is the declared provider", kernel::origin(ps, "lab/agent", kernel::Criterion::provision) == "provider/model");
    s.require("a declared relation of control without faults has none reported", mandates::control_faults(ps).empty());
    ps.add({"a", Kind::operative, "b"});
    ps.add({"b", Kind::operative, "a"});
    ps.add({"c", Kind::operative, "nobody"});
    const auto faults = mandates::control_faults(ps);
    s.require("operatives that control one another, and an unknown controller, are faults of the declaration", faults.size() == 3);
  }

  // ---- 1. Chains that only narrow ----
  {
    calx::random::Stream r(11, 1, 0, 0);
    const std::set<std::string> kinds = {"say", "offer", "deliver", "commit", "vote", "assess"};
    const std::set<std::string> matters = {"catalogue", "glossary", "review", "accounts"};
    const std::vector<std::string> faults_kind = {"kinds", "matters", "allowance", "term", "undelegable", "issuer"};
    int standing = 0, refused = 0, wrong = 0;
    for (int trial = 0; trial < 400; trial++) {
      Parties ps;
      ps.add({"p", Kind::person});
      mandates::Registry reg;
      std::string holder = "p";
      std::set<std::string> k = kinds, m = matters;
      long long ceiling = 1000, from = 0, to = 100;
      const long long depth = between(r, 1, 5);
      const long long fault_at = between(r, 0, depth + 1) - 2;  // -2, -1: no fault
      const std::string fault = faults_kind[between(r, 0, 5)];
      for (long long level = 0; level < depth; level++) {
        const std::string agent = "a" + std::to_string(level);
        ps.add({agent, Kind::operative, "p"});
        Mandate x;
        x.id = "m" + std::to_string(level);
        x.principal = holder;
        x.holder = agent;
        x.kinds = narrowed(r, k);
        x.matters = narrowed(r, m);
        x.allowance["tokens"] = ceiling - between(r, 0, 100);
        x.from = from + between(r, 0, 3);
        x.to = to - between(r, 0, 3);
        x.delegable = true;
        x.parent = level ? "m" + std::to_string(level - 1) : "";
        if (fault_at == level && level > 0) {
          if (fault == "kinds") x.kinds.insert("amend");
          else if (fault == "matters") x.matters.insert("treasury");
          else if (fault == "allowance") x.allowance["tokens"] = ceiling + 1;
          else if (fault == "term") x.from = from - 1;
          else if (fault == "issuer") x.principal = "p";
          else reg["m" + std::to_string(level - 1)].delegable = false;
        }
        reg[x.id] = x;
        holder = agent;
        k = x.kinds, m = x.matters, ceiling = x.allowance["tokens"], from = x.from, to = x.to;
      }
      const std::string leaf_id = "m" + std::to_string(depth - 1);
      const auto f = mandates::faults(ps, reg, leaf_id);
      if (fault_at > 0) {
        refused++;
        wrong += f.empty();
      } else {
        standing++;
        const Mandate& leaf = reg[leaf_id];
        bool inside = f.empty();
        for (const Mandate* link : mandates::chain(reg, leaf_id))
          inside = inside && std::includes(link->kinds.begin(), link->kinds.end(), leaf.kinds.begin(), leaf.kinds.end()) &&
                   std::includes(link->matters.begin(), link->matters.end(), leaf.matters.begin(), leaf.matters.end()) &&
                   leaf.allowance.at("tokens") <= link->allowance.at("tokens") && link->from <= leaf.from && leaf.to <= link->to;
        wrong += !inside;
      }
    }
    s.require("a chain with a widening or misissued link carries no authority, and a standing chain gives its holder no more than any link above it (" +
                  std::to_string(standing) + " standing, " + std::to_string(refused) + " refused)",
              wrong == 0 && standing > 100 && refused > 50);
    Parties ps;
    ps.add({"x", Kind::operative});
    ps.add({"y", Kind::operative, "x"});
    mandates::Registry rootless{{"m", Mandate{"m", "x", "y", {"say"}, {"a"}, {}, 0, 9}}};
    const auto f = mandates::faults(ps, rootless, "m");
    s.require("a chain whose root principal is an operative is refused: nobody answers", !f.empty() && f.front().find("nobody answers") != std::string::npos);
    ps.add({"p", Kind::person});
    mandates::Registry one{{"m", Mandate{"m", "p", "y", {"say"}, {}, {}, 0, 9}}};
    s.require("a revoked link defeats the chain", !mandates::faults(ps, one, "m", {"m"}).empty() && mandates::faults(ps, one, "m").empty());
  }

  // ---- 2. Envelopes ----
  {
    calx::random::Stream r(5, 2, 0, 0);
    int bad = 0;
    for (int trial = 0; trial < 200; trial++) {
      mandates::Registry reg;
      Mandate root{"root", "p", "a0", {}, {}, {{"tokens", 100}}, 0, 9, true};
      reg["root"] = root;
      std::vector<std::string> names{"root"};
      const long long n = between(r, 1, 7);
      for (long long i = 0; i < n; i++) {
        const std::string parent = names[between(r, 0, static_cast<long long>(names.size()) - 1)];
        Mandate x{"m" + std::to_string(i), "x", "y", {}, {}, {{"tokens", between(r, 10, reg[parent].allowance["tokens"])}}, 0, 9, true, parent};
        reg[x.id] = x;
        names.push_back(x.id);
      }
      mandates::Envelopes env(reg);
      long long admitted = 0;
      for (int k = 0; k < 60; k++) {
        const std::string name = names[between(r, 0, static_cast<long long>(names.size()) - 1)];
        const long long amount = between(r, 1, 30);
        const auto before = env.all();
        if (env.debit(name, "tokens", amount)) admitted += amount;
        else bad += env.all() != before;
      }
      for (const auto& name : names) bad += env.used(name, "tokens") > reg[name].allowance["tokens"];
      bad += env.used("root", "tokens") != admitted;
    }
    s.require("use under every mandate stays within its ceiling, the root's use is the sum admitted, and a refused charge changes nothing (200 random trees)", bad == 0);
    mandates::Registry two{{"root", Mandate{"root", "p", "a", {}, {}, {{"tokens", 100}}, 0, 9, true}},
                           {"left", Mandate{"left", "a", "b", {}, {}, {{"tokens", 80}}, 0, 9, false, "root"}},
                           {"right", Mandate{"right", "a", "c", {}, {}, {{"tokens", 80}}, 0, 9, false, "root"}}};
    mandates::Envelopes apart(two, false), shared(two);
    long long spent_apart = 0, spent_shared = 0;
    for (const char* name : {"left", "right"}) {
      spent_apart += apart.debit(name, "tokens", 80) ? 80 : 0;
      spent_shared += shared.debit(name, "tokens", 80) ? 80 : 0;
    }
    s.require("independent ceilings of 80 under a root of 100 admit 160; charged along the chain they admit 80 (Proposition 2)", spent_apart == 160 && spent_shared == 80);
  }

  // ---- 3. Exposure after a revocation learnt late ----
  {
    calx::random::Stream r(7, 3, 0, 0);
    int over = 0;
    for (int trial = 0; trial < 2000; trial++) {
      const long long budget = between(r, 50, 2000), slice = between(r, 5, 200), cost = between(r, 1, 20);
      const long long window = between(r, 1, 30), revoked_at = between(r, 0, 200);
      over += mandates::exposure(budget, slice, cost, revoked_at, window) > std::min(budget, slice);
    }
    s.require("what a remote verifier admits after a revocation is at most one slice and at most the budget (Proposition 3; 2,000 random cases)", over == 0);
    const long long unsliced = mandates::exposure(1000, 0, 10, 5, 50), sliced = mandates::exposure(1000, 100, 10, 5, 50);
    s.require("charged 10 a turn, revoked at turn 5 and checked every 50 turns, a mandate loses 450 with no slice and 50 with a slice of 100",
              unsliced == 450 && sliced == 50, std::to_string(unsliced) + ", " + std::to_string(sliced));
  }
  return s.done();
}
