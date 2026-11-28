// Checks of counts and compacts in the fold (decision 0013): an assembly counted by provider, three agents of one provider weighing one;
// a double majority of members and weight deciding an amendment, refused for the largest member alone and admitted with a second; the
// faults of weights and rules; weights changed by amendment; a compact ratified by amendment with its terms and reservations, a reserved
// article closed to reservation refused, terms that name other parties refused, a second ratification refused, a reservation withdrawn,
// the community withdrawn after the compact's notice and ratifying again once it took effect, a compact that admits no withdrawal, an
// older ratification by name alone; three communities' records read as one compact, each party's withdrawal kept apart; on 500 random
// disclosure agreements the agreement in force exactly when the compact of its two sides is (Proposition 36); and the records replay.

#include <thesmos/chronicle/briefing.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;
namespace ch = thesmos::chronicle;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
symploke::jsonld::Json workshop() { return symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json")); }
constitutions::Constitution with(const std::string& decision, const std::string& written) {
  auto doc = workshop();
  doc.get("aggregation")->set(decision, symploke::jsonld::parse(written));
  return constitutions::read(symploke::jsonld::dump(doc));
}
// A proposal decided by `decision`, voted by `yes` and `no`, then amended at `turn` + 1 (the deliberative tempo).
std::vector<ch::Outcome> decide(State& s, const std::string& id, const std::string& decision, std::map<std::string, std::string> change, const std::vector<std::string>& yes,
                                const std::vector<std::string>& no, long long turn, const std::string& proposer = "sam") {
  std::vector<ch::Outcome> out;
  std::map<std::string, std::string> f{{"id", id}, {"decision", decision}};
  for (auto& [k, v] : change) f["change." + k] = v;
  out.push_back(ch::step(s, act(proposer, "propose", turn, f)));
  for (const auto& v : yes) out.push_back(ch::step(s, act(v, "vote", turn, {{"id", id}, {"value", "yes"}}, s.mandates.count("m-" + v) ? "m-" + v : std::string())));
  for (const auto& v : no) out.push_back(ch::step(s, act(v, "vote", turn, {{"id", id}, {"value", "no"}}, s.mandates.count("m-" + v) ? "m-" + v : std::string())));
  out.push_back(ch::step(s, act("sam", "amend", turn + 1, {{"id", id}})));
  return out;
}
const char* kTerms = R"({"format": "noetarium.compact/1", "id": "clearing compact", "parties": ["workshop", "guild", "press"],
  "articles": ["article 1", {"id": "article 2", "closed to reservation": true}, "article 3"], "entry into force": "all", "withdrawal": {"notice": 3}})";
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. An assembly counted by provider ----
  {
    Parties ps;
    for (const char* p : {"sam", "rae", "ana", "bea"}) ps.add({p, Kind::person});
    for (int i = 0; i < 3; i++) ps.add({"a-" + std::to_string(i), Kind::operative, "ana", "provider-a"});
    ps.add({"b-0", Kind::operative, "bea", "provider-b"});
    State st(ps, with("amend operational", R"({"body": "assembly", "rule": "majority", "count by": "provider", "tempo": "deliberative"})"));
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"ana", "contributor"}};
    ch::step(st, f);
    for (const std::string op : {"a-0", "a-1", "a-2", "b-0"}) {
      Act m = act(op == "b-0" ? "bea" : "ana", "mandate", 0);
      mandates::Mandate g;
      g.id = "m-" + op, g.principal = op == "b-0" ? "bea" : "ana", g.holder = op, g.from = 0, g.to = 100, g.kinds = {"join", "vote", "propose"};
      m.grant = g;
      ch::step(st, m);
      ch::step(st, act(op, "join", 0, {{"position", "contributor"}}, "m-" + op));
    }
    const auto first = decide(st, "p1", "amend operational", {{"receiving", "8"}}, {"a-0", "a-1", "a-2"}, {"b-0", "ana"}, 1);
    const auto later = decide(st, "p2", "amend operational", {{"receiving", "9"}}, {"a-0", "a-1", "a-2", "sam", "rae"}, {"b-0", "ana"}, 3);
    s.require("an assembly counted by provider: three agents of provider-a in favour against provider-b's agent and a person weigh 1/3 and the amendment is refused; "
              "with the steward and the reviewer, each a provider of judgement of their own, 3/5 and it is adopted",
              !first.back().admitted && has(first.back().reason, "has not passed") && later.back().admitted && st.rules.receiving_per_round == 9, first.back().reason + " / " + later.back().reason);
  }

  // ---- 2. A double majority of members and weight ----
  {
    auto doc = workshop();
    doc.get("positions")->set("member", symploke::jsonld::parse(R"({"held by": ["person", "organisation", "community"]})"));
    doc.get("bodies")->set("members", symploke::jsonld::parse(R"({"of": ["member"]})"));
    doc.get("aggregation")->set("amend operational", symploke::jsonld::parse(R"({"body": "members", "rule": {"double majority": {"members": "1/2", "weight": "2/3"}},
      "count by": "weight", "weights": {"north": 5, "south": 2, "east": 1, "west": "1/1"}, "tempo": "deliberative"})"));
    const auto c = constitutions::read(symploke::jsonld::dump(doc));
    Parties ps;
    ps.add({"sam", Kind::person});
    ps.add({"rae", Kind::person});
    for (const char* x : {"north", "south", "east", "west"}) ps.add({x, Kind::community});
    State st(ps, c);
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"north", "member"}, {"south", "member"}, {"east", "member"}, {"west", "member"}};
    ch::step(st, f);
    const auto alone = decide(st, "b1", "amend operational", {{"receiving", "7"}}, {"north"}, {"south", "east", "west"}, 1, "north");
    const auto two = decide(st, "b2", "amend operational", {{"receiving", "8"}}, {"north", "south"}, {"east", "west"}, 3, "north");
    const auto& d = st.rules.aggregation.at("amend operational");
    s.require("a double majority of members at least 1/2 and weight at least 2/3: the largest member alone (1/4 of the members, 5/9 of the weight) is refused; with a "
              "second (1/2 and 7/9) the amendment is adopted; the rule written as an object is read into its text",
              constitutions::admission(c).empty() && !alone.back().admitted && two.back().admitted && st.rules.receiving_per_round == 8 &&
                  d.rule == "double majority of members at least 1/2 and weight at least 2/3" && d.weights.at("west") == procedures::Rational(1),
              alone.back().reason + " / " + two.back().reason);
    const auto reweighed = decide(st, "b3", "amend operational", {{"decision", "amend operational"}, {"weights", R"({"north": 2, "south": 2, "east": 2, "west": 2})"}}, {"north", "south"},
                                  {"east", "west"}, 5, "north");
    const auto badly = decide(st, "b4", "amend operational", {{"decision", "amend operational"}, {"weights", R"({"north": -1})"}}, {"north", "south", "east"}, {}, 7, "north");
    s.require("weights are changed by amendment, every member then weighing two; a negative weight is refused",
              reweighed.back().admitted && st.rules.aggregation.at("amend operational").weights.at("north") == procedures::Rational(2) && !badly.back().admitted,
              reweighed.back().reason + " / " + badly.back().reason);
    auto faults_of = [](const std::string& written) {
      std::string all;
      for (const auto& x : constitutions::admission(with("amend operational", written))) all += x.reason + "; ";
      return all;
    };
    const auto f1 = faults_of(R"({"body": "assembly", "rule": "majority", "count by": "weight", "tempo": "deliberative"})");
    const auto f2 = faults_of(R"({"body": "assembly", "rule": {"double majority": {"members": "1/2", "weight": "2/3"}}, "count by": "origin", "tempo": "deliberative"})");
    const auto f3 = faults_of(R"({"body": "assembly", "rule": "at least 3/2", "count by": "origin", "tempo": "deliberative"})");
    const auto f4 = faults_of(R"({"body": "assembly", "rule": "majority", "count by": "weight", "weights": {"north": "heavy"}, "tempo": "deliberative"})");
    s.require("faults of form: a count by weight with no weight declared; a double majority counted by origin; a rule the kernel does not know; a weight that is no number",
              has(f1, "declares no positive weight") && has(f2, "counts by origin") && has(f3, "the kernel does not know") && has(f4, "no whole number or fraction"), f1 + f2 + f3 + f4);
  }

  // ---- 3. Compacts ratified, reserved, withdrawn ----
  auto community = [](const std::string& name) {
    auto doc = workshop();
    doc.set("name", symploke::jsonld::Json::string(name));
    Parties ps;
    ps.add({"sam", Kind::person});
    ps.add({"rae", Kind::person});
    State st(ps, constitutions::read(symploke::jsonld::dump(doc)));
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}, {"rae", "reviewer"}};
    ch::step(st, f);
    return st;
  };
  State w = community("workshop");
  {
    const auto closed = decide(w, "r0", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}, {"reservations", R"(["article 2"])"}}, {"sam", "rae"}, {}, 1);
    std::string others = kTerms;
    others.replace(others.find("\"workshop\", "), 12, "");
    const auto foreign = decide(w, "r1", "amend operational", {{"compact", "clearing compact"}, {"terms", others}}, {"sam", "rae"}, {}, 1);
    const auto ratified = decide(w, "r2", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}, {"reservations", R"(["article 3"])"}}, {"sam", "rae"}, {}, 2);
    const auto twice = decide(w, "r3", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}}, {"sam", "rae"}, {}, 4);
    const auto* r = constitutions::ratification(w.rules, "clearing compact", 4);
    const auto b = ch::brief(w, "sam");
    s.require("a compact ratified by the community's own amendment with its terms and a reservation of article 3; a reservation of article 2, closed to it, refused; terms "
              "naming other parties refused; a second ratification refused while the first stands; the briefing names the compact and the reservation",
              !closed.back().admitted && has(closed.back().reason, "closed to reservation") && !foreign.back().admitted && has(foreign.back().reason, "none of them") &&
                  ratified.back().admitted && r && r->since == 3 && r->has_terms && r->reserved("article 3", 4) && !twice.back().admitted && has(twice.back().reason, "ratified already") &&
                  b.compacts.size() == 1 && has(b.compacts.front(), "reserving article 3"),
              closed.back().reason + " / " + foreign.back().reason + " / " + ratified.back().reason);
    const auto unreserved = decide(w, "r4", "amend operational", {{"withdraw reservation", "clearing compact"}, {"reservations", "article 3"}}, {"sam", "rae"}, {}, 5);
    const auto withdrawn = decide(w, "r5", "amend operational", {{"withdraw compact", "clearing compact"}}, {"sam", "rae"}, {}, 7);
    const auto again = decide(w, "r6", "amend operational", {{"withdraw compact", "clearing compact"}}, {"sam", "rae"}, {}, 9);
    const bool during = !constitutions::compact_ids(w.rules, 10).empty(), after = constitutions::compact_ids(w.rules, 11).empty();
    const auto b2 = ch::brief(w, "sam");
    const auto renewed = decide(w, "r7", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}}, {"sam", "rae"}, {}, 11);
    s.require("the reservation withdrawn from turn 6; the community's withdrawal at turn 8 standing through the notice of three turns and gone at turn 11, a second "
              "withdrawal refused while it runs and the briefing saying so; ratified again once it took effect",
              unreserved.back().admitted && !constitutions::ratification(w.rules, "clearing compact", 6)->reserved("article 3", 6) && withdrawn.back().admitted && !again.back().admitted &&
                  has(again.back().reason, "withdrawing") && during && after && has(b2.compacts.front(), "with effect from turn 11") && renewed.back().admitted &&
                  constitutions::ratification(w.rules, "clearing compact", 12)->since == 12,
              unreserved.back().reason + " / " + withdrawn.back().reason + " / " + again.back().reason + " / " + renewed.back().reason);
    std::string firm = kTerms;
    firm.replace(firm.find("{\"notice\": 3}"), 13, "\"none\"");
    firm.replace(firm.find("clearing compact"), 16, "firm compact");
    State v = community("workshop");
    const auto ok = decide(v, "f1", "amend operational", {{"compact", "firm compact"}, {"terms", firm}}, {"sam", "rae"}, {}, 1);
    const auto out = decide(v, "f2", "amend operational", {{"withdraw compact", "firm compact"}}, {"sam", "rae"}, {}, 3);
    const auto plain = decide(v, "f3", "amend operational", {{"compact", "old compact"}, {"reservations", "article 1"}}, {"sam", "rae"}, {}, 5);
    const auto name = decide(v, "f4", "amend operational", {{"compact", "old compact"}}, {"sam", "rae"}, {}, 7);
    const auto quit = decide(v, "f5", "amend operational", {{"withdraw compact", "old compact"}}, {"sam", "rae"}, {}, 9);
    s.require("a compact that admits no withdrawal cannot be withdrawn from; a compact ratified by its name alone, as older records did, admits no reservation, and its "
              "withdrawal takes effect at once",
              ok.back().admitted && !out.back().admitted && has(out.back().reason, "admits no withdrawal") && !plain.back().admitted && has(plain.back().reason, "without its terms") &&
                  name.back().admitted && quit.back().admitted && constitutions::compact_ids(v.rules, 10) == std::vector<std::string>{"firm compact"},
              out.back().reason + " / " + plain.back().reason + " / " + quit.back().reason);
    State again_w;
    const auto rv = ch::replay(w.parties, community("workshop").rules, w.acts, w.journal, &again_w);
    s.require("the community's record replays with its seals and the same digest", rv.holds() && ch::relational_digest(again_w) == ch::relational_digest(w), rv.reason);
  }

  // ---- 4. Three communities' records read as one compact ----
  {
    State x = community("workshop"), y = community("guild"), z = community("press");
    decide(x, "k", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}}, {"sam", "rae"}, {}, 1);
    decide(y, "k", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}, {"reservations", "article 1"}}, {"sam", "rae"}, {}, 3);
    decide(z, "k", "amend operational", {{"compact", "clearing compact"}, {"terms", kTerms}}, {"sam", "rae"}, {}, 5);
    decide(y, "w", "amend operational", {{"withdraw compact", "clearing compact"}}, {"sam", "rae"}, {}, 8);
    const auto terms = compacts::read(std::string_view(kTerms));
    compacts::Record rec;
    for (const State* st : {&x, &y, &z})
      for (const auto& r : st->rules.compacts) rec[r.party].push_back(r);
    const auto at7 = compacts::standing(terms, rec, 7), at12 = compacts::standing(terms, rec, 12);
    const bool digests = rec.at("workshop").front().digest == rec.at("press").front().digest;
    s.require("three communities' own records read as one compact: in force from the third ratification at turn 6, the guild's reservation of article 1 excluding it between "
              "the guild and the others; the guild's withdrawal taking effect at turn 12 and the other two still bound; all three ratified the same terms by digest",
              at7.in_force && at7.entered == 6 && at7.bound.size() == 3 && !compacts::binds(terms, rec, 7, "article 1", "guild", "press") &&
                  compacts::binds(terms, rec, 7, "article 1", "workshop", "press") && at12.in_force && at12.bound == std::vector<std::string>{"workshop", "press"} && digests);
  }

  // ---- 5. The disclosure agreement as the compact of its two sides (Proposition 36) ----
  {
    calx::random::Stream r(20261127, 36, 0, 0);
    bool same = true;
    long long inforce = 0;
    for (int trial = 0; trial < 500; trial++) {
      State::Agreement g;
      g.terms.notice = between(r, 0, 4);
      g.terms.to = r.below(3) == 0 ? between(r, 5, 30) : -1;
      g.principal = r.below(6) == 0 ? -1 : between(r, 0, 12);
      g.community = r.below(6) == 0 ? -1 : between(r, 0, 12);
      if (g.principal >= 0 && g.community >= 0 && r.below(2) == 0) {
        g.denounced = std::max(g.principal, g.community) + between(r, 0, 10);
        g.denounced_by = r.below(2) == 0 ? "principal" : "community";
      }
      compacts::Terms t;
      t.format = "noetarium.compact/1", t.id = "agreement", t.parties = {"principal", "community"}, t.articles = {{"the terms", false}};
      t.entry_all = true, t.notice = g.terms.notice, t.to = g.terms.to;
      compacts::Record rec;
      for (const std::string side : {"principal", "community"}) {
        const long long at = side == "principal" ? g.principal : g.community;
        if (at < 0) continue;
        compacts::Ratification x;
        x.id = "agreement", x.party = side, x.has_terms = true, x.terms = t, x.since = at;
        if (g.denounced_by == side) x.withdrawn = g.denounced;
        rec[side].push_back(x);
      }
      for (long long turn = 0; turn <= 45; turn++) {
        const bool a = ch::in_force(g, turn), b = compacts::standing(t, rec, turn).in_force;
        same = same && a == b;
        inforce += a;
      }
    }
    s.require("on 500 random disclosure agreements, at every turn, the agreement is in force exactly when the compact of its two sides, entering into force when both have "
              "ratified and withdrawn by either after its notice, is (Proposition 36; " + std::to_string(inforce) + " turns in force)",
              same && inforce > 1000);
  }
  return s.done();
}
