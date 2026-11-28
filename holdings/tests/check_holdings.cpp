// Checks of holdings and the norms of access, on invented holdings and norms:
//   - a worked set of norms, each decision as the resolution states it: the most specific decides (a holding before a kind, the
//     holding itself before what it is kept within, a role before a position before anyone), a prohibition prevails at equal
//     specificity, a permission's conditions (a mandate, a quota, a need, a purpose) decide at its level, and where no norm speaks
//     the request is refused unless its kind is open;
//   - Proposition 8 on 3,000 random registers: the verdict does not depend on the order of the norms, the norm cited is the first of
//     its class in declared order, the verdict equals the definition read literally, a norm less specific than the deciding level
//     changes nothing, and a prohibition added at that level refuses; every refusal names its norm or the absence of one;
//   - Proposition 10 on 1,000 random registers: the briefing's one sweep equals decide over every target and aim;
//   - norms written in words; the written form read, and a breach of form found under closure or the custody of means.

#include <thesmos/holdings/holdings.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

using namespace thesmos::holdings;
using thesmos::kernel::Kind;

namespace {

long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
template <class T>
const T& pick(calx::random::Stream& r, const std::vector<T>& v) { return v[r.below(v.size())]; }

Norm norm(std::string id, Attribute who, Deontic d, Aim aim, Object over, Conditions c = {}) {
  Norm n;
  n.id = std::move(id), n.attribute = std::move(who), n.deontic = d, n.aim = aim, n.object = std::move(over), n.conditions = std::move(c);
  return n;
}
Target holding(std::string id, std::string kind, std::vector<std::string> within = {}, std::string custodian = "steward", std::string unit = {}) {
  Target t;
  t.id = std::move(id), t.kind = std::move(kind), t.within = std::move(within), t.custodian = std::move(custodian), t.unit = std::move(unit);
  t.metered = !t.unit.empty();
  return t;
}
Situation person(std::string id, std::set<std::string> positions) {
  Situation s;
  s.party = s.bearer = s.origin = std::move(id);
  s.positions = s.origin_positions = std::move(positions);
  return s;
}

// The definition read literally, written apart from decide: the matching norms, the greatest specificity among them, the norms of
// that specificity whose conditions hold, then prohibition, permission, or refusal; with no matching norm, open access.
bool by_definition(const Register& r, const Target& t, const Request& q, const Situation& s) {
  std::vector<std::pair<Specificity, std::size_t>> m;
  for (std::size_t i = 0; i < r.norms.size(); i++)
    if (const auto sp = matches(r.norms[i], t, q.aim, s)) m.push_back({*sp, i});
  if (m.empty()) return r.open.count(t.kind) && r.open.at(t.kind).count(q.aim);
  const Specificity top = std::max_element(m.begin(), m.end())->first;
  bool forbid = false, permit = false;
  for (const auto& [sp, i] : m)
    if (sp == top && unmet(r.norms[i], t, q, s).empty()) (r.norms[i].permits() ? permit : forbid) = true;
  return !forbid && permit;
}

struct Instance {
  Register r;
  std::vector<Target> targets;
  Situation s;
};

const std::vector<std::string> kPositions = {"member", "steward", "aide", "guest"};
const std::vector<std::string> kKinds = {"data", "tool", "budget"};
const std::vector<std::string> kHoldings = {"h0", "h1", "h2", "h3", "h4"};
const std::vector<std::string> kPurposes = {"", "analysis", "replication"};
const std::vector<std::string> kServices = {"", "", "provider-a", "provider-b"};

Instance random_instance(calx::random::Stream& g) {
  Instance x;
  // a forest of holdings: each kept within an earlier one, or within none
  std::vector<std::vector<std::string>> within(kHoldings.size());
  for (std::size_t i = 0; i < kHoldings.size(); i++) {
    if (i > 0 && g.below(2)) {
      const std::size_t p = g.below(i);
      within[i] = {kHoldings[p]};
      within[i].insert(within[i].end(), within[p].begin(), within[p].end());
    }
    const std::string kind = pick(g, kKinds);
    Target t = holding(kHoldings[i], kind, within[i], g.below(2) ? pick(g, kPositions) : "p-custodian", kind == "budget" ? "units" : "");
    t.office = t.custodian != "p-custodian";
    t.depositor = g.below(2) ? "p0" : "p1";
    t.aims = aims();
    x.targets.push_back(t);
    if (g.below(3) == 0) {  // the new holding a deposit within it would make
      Target f = t;
      f.fresh = true, f.id.clear(), f.depositor.clear();
      f.within.insert(f.within.begin(), kHoldings[i]);
      f.aims = {Aim::deposit};
      x.targets.push_back(f);
    }
  }
  const int n = static_cast<int>(between(g, 0, 9));
  for (int k = 0; k < n; k++) {
    Norm m;
    m.id = "n" + std::to_string(k);
    switch (g.below(6)) {
      case 0: m.attribute = {Who::anyone, {}}; break;
      case 1: m.attribute = {Who::kind, g.below(2) ? "person" : "operative"}; break;
      case 2: m.attribute = {Who::member, {}}; break;
      case 3: m.attribute = {Who::origin, pick(g, kPositions)}; break;
      case 4: m.attribute = {Who::position, pick(g, kPositions)}; break;
      default: m.attribute = {Who::role, g.below(2) ? "custodian" : "depositor"}; break;
    }
    m.deontic = g.below(3) == 0 ? Deontic::must_not : g.below(4) == 0 ? Deontic::must : Deontic::may;
    m.aim = pick(g, aims());
    switch (g.below(3)) {
      case 0: m.object = {Over::any, {}}; break;
      case 1: m.object = {Over::kind, pick(g, kKinds)}; break;
      default: m.object = {Over::holding, pick(g, kHoldings)}; break;
    }
    if (g.below(4) == 0) m.conditions.quota = between(g, 0, 4);
    if (g.below(5) == 0) m.conditions.need = "pressing need";
    if (g.below(4) == 0) m.conditions.purposes = {g.below(2) ? "analysis" : "replication"};
    if (g.below(5) == 0) m.conditions.under_mandate = true, m.conditions.mandate_kinds = {"use"};
    if (g.below(4) == 0) {
      m.conditions.transmit_declared = true;
      if (g.below(2)) m.conditions.transmit_any = true;
      else m.conditions.transmit_to = {"provider-a"};
    }
    x.r.norms.push_back(m);
  }
  if (g.below(2)) x.r.open[pick(g, kKinds)] = {pick(g, aims())};
  Situation& s = x.s;
  s.party = g.below(3) == 0 ? "op" : (g.below(2) ? "p0" : "p1");
  s.kind = s.party == "op" ? Kind::operative : Kind::person;
  s.bearer = s.party == "op" ? "p0" : s.party;
  s.origin = s.bearer;
  for (const auto& p : kPositions)
    if (g.below(3) == 0) s.positions.insert(p);
  if (s.party != s.bearer)
    for (const auto& p : kPositions)
      if (g.below(3) == 0) s.bearer_positions.insert(p);
  for (const auto& p : kPositions)
    if (g.below(3) == 0) s.origin_positions.insert(p);
  s.mandated = s.party == "op" || g.below(4) == 0;
  if (s.mandated && g.below(2)) s.mandate_kinds = {"use"};
  if (g.below(2)) s.needs = {"pressing need"};
  for (const Norm& m : x.r.norms)
    if (g.below(3) == 0) s.used[m.id] = between(g, 0, 4);
  return x;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- A worked set of norms -------------------------------------------------------------------------------------------------------
  Register r;
  r.norms = {
      norm("shelf consulted", {Who::member, {}}, Deontic::may, Aim::consult, {Over::holding, "shelf"}),
      norm("shelf deposited", {Who::position, "contributor"}, Deontic::may, Aim::deposit, {Over::holding, "shelf"}),
      norm("no advertising", {Who::position, "contributor"}, Deontic::must_not, Aim::deposit, {Over::holding, "shelf"}, Conditions{false, {}, {}, -1, {}, {"advertising"}}),
      norm("shelf withdrawn", {Who::role, "custodian"}, Deontic::may, Aim::withdraw, {Over::holding, "shelf"}),
      norm("item kept", {Who::anyone, {}}, Deontic::must_not, Aim::consult, {Over::holding, "item"}),
      norm("tools used", {Who::anyone, {}}, Deontic::may, Aim::use, {Over::kind, "tool"}),
      norm("bench closed to applicants", {Who::position, "applicant"}, Deontic::must_not, Aim::use, {Over::holding, "bench"}),
      norm("pool by quota", {Who::position, "member"}, Deontic::may, Aim::use, {Over::holding, "pool"}, Conditions{false, {}, {}, 30, {}, {}}),
      norm("pool by need", {Who::position, "candidate"}, Deontic::may, Aim::use, {Over::holding, "pool"}, Conditions{false, {}, {}, -1, "pressing need", {}}),
      norm("pool for agents", {Who::kind, "operative"}, Deontic::may, Aim::use, {Over::holding, "pool"}, Conditions{true, {"use"}, {"writing"}, -1, {}, {}}),
      norm("pool for hands", {Who::position, "hand"}, Deontic::may, Aim::use, {Over::holding, "pool"}, Conditions{false, {}, {}, 10, {}, {}}),
  };
  r.open["knowledge"] = {Aim::consult};
  const Target shelf = holding("shelf", "data"), bench = holding("bench", "tool"), item = holding("item", "data", {"shelf"}), guide = holding("guide", "knowledge"),
               pool = holding("pool", "budget", {}, "board", "words");
  Target into = holding({}, "data", {"shelf"});
  into.fresh = true;
  const Situation contributor = person("ana", {"contributor"}), outsider = person("bo", {}), steward = person("cy", {"steward"}),
                  applicant = person("di", {"applicant"});
  auto decided = [&](const Target& t, Aim a, const Situation& x, long long amount = 0, std::string purpose = {}) { return decide(r, t, Request{a, amount, purpose}, x); };

  const auto c1 = decided(shelf, Aim::consult, contributor), c2 = decided(shelf, Aim::consult, outsider);
  s.require("anyone who holds a position may consult the shelf, and a party holding none is refused for want of a norm",
            c1.permitted && c1.norm == "shelf consulted" && !c2.permitted && !c2.spoken && c2.reason.find("no norm permits") != std::string::npos, c2.reason);
  const auto d1 = decided(into, Aim::deposit, contributor), d2 = decided(into, Aim::deposit, contributor, 0, "advertising");
  s.require("a contributor may deposit within the shelf, and a prohibition of equal specificity prevails when its condition holds",
            d1.permitted && d1.norm == "shelf deposited" && !d2.permitted && d2.norm == "no advertising" && d2.reason.find("forbids") != std::string::npos, d2.reason);
  const auto i1 = decided(item, Aim::consult, contributor);
  s.require("a norm on the holding itself is nearer than one on what it is kept within, and decides", !i1.permitted && i1.norm == "item kept" && i1.level->nearness == 0, i1.reason);
  const auto w1 = decided(shelf, Aim::withdraw, steward), w2 = decided(shelf, Aim::withdraw, contributor);
  s.require("its custodian, the holder of the office that keeps it, may withdraw it, and no one else", w1.permitted && !w2.permitted && !w2.spoken, w2.reason);
  const auto b1 = decided(bench, Aim::use, applicant), b2 = decided(bench, Aim::use, contributor);
  s.require("a prohibition on the bench itself is more specific than a permission on its kind: applicants are refused, others permitted",
            !b1.permitted && b1.norm == "bench closed to applicants" && b2.permitted && b2.norm == "tools used", b1.reason);
  Situation member = person("el", {"member"});
  member.used["pool by quota"] = 20;
  const auto p1 = decided(pool, Aim::use, member, 10), p2 = decided(pool, Aim::use, member, 11);
  s.require("within a quota of 30 a round, a member who drew 20 may draw 10 and is refused 11, the quota named",
            p1.permitted && !p2.permitted && p2.norm == "pool by quota" && p2.reason.find("quota is 30 words a round, of which 20 is drawn") != std::string::npos, p2.reason);
  Situation candidate = person("fi", {"candidate"});
  const auto n1 = decided(pool, Aim::use, candidate, 5);
  candidate.needs.insert("pressing need");
  const auto n2 = decided(pool, Aim::use, candidate, 5);
  s.require("by need under a declared standard: refused until the record attests the need, then permitted",
            !n1.permitted && n1.reason.find("attests no need") != std::string::npos && n2.permitted, n1.reason);
  Situation agent;
  agent.party = "fi/agent", agent.bearer = agent.origin = "fi", agent.kind = Kind::operative, agent.mandated = true, agent.mandate_kinds = {"use"};
  const auto m1 = decided(pool, Aim::use, agent, 5);
  agent.mandate_matters = {"writing"};
  const auto m2 = decided(pool, Aim::use, agent, 5);
  s.require("under a mandate of given kinds and matters: refused while its mandate lacks the matter, then permitted",
            !m1.permitted && m1.reason.find("does not cover the matter writing") != std::string::npos && m2.permitted, m1.reason);
  Situation hand;
  hand.party = "el/hand", hand.bearer = hand.origin = "el", hand.kind = Kind::operative, hand.mandated = true, hand.positions = {"hand"}, hand.bearer_positions = {"member"};
  const auto h1 = decided(pool, Aim::use, hand, 15), h2 = decided(pool, Aim::use, hand, 10);
  s.require("a position the author holds itself outranks one reaching it through the party that answers for it: a member's hand draws within the hands' quota",
            !h1.permitted && h1.norm == "pool for hands" && h2.permitted && h2.norm == "pool for hands", h1.reason);
  Register sent;
  sent.norms = {norm("shelf read and sent", {Who::member, {}}, Deontic::may, Aim::consult, {Over::holding, "shelf"}, Conditions{false, {}, {}, -1, {}, {}, true, false, {"provider-a"}}),
                norm("bench never sent", {Who::anyone, {}}, Deontic::must_not, Aim::consult, {Over::holding, "bench"}, Conditions{false, {}, {}, -1, {}, {}, true, true, {}}),
                norm("bench read", {Who::anyone, {}}, Deontic::may, Aim::consult, {Over::holding, "bench"})};
  auto sending = [&](const Target& t, std::string service) { return decide(sent, t, Request{Aim::consult, 0, {}, std::move(service)}, contributor); };
  const auto x1 = sending(shelf, "provider-a"), x2 = sending(shelf, "provider-b"), x3 = sending(bench, ""), x4 = sending(bench, "provider-a"), x5 = decide(r, shelf, Request{Aim::consult, 0, {}, "provider-a"}, contributor);
  s.require("transmission: a permission sends only where it says, and one silent on it sends nowhere; a prohibition of sending forbids only what is sent",
            x1.permitted && !x2.permitted && x2.reason.find("provider-b") != std::string::npos && x3.permitted && !x4.permitted && x4.norm == "bench never sent" && !x5.permitted &&
                x5.reason.find("sent outside the community") != std::string::npos,
            x2.reason + " / " + x4.reason + " / " + x5.reason);
  const Visibility office{Visibility::Level::offices, {"steward"}};
  s.require("visibility: the public is seen by anyone, the members' by anyone holding a position, an office's by its holders alone",
            sees(Visibility::everyone(), {}) && !sees(Visibility::members(), {}) && sees(Visibility::members(), {"guest"}) && sees(office, {"steward"}) && !sees(office, {"member"}));
  const auto g1 = decided(guide, Aim::consult, outsider), g2 = decided(guide, Aim::use, outsider);
  s.require("where no norm speaks, a kind declared open is open to its aims alone", g1.permitted && g1.open && !g2.permitted && !g2.open, g2.reason);

  // ---- Proposition 8 on random registers -------------------------------------------------------------------------------------------
  {
    calx::random::Stream g(41, 1, 0, 0);
    long long order = 0, cited = 0, literal = 0, specialis = 0, prevails = 0, named = 0, cases = 0;
    for (int run = 0; run < 3000; run++) {
      Instance x = random_instance(g);
      for (const Target& t : x.targets)
        for (Aim a : t.aims) {
          const Request q{a, least(t, a) + static_cast<long long>(g.below(2)), pick(g, kPurposes), pick(g, kServices)};
          const Decision d = decide(x.r, t, q, x.s);
          cases++;
          // the order of the norms: shuffled, the verdict and level are the same, and the norm cited is the first of its class
          Register shuffled = x.r;
          for (std::size_t i = shuffled.norms.size(); i > 1; i--) std::swap(shuffled.norms[i - 1], shuffled.norms[g.below(i)]);
          const Decision e = decide(shuffled, t, q, x.s);
          order += e.permitted != d.permitted || e.spoken != d.spoken || e.level != d.level;
          if (e.spoken) {
            bool first = true;
            for (std::size_t i = 0; i < e.index; i++) {
              const auto sp = matches(shuffled.norms[i], t, a, x.s);
              const bool same_class = shuffled.norms[i].permits() == shuffled.norms[e.index].permits();
              const bool applies = unmet(shuffled.norms[i], t, q, x.s).empty() == unmet(shuffled.norms[e.index], t, q, x.s).empty();
              if (sp && *sp == *e.level && same_class && applies) first = false;
            }
            cited += !first;
          }
          literal += by_definition(x.r, t, q, x.s) != d.permitted;
          named += !d.permitted && (d.spoken ? d.reason.find("\"" + d.norm + "\"") == std::string::npos : d.reason.find("no norm permits") == std::string::npos);
          if (d.spoken) {
            // a norm less specific than the deciding level changes nothing
            Register more = x.r;
            Norm extra = norm("extra", {Who::anyone, {}}, g.below(2) ? Deontic::may : Deontic::must_not, a, {Over::any, {}});
            if (*d.level > Specificity{0, 0, 0}) {
              more.norms.insert(more.norms.begin() + static_cast<long>(g.below(more.norms.size() + 1)), extra);
              specialis += decide(more, t, q, x.s).permitted != d.permitted;
            }
            // a prohibition without conditions at the deciding level refuses
            Register forbidden = x.r;
            Norm no = x.r.norms[d.index];
            no.id = "no", no.deontic = Deontic::must_not, no.conditions = {};
            forbidden.norms.push_back(no);
            prevails += decide(forbidden, t, q, x.s).permitted;
          }
        }
    }
    s.require("the verdict and its level do not depend on the order of the norms (Proposition 8; " + std::to_string(cases) + " requests on 3,000 random registers)", order == 0,
              std::to_string(order));
    s.require("the norm cited is the first of its class, in declared order, at the deciding level", cited == 0, std::to_string(cited));
    s.require("the verdict equals the definition read literally", literal == 0, std::to_string(literal));
    s.require("a norm less specific than the deciding level changes no verdict, and a prohibition added at that level refuses", specialis == 0 && prevails == 0,
              std::to_string(specialis) + " / " + std::to_string(prevails));
    s.require("every refusal names the norm that decides it, or the absence of any", named == 0, std::to_string(named));
  }

  // ---- Proposition 10: the briefing equals decide over every target and aim ----------------------------------------------------------
  {
    calx::random::Stream g(43, 1, 0, 0);
    long long differ = 0, listed = 0, pairs = 0;
    for (int run = 0; run < 1000; run++) {
      const Instance x = random_instance(g);
      const std::string purpose = pick(g, kPurposes);
      const auto entries = brief(x.r, x.targets, x.s, purpose);
      std::map<std::pair<std::size_t, Aim>, const Entry*> by;
      for (const Entry& e : entries) by[{e.target, e.aim}] = &e;
      for (std::size_t k = 0; k < x.targets.size(); k++)
        for (Aim a : x.targets[k].aims) {
          pairs++;
          const Decision d = decide(x.r, x.targets[k], Request{a, least(x.targets[k], a), purpose}, x.s);
          const auto it = by.find({k, a});
          const bool expected = d.spoken || d.open;
          if ((it != by.end()) != expected) differ++;
          else if (it != by.end()) {
            listed++;
            differ += it->second->decision.permitted != d.permitted || it->second->decision.norm != d.norm || it->second->decision.reason != d.reason;
          }
        }
    }
    s.require("the briefing's one sweep over the norms lists every target and aim on which a norm speaks or open access applies, each with decide's verdict, norm and "
              "reason (Proposition 10; " + std::to_string(pairs) + " pairs, " + std::to_string(listed) + " listed)",
              differ == 0, std::to_string(differ));
  }

  // ---- Words and the written form -----------------------------------------------------------------------------------------------------
  s.require("norms are written in words", text(r.norms[1]) == "contributor may deposit to the shelf." &&
                                              text(r.norms[7]) == "member may use the pool within 30 a round." &&
                                              text(r.norms[9]) == "any operative may use the pool under a mandate of use on writing.",
            text(r.norms[1]) + " / " + text(r.norms[7]) + " / " + text(r.norms[9]));
  const std::string written = R"({
    "holdings": {"shelf": {"kind": "data", "custodian": "steward", "review": "another origin", "visibility": "public", "provenance": {"source": "the founding", "licence": "CC BY 4.0"}},
                 "pool": {"kind": "budget", "unit": "words", "quantity": 0, "custodian": "board"},
                 "account": {"kind": "reference", "custodian": {"party": "a bank"}, "visibility": {"office": ["board"]}}},
    "access": {"open": {"knowledge": ["consult"]}, "kinds": ["tool"],
               "norms": [{"id": "consult", "who": "any position", "may": "consult", "object": "shelf", "transmit": ["a model provider"]},
                         {"id": "draw", "who": {"origin": "member"}, "may": "use", "object": {"holding": "pool"}, "quota": 2000, "for": ["writing"],
                          "under mandate": {"kinds": ["use"], "matters": ["writing"]}, "or else": "the draw is returned"},
                         {"id": "tools", "who": {"kind": "operative"}, "must not": "withdraw", "object": {"kind": "tool"}}]}})";
  const Json doc = symploke::jsonld::parse(written);
  const Register w = read(doc.get("holdings"), doc.get("access"));
  const std::map<std::string, std::vector<std::string>> positions = {{"steward", {"person"}}, {"board", {"person"}}, {"member", {"person"}}};
  s.require("the written form is read: three holdings, one kept by a party, one metered, each with its visibility, three norms with their conditions and a "
            "transmission, a kind declared open",
            w.holdings.size() == 3 && !w.holdings.at("account").office && w.holdings.at("pool").metered() && w.norms.size() == 3 &&
                w.norms[1].conditions.quota == 2000 && w.norms[1].conditions.under_mandate && w.norms[1].or_else == "the draw is returned" &&
                w.norms[2].deontic == Deontic::must_not && w.open.at("knowledge").count(Aim::consult) && faults(w, positions).empty() &&
                w.holdings.at("shelf").visibility.level == Visibility::Level::everyone && w.holdings.at("pool").visibility.level == Visibility::Level::members &&
                w.holdings.at("account").visibility.offices == std::set<std::string>{"board"} && w.norms[0].conditions.transmit_to == std::set<std::string>{"a model provider"},
            faults(w, positions).empty() ? text(w.norms[1]) : faults(w, positions).front().reason);
  const std::vector<std::pair<int, std::string>> breaches = {
      {1, R"({"norms": [{"id": "x", "who": "anyone", "may": "consult", "object": "attic"}]})"},
      {1, R"({"norms": [{"id": "x", "who": "anyone", "may": "borrow", "object": "shelf"}]})"},
      {1, R"({"norms": [{"id": "x", "who": "ghost", "may": "consult", "object": "shelf"}]})"},
      {1, R"({"norms": [{"id": "x", "who": "anyone", "may": "consult"}, {"id": "x", "who": "anyone", "may": "use"}]})"},
      {1, R"({"norms": [{"id": "x", "who": "anyone", "may": "consult", "object": {"kind": "tapestry"}}]})"},
  };
  int found = 0;
  for (const auto& [condition, access] : breaches) {
    const Json a = symploke::jsonld::parse(access);
    for (const Fault& f : faults(read(doc.get("holdings"), &a), positions))
      if (f.condition == condition) {
        found++;
        break;
      }
  }
  {
    const Json a = symploke::jsonld::parse(R"({"shelf": {"kind": "data", "custodian": "steward", "visibility": {"office": "treasurer"}}})");
    for (const Fault& f : faults(read(&a, nullptr), positions))
      if (f.condition == 1 && f.reason.find("treasurer") != std::string::npos) {
        found++;
        break;
      }
  }
  const std::vector<std::string> custody = {R"({"shelf": {"kind": "data", "custodian": "librarian"}})", R"({"shelf": {"kind": "data"}})",
                                            R"({"a": {"kind": "data", "custodian": "steward", "within": "b"}, "b": {"kind": "data", "custodian": "steward", "within": "a"}})",
                                            R"({"shelf": {"kind": "data", "custodian": "steward", "provenance": {"digest": "abc"}}})",
                                            R"({"shelf": {"kind": "data", "custodian": "steward", "review": "a vote"}})"};
  for (const auto& h : custody) {
    const Json a = symploke::jsonld::parse(h);
    for (const Fault& f : faults(read(&a, nullptr), positions))
      if (f.condition == 6) {
        found++;
        break;
      }
  }
  s.require("a breach of form is found: a norm naming an undeclared holding, aim, position or kind, or declared twice, a holding shown to an undeclared office "
            "(closure); a holding kept by no office, "
            "within a cycle, with a malformed digest or an unknown review (the custody of means)",
            found == 11, std::to_string(found) + " of 11");
  s.require("a digest is made by archeion's rule", digest("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" && is_digest(digest("")));
  return s.done();
}
