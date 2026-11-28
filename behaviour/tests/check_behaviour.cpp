// Checks of the taxonomy of behaviour (decision 0008): the base taxonomy read from schema/behaviour.json is the ground's, every kind
// of the ground placed once with its force, and kategoria admits it; a community declares an objection counting as a claim, an appeal
// counting as an objection and a category of conduct, and its record replays; declarations are refused for a changed force, a cycle,
// an unknown target, a kind of the ground redeclared, a condition that does not read or reads what no act carries, a trigger whose
// effect the ground does not mark as by law, a counts-as rule through an analytic or unreadable category, and a scene that is not
// declared; on 200 random taxonomies and chronicles every admitted act of a declared kind has exactly the effect of its reduct
// (Proposition 14); on 100 random runs through the basic counts-as every act is joined to the one episode that constituted it
// (Proposition 15); the state is given again by the acts and the turns alone, the journal withheld or changed, and trigger rules lapse
// a commitment and end a position as a replay does again (Proposition 16, G9); a counts-as rule makes an operative's reading a
// transmission its norm refuses; categories, omissions, nearness and standing; a revision renaming a kind and widening a category is
// admitted, by the direct decision and by kategoria alike, and the record carried as the left Kan extension, while revisions that move
// a kind to another primitive, break the order counts as, or narrow a category are refused (Proposition 17); and a community revises
// its taxonomy by its own procedure, each act judged under the taxonomy in force when it was admitted.

#include <thesmos/behaviour/ontology.hpp>
#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/conduct.hpp>

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
namespace bh = thesmos::behaviour;

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
const std::string& workshop_text() {
  static const std::string t = slurp(std::string(THESMOS_DATA) + "/examples/workshop.json");
  return t;
}
// The illustrative workshop with a part behaviour.
constitutions::Constitution with_part(const std::string& part) {
  auto doc = symploke::jsonld::parse(workshop_text());
  doc.set("behaviour", symploke::jsonld::parse(part));
  return constitutions::read(symploke::jsonld::dump(doc));
}
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "cy", "dee"}) ps.add({p, Kind::person});
  ps.add({"cy/agent", Kind::operative, "cy", "provider-a"});
  return ps;
}
State founded(const constitutions::Constitution& c) {
  State s(parties(), c);
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"cy", "contributor"}};
  chronicle::step(s, f);
  return s;
}
bool any_fault(const constitutions::Constitution& c, const std::string& part) {
  for (const auto& f : constitutions::admission(c))
    if (f.reason.find(part) != std::string::npos) return true;
  return false;
}

const char* kPart = R"({
  "format": "noetarium.behaviour/1", "name": "workshop", "version": 1, "extends": "base 1",
  "kinds": [
    {"kind": "objection", "force": "directive", "counts as": "claim", "fields": {"against": "about"}, "fixed": {"type": "objection"}, "valid when": ["field.about is present"], "in": ["intake"]},
    {"kind": "appeal", "force": "directive", "counts as": "objection", "valid when": ["field.ruling is present"]},
    {"kind": "merge accepted", "force": "declarative", "counts as": "accept", "in": ["tasks"]}
  ],
  "features": [{"name": "pages read", "space": "real", "unit": "pages", "scale": 50}],
  "conduct": [
    {"category": "long reading", "where": "operation is 'read' and pages-read above 20"},
    {"category": "provider reading", "where": "kind is 'consult' and field.provider is present"},
    {"category": "a likely draft", "where": "operation is 'write'", "analytic": true}
  ],
  "counts as": [{"rule": "a reading reaches the provider", "conduct": "provider reading", "as": {"transmission": "field.provider"}}]
})";

std::string replaced(std::string text, const std::string& from, const std::string& to) {
  const auto at = text.find(from);
  if (at != std::string::npos) text.replace(at, from.size(), to);
  return text;
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. The base taxonomy --------------------------------------------------------------------------------------------------------
  {
    const bh::Part first = bh::read_part(slurp(std::string(THESMOS_DATA) + "/schema/behaviour.json"));
    const bh::Part second = bh::read_part(slurp(std::string(THESMOS_DATA) + "/schema/behaviour-2.json"));
    const bh::Part third = bh::read_part(slurp(std::string(THESMOS_DATA) + "/schema/behaviour-3.json"));
    const bh::Part fourth = bh::read_part(slurp(std::string(THESMOS_DATA) + "/schema/behaviour-4.json"));
    const bh::Part file = bh::read_part(slurp(std::string(THESMOS_DATA) + "/schema/behaviour-5.json"));
    s.require("the base taxonomy's five versions read from schema/behaviour.json and behaviour-2.json to behaviour-5.json are those the ground holds, the fifth in "
              "force, and none has a fault of form",
              first == bh::first_base() && second == bh::second_base() && third == bh::third_base() && fourth == bh::fourth_base() && file == bh::base() &&
                  bh::faults(bh::first_base()).empty() && bh::faults(bh::second_base()).empty() && bh::faults(bh::third_base()).empty() &&
                  bh::faults(bh::fourth_base()).empty() && bh::faults(bh::base()).empty(),
              file == bh::base() ? (bh::faults(bh::base()).empty() ? (first == bh::first_base() ? "" : "the first version differs") : bh::faults(bh::base()).front())
                                 : bh::canonical(file).substr(0, 300));
    bool placed = bh::base().primitives.size() == acts::ground().size();
    for (const auto& [kind, force] : acts::ground()) {
      const bh::Primitive* p = bh::base().primitive(kind);
      placed = placed && p && p->force == force && std::find(bh::base().classes.begin(), bh::base().classes.end(), p->cls) != bh::base().classes.end();
      for (const auto& e : p ? p->effects : std::vector<std::string>{}) placed = placed && bh::base().effect(e) != nullptr;
    }
    s.require("every kind of the ground (" + std::to_string(acts::ground().size()) + ") is placed once, in a class, with its force and effect primitives the base declares", placed);
    const auto v = bh::verify(bh::base());
    const auto printed = kategoria::ontologies::print(bh::ontology_of(bh::base()));
    const auto back = kategoria::ontologies::read(printed);
    s.require("kategoria admits the base taxonomy as an ontology: a forest in the three registers with one root, its kinds and registers orders, the register a functor; "
              "printed in kategoria's language it reads back to itself",
              v.admitted() && back.ok() && back.ontology == bh::ontology_of(bh::base()), v.verdict.reason);
    std::vector<std::string> by_law;
    for (const auto& e : bh::base().effects)
      for (const auto& f : e.by_law) by_law.push_back(f);
    s.require("the effects the law produces with no author are six: a position's end, a commitment's lapse and its overdue mark, a loan's return at its term, an "
              "agreement's end at its term or notice, and an item's reopening when a taking's term or a redelivery runs out",
              by_law == std::vector<std::string>{"end position", "lapse", "overdue", "loan returns", "agreement ends", "item reopens"});
  }

  // ---- 2. A community declares kinds and categories ----------------------------------------------------------------------------------
  const constitutions::Constitution decl = with_part(kPart);
  {
    const auto faults = constitutions::admission(decl);
    s.require("a community's part behaviour is read and the constitution admitted with it", decl.behaviour_declared && faults.empty(), faults.empty() ? "" : faults.front().reason);
    const auto v = bh::verify(decl.behaviour);
    s.require("and kategoria admits its taxonomy in force, the declared kinds beneath the kinds they count as", v.admitted(), v.verdict.reason);
  }
  State c = founded(decl);
  {
    const auto o1 = chronicle::step(c, act("cy", "objection", 1, {{"id", "k1"}, {"about", "entry 7 of the catalogue"}}));
    const auto o2 = chronicle::step(c, act("cy", "objection", 1, {{"id", "k2"}}));
    const auto o3 = chronicle::step(c, act("cy", "appeal", 2, {{"id", "k3"}, {"about", "entry 9"}, {"ruling", "r1"}}));
    const auto o4 = chronicle::step(c, act("sam", "answer", 2, {{"id", "k1"}, {"finding", "corrected"}}));
    s.require("an objection counts as a claim: admitted, the claim is the claimant's against what the objection is about, and the act is kept as written with its "
              "reading as a claim of type objection",
              o1.admitted && c.claims.count("k1") && c.claims.at("k1").against == "entry 7 of the catalogue" && c.acts[1].kind == "objection" && c.readings[1].kind == "claim" &&
                  c.readings[1].field("type") == "objection",
              o1.reason);
    s.require("an objection that names nothing it is about is refused by the condition the community declared, and an appeal, counting as an objection, is a claim",
              !o2.admitted && o2.reason.find("field.about is present") != std::string::npos && o3.admitted && c.claims.count("k3") && o4.admitted, o2.reason + " / " + o3.reason);
    chronicle::step(c, act("cy", "offer", 3, {{"id", "t1"}, {"to", "sam"}, {"matter", "catalogue"}}));
    chronicle::step(c, act("sam", "take", 3, {{"id", "t1"}}));
    chronicle::step(c, act("cy", "deliver", 4, {{"id", "t1"}, {"artefact", "entries"}}));
    const auto merged = chronicle::step(c, act("sam", "merge accepted", 5, {{"id", "t1"}}));
    const auto replayed = chronicle::replay(c.parties, decl, c.acts, c.journal);
    s.require("a merge accepted counts as an acceptance and discharges the delivery; the record, declared kinds and all, replays and seals as recorded",
              merged.admitted && c.commitments.at("t1").status == commitments::Status::discharged && replayed.holds(), merged.reason + " / " + replayed.reason);
    const auto b = chronicle::brief(c, "cy");
    s.require("the briefing gives the kinds the base and the community declare (the base's notice, apply, question and petition first) and what each counts as, and its "
              "categories of conduct (the base's four, the reading by an operative among them, and the community's), each said decidable or analytic",
              b.declared.size() == 7 && b.declared[4].find("objection counts as claim") == 0 && b.conduct.size() == 7 &&
                  std::find(b.acts.begin(), b.acts.end(), "objection") != b.acts.end(),
              b.declared.size() < 5 ? "" : b.declared[4]);
  }

  // ---- 3. Declarations refused ------------------------------------------------------------------------------------------------------
  {
    const auto force = with_part(replaced(kPart, R"("kind": "objection", "force": "directive")", R"("kind": "objection", "force": "assertive")"));
    const auto cycle = with_part(replaced(kPart, R"("counts as": "claim")", R"("counts as": "appeal")"));
    const auto unknown = with_part(replaced(kPart, R"("counts as": "claim")", R"("counts as": "plea")"));
    const auto ground = with_part(replaced(kPart, R"("kind": "merge accepted")", R"("kind": "accept")"));
    s.require("declarations are refused for a changed force, a cycle of counting, an unknown kind counted as, and a kind of the ground declared again",
              any_fault(force, "keeps the force of what it counts as") && any_fault(cycle, "never reach a primitive") && any_fault(unknown, "which is no kind in force") &&
                  any_fault(ground, "is a kind of the ground"));
    const auto unread = with_part(replaced(kPart, R"("field.about is present")", R"("field.about is")"));
    const auto journal = with_part(replaced(kPart, R"("field.about is present")", R"("operation is 'read'")"));
    const auto scene = with_part(replaced(kPart, R"("in": ["intake"])", R"("in": ["market"])"));
    s.require("a condition that does not read, a condition reading what no act carries, and a scene the constitution lacks are each refused",
              any_fault(unread, "does not read") && any_fault(journal, "reads what no act carries") && any_fault(scene, "the scene market"));
    const auto analytic = with_part(replaced(kPart, R"("conduct": "provider reading")", R"("conduct": "a likely draft")"));
    const auto unreadable = with_part(replaced(kPart, R"("conduct": "provider reading")", R"("conduct": "long reading")"));
    const auto status = with_part(replaced(kPart, R"({"transmission": "field.provider"})", R"({"payment": "field.provider"})"));
    s.require("a counts-as rule through an analytic category, through one the fold cannot read, or to a status no norm reads is refused",
              any_fault(analytic, "never decides") && any_fault(unreadable, "the fold cannot read it") && any_fault(status, "which no norm reads"));
    const auto lawless = with_part(replaced(kPart, R"("counts as": [)", R"("triggers": [{"rule": "an office granted", "when": {"position": "steward", "held for more than": 5}, "then": "grant"}], "counts as": [)"));
    const auto formless = with_part(replaced(kPart, R"("counts as": [)", R"("triggers": [{"rule": "an office ends", "then": "end position"}], "counts as": [)"));
    s.require("a trigger rule whose effect the ground does not mark as by law is refused, and so is one not in its effect's form",
              any_fault(lawless, "does not mark as an effect by law") && any_fault(formless, "names neither"));
  }

  // ---- 4. Proposition 14 on random taxonomies and chronicles ------------------------------------------------------------------------
  {
    calx::random::Stream r(29, 4, 0, 0);
    const std::vector<std::string> prims = {"claim", "say", "report", "offer", "commit", "assess"};
    const std::vector<std::string> authors = {"sam", "rae", "cy", "dee"};
    int trials = 0, declared_acts = 0, refused_by_condition = 0, reducts_refused = 0, digests_differ = 0, readings_differ = 0;
    for (int trial = 0; trial < 200; trial++) {
      std::string kinds;
      std::vector<std::pair<std::string, std::string>> declared;  // name, primitive
      const long long n = between(r, 1, 6);
      for (long long i = 0; i < n; i++) {
        const bool chain = !declared.empty() && between(r, 0, 1);
        const auto& target = chain ? declared[static_cast<std::size_t>(between(r, 0, static_cast<long long>(declared.size()) - 1))] : std::pair<std::string, std::string>{};
        const std::string onto = chain ? target.first : prims[static_cast<std::size_t>(between(r, 0, 5))];
        const std::string prim = chain ? target.second : onto;
        const std::string name = "k" + std::to_string(i);
        std::string d = R"({"kind": ")" + name + R"(", "force": ")" + std::string(acts::word(*acts::force_of(prim))) + R"(", "counts as": ")" + onto + "\"";
        if (between(r, 0, 1)) d += R"(, "fields": {"against": "about"})";
        if (between(r, 0, 1)) d += R"(, "fixed": {"type": ")" + name + "\"}";
        const long long cond = between(r, 0, 3);
        if (cond == 1) d += R"(, "valid when": ["field.about is present"])";
        if (cond == 2) d += R"(, "valid when": ["turn at least 3"])";
        if (cond == 3) d += R"(, "valid when": ["author holds 'contributor'"])";
        d += "}";
        kinds += (kinds.empty() ? "" : ", ") + d;
        declared.push_back({name, prim});
      }
      const auto cons = with_part(R"({"format": "noetarium.behaviour/1", "name": "random", "version": 1, "kinds": [)" + kinds + "]}");
      if (!constitutions::admission(cons).empty()) continue;
      trials++;
      State a = founded(cons), b = founded(cons);
      for (int k = 1; k <= 40; k++) {
        const bool use_declared = between(r, 0, 1);
        const std::string kind = use_declared ? declared[static_cast<std::size_t>(between(r, 0, static_cast<long long>(declared.size()) - 1))].first : prims[static_cast<std::size_t>(between(r, 0, 5))];
        Act x = act(authors[static_cast<std::size_t>(between(r, 0, 3))], kind, k / 3, {{"id", "x" + std::to_string(k)}});
        if (between(r, 0, 1)) x.fields["about"] = "subject " + std::to_string(between(r, 0, 3));
        x.fields["to"] = "sam";
        x.fields["object"] = "cy", x.fields["criterion"] = "care", x.fields["value"] = "good";
        const auto o = chronicle::step(a, x);
        if (!o.admitted) {
          refused_by_condition += o.reason.find("condition of") != std::string::npos;
          continue;
        }
        declared_acts += use_declared;
        const auto red = bh::reduce(a.rules.behaviour, x);
        readings_differ += !(a.readings.back().kind == red.act.kind && a.readings.back().fields == red.act.fields);
        reducts_refused += !chronicle::step(b, red.act).admitted;
      }
      digests_differ += chronicle::relational_digest(a) != chronicle::relational_digest(b);
    }
    s.require("on random taxonomies (" + std::to_string(trials) + " admitted of 200), every admitted act of a declared kind (" + std::to_string(declared_acts) +
                  ") has its reduct admitted as an act of the ground, and the chronicle of reducts reaches the same relational state (Proposition 14)",
              trials > 150 && declared_acts > 500 && reducts_refused == 0 && digests_differ == 0 && readings_differ == 0,
              std::to_string(reducts_refused) + " reducts refused, " + std::to_string(digests_differ) + " states differ");
    s.require("and the declared conditions only narrow: " + std::to_string(refused_by_condition) + " acts refused by a condition of their kind", refused_by_condition > 100);
  }

  // ---- 5. Proposition 15: the constitution of acts by conduct --------------------------------------------------------------------------
  {
    calx::random::Stream r(31, 5, 0, 0);
    const std::vector<std::string> ops = {"read", "write", "run tool", "call model", "message", "wait"};
    int bad = 0, admitted = 0, refused = 0, pure = 0, tamper_found = 0, stray_found = 0;
    for (int trial = 0; trial < 100; trial++) {
      State st = founded(constitutions::read(workshop_text()));
      chronicle::Conduct cd;
      for (int k = 1; k <= 30; k++) {
        bh::Episode e;
        e.party = between(r, 0, 1) ? "cy" : "sam", e.bearer = e.party, e.operation = ops[static_cast<std::size_t>(between(r, 0, 5))], e.turn = k;
        e.resources["seconds"] = between(r, 1, 600);
        e.inputs.insert(archeion::fixity::sha256_hex("input " + std::to_string(between(r, 0, 9))));
        if (between(r, 0, 2) == 0) {
          chronicle::observe(cd, e, chronicle::detail::padded(k));
          pure++;
          continue;
        }
        const std::string kind = between(r, 0, 1) ? "say" : (between(r, 0, 1) ? "assess" : "claim");
        Act x = act(e.party, kind, k, {{"id", "c" + std::to_string(trial) + "-" + std::to_string(k)}, {"object", "cy"}, {"criterion", "care"}, {"value", "good"}});
        e.kind = kind;
        const auto out = chronicle::constitute(st, cd, e, x);
        out.admitted ? admitted++ : refused++;
      }
      const auto v = chronicle::joined(st, cd);
      bad += !v.holds();
      // An outcome rewritten to name another seal, the journal sealed afresh, is found.
      archeion::fixity::Journal forged;
      bool changed = false;
      for (const auto& en : cd.journal.entries) {
        archeion::fixity::Entry x = en;
        if (!changed && x.what == "outcome" && x.about.find("seal:") != std::string::npos) x.about = x.about.substr(0, x.about.find("seal:")) + "seal:" + archeion::fixity::sha256_hex("another"), changed = true;
        x.seal.clear();
        forged.append(x);
      }
      tamper_found += chronicle::joined(st, chronicle::Conduct{forged}).fails();
      State stray = st;
      chronicle::step(stray, act("sam", "say", 31, {{"episode", archeion::fixity::sha256_hex("no such episode")}}));
      stray_found += chronicle::joined(stray, cd).fails();
    }
    s.require("on 100 random runs through the basic counts-as (" + std::to_string(admitted) + " acts admitted, " + std::to_string(refused) + " refused, " + std::to_string(pure) +
                  " episodes constituting none), every admitted act is joined to the one episode that constituted it and every refusal leaves its episode and no act (Proposition 15)",
              bad == 0 && admitted > 1000 && refused > 100 && pure > 500);
    s.require("an outcome rewritten to name another seal, and an act carrying an episode the journal does not hold, break the join, each found", tamper_found == 100 && stray_found == 100);
    State x = founded(constitutions::read(workshop_text()));
    const auto typed = chronicle::step(x, act("sam", "say", 1, {{"episode", "not a digest"}}));
    s.require("an act names the episode that constituted it by its SHA-256 digest, or is refused", !typed.admitted && typed.reason.find("SHA-256") != std::string::npos, typed.reason);
  }

  // ---- 6. Proposition 16 (G9): effects only by act or by rule -----------------------------------------------------------------------
  {
    const auto plain = constitutions::read(workshop_text());
    State one = founded(plain), two = founded(plain);
    chronicle::Conduct j1, j2;
    for (int k = 1; k <= 12; k++) {
      bh::Episode e1, e2;
      e1.party = e2.party = "cy", e1.turn = e2.turn = k, e1.operation = "write", e2.operation = "read";
      e1.resources["tokens"] = 100 * k, e2.resources["tokens"] = 7 * k;
      const Act x = act("cy", k % 2 ? "offer" : "say", k, {{"id", "o" + std::to_string(k)}, {"to", "sam"}});
      chronicle::constitute(one, j1, e1, x);
      chronicle::constitute(two, j2, e2, x);
    }
    State again;
    const auto replayed = chronicle::replay(one.parties, plain, one.acts, one.journal, &again);
    s.require("the state is given again by the acts and the turns alone, the journal withheld; and two journals of different conduct behind the same acts leave the same "
              "relational state under different seals (Proposition 16)",
              replayed.holds() && chronicle::relational_digest(again) == chronicle::relational_digest(one) && chronicle::relational_digest(one) == chronicle::relational_digest(two) &&
                  one.journal.head() != two.journal.head());
    const auto lapse = with_part(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1,
      "triggers": [{"rule": "a taking lapses", "when": {"commitment": {"matter": "catalogue", "past due by": 2}}, "then": "lapse"},
                   {"rule": "a review ends", "when": {"position": "reviewer", "held for more than": 5}, "then": "end position"}]})");
    State t = founded(lapse), u = founded(plain);
    for (State* x : {&t, &u}) {
      chronicle::step(*x, act("cy", "commit", 1, {{"id", "c1"}, {"to", "sam"}, {"matter", "catalogue"}, {"due", "3"}}));
      chronicle::step(*x, act("rae", "assess", 4, {{"object", "cy"}, {"criterion", "care"}, {"value", "good"}}));
    }
    const std::string before = chronicle::relational_digest(t);
    const auto refused = chronicle::step(t, act("dee", "assess", 6, {{"object", "cy"}, {"criterion", "care"}, {"value", "good"}}));
    const bool untouched = chronicle::relational_digest(t) == before && t.commitments.at("c1").status == commitments::Status::active;
    const auto late = chronicle::step(t, act("cy", "deliver", 6, {{"id", "c1"}, {"artefact", "late"}}));
    const auto plain_late = chronicle::step(u, act("cy", "deliver", 6, {{"id", "c1"}, {"artefact", "late"}}));
    const auto review = chronicle::step(t, act("rae", "assess", 7, {{"object", "cy"}, {"criterion", "care"}, {"value", "good"}}));
    const bool view = chronicle::settled(t, 7).commitments.at("c1").status == commitments::Status::handed_over && t.commitments.at("c1").status == commitments::Status::active;
    const auto said = chronicle::step(t, act("sam", "say", 8));
    s.require("a trigger rule lapses an active commitment more than two turns past its due and ends a review held more than five turns, by law: the late delivery and the review "
              "are judged on the settled state and refused, a refused act settles nothing, the next admitted act keeps the settlement; without the rules the late delivery is "
              "admitted",
              !refused.admitted && untouched && !late.admitted && !review.admitted && view && said.admitted && t.commitments.at("c1").status == commitments::Status::handed_over &&
                  t.lapsed.count("c1") && !t.holds("rae", "reviewer") && plain_late.admitted,
              late.reason + " / " + review.reason);
    State given;
    const auto again2 = chronicle::replay(t.parties, lapse, t.acts, t.journal, &given);
    s.require("and a replay settles the same effects by law from the record and its turns: the same relational state", again2.holds() && chronicle::relational_digest(given) == chronicle::relational_digest(t),
              again2.reason);
  }

  // ---- 7. Counts-as: an operative's reading counted as a transmission ----------------------------------------------------------------
  {
    State st = founded(decl);
    mandates::Mandate m;
    m.id = "m1", m.principal = "cy", m.holder = "cy/agent", m.kinds = {"consult", "join"}, m.from = 0, m.to = 100;
    Act g = act("cy", "mandate", 1);
    g.grant = m;
    const Act joins = act("cy/agent", "join", 1, {{"position", "contributor"}}, "m1");
    chronicle::step(st, g);
    chronicle::step(st, joins);
    const auto counted = chronicle::step(st, act("cy/agent", "consult", 2, {{"holding", "catalogue"}, {"provider", "provider-a"}}, "m1"));
    const auto sources = chronicle::step(st, act("cy/agent", "consult", 2, {{"holding", "sources"}, {"provider", "provider-a"}}, "m1"));
    State plain = founded(constitutions::read(workshop_text()));
    chronicle::step(plain, g);
    chronicle::step(plain, joins);
    const auto uncounted = chronicle::step(plain, act("cy/agent", "consult", 2, {{"holding", "catalogue"}, {"provider", "provider-a"}}, "m1"));
    s.require("a reading that carries its provider counts as a transmission to it: the catalogue, whose norm sends nothing outside, is refused, the sources, which may go "
              "anywhere, are read and the transmission recorded; with no such rule the same reading is admitted",
              !counted.admitted && counted.reason.find("provider-a") != std::string::npos && sources.admitted && st.accesses.back().transmit == "provider-a" && uncounted.admitted,
              counted.reason);
  }

  // ---- 8. Categories, omissions, nearness, standing ---------------------------------------------------------------------------------
  {
    const bh::Part& t = decl.behaviour;
    bh::Episode read30, read10, bench;
    read30.party = "cy", read30.operation = "read", read30.features["pages read"] = "30", read30.turn = 1;
    read10 = read30, read10.features["pages read"] = "10", read10.turn = 2;
    bench.party = "cy", bench.operation = "run tool", bench.holdings = {"bench"}, bench.turn = 3;
    const auto c30 = bh::categorise(t, read30), c10 = bh::categorise(t, read10), cb = bh::categorise(t, bench);
    s.require("episodes lie in the categories their recorded features decide: a reading of 30 pages is a reading and a long one, of 10 pages only a reading, a run of the bench a "
              "bench run; an analytic category decides nothing",
              c30 == std::vector<std::string>{"a reading", "long reading"} && c10 == std::vector<std::string>{"a reading"} && cb == std::vector<std::string>{"bench run"});
    s.require("of the community's categories, the one reading only what acts carry is read by the fold, the one reading the journal's features is not, the analytic one never",
              bh::fold_readable(t.category("provider reading")->where) && !bh::fold_readable(t.category("long reading")->where) && t.category("a likely draft")->analytic);
    const std::vector<bh::Episode> journal = {read10, read30, bench};
    const auto missed = bh::omissions(t, journal, "a reading", "bench run", "cy", 1);
    s.require("a reading opens a window of one turn in which a bench run is owed: the reading at turn 2 is answered at turn 3, the one at turn 1 is not, and is the omission",
              missed == std::vector<long long>{1});
    calx::random::Stream r(37, 8, 0, 0);
    int broken = 0;
    for (int k = 0; k < 300; k++) {
      std::vector<bh::Episode> e(3);
      for (auto& x : e) {
        x.operation = t.features.front().vocabulary[static_cast<std::size_t>(between(r, 0, 8))];
        x.resources["tokens"] = between(r, 0, 300000), x.resources["seconds"] = between(r, 0, 9000);
        for (int i = 0; i < 3; i++)
          if (between(r, 0, 1)) x.inputs.insert("d" + std::to_string(between(r, 0, 5)));
        x.features["pages read"] = std::to_string(between(r, 0, 100));
      }
      bh::Part maxed = t;
      maxed.nearness = "max";
      for (const bh::Part* p : std::vector<const bh::Part*>{&t, &maxed}) {
        const double ab = bh::distance(*p, e[0], e[1]), ba = bh::distance(*p, e[1], e[0]), bc = bh::distance(*p, e[1], e[2]), ac = bh::distance(*p, e[0], e[2]);
        broken += bh::distance(*p, e[0], e[0]) != 0 || std::abs(ab - ba) > 1e-12 || ac > ab + bc + 1e-9;
      }
    }
    s.require("nearness of conduct is a pseudometric, summed or taken as the greatest: nought on itself, symmetric, and within the triangle (300 random triples)", broken == 0);
    const bh::Standard st = t.recognition.front();
    const std::vector<bh::Observation> one = {{"rae", "rae", "cy wrote entry 7", "B", "yes"}};
    const std::vector<bh::Observation> self = {{"cy", "cy", "cy wrote entry 7", "A", "yes"}};
    const std::vector<bh::Observation> two = {{"rae", "rae", "cy wrote entry 7", "A", "yes"}, {"sam", "sam", "cy wrote entry 7", "B", "no"}};
    const std::vector<bh::Observation> memory = {{"a model", "provider-a", "cy wrote entry 7", "C", "yes"}};
    s.require("authorship stands established when another origin attests it at grade B, attested only when the author attests it alone, disputed when two that meet the "
              "standard disagree, proposed on a model's word",
              bh::standing(one, st, "cy") == "established" && bh::standing(self, st, "cy") == "attested" && bh::standing(two, st, "cy") == "disputed" &&
                  bh::standing(memory, st, "cy") == "proposed");
  }

  // ---- 9. Proposition 17: revisions -------------------------------------------------------------------------------------------------
  {
    const bh::Part& old_t = decl.behaviour;
    const std::string v2 = replaced(replaced(replaced(replaced(kPart, R"("version": 1)", R"("version": 2, "revises": {"from": 1, "kinds": {"objection": "plea"}})"), R"("kind": "objection")",
                                                      R"("kind": "plea")"),
                                             R"("counts as": "objection")", R"("counts as": "plea")"),
                                    "pages-read above 20", "pages-read above 10");
    const bh::Part new_t = bh::in_force(bh::read_part(v2), true);
    const auto direct = bh::revision_faults(old_t, new_t);
    const auto categorical = bh::admit_revision(old_t, new_t);
    s.require("a revision renaming the objection a plea and widening the long reading is admitted, directly and by kategoria (the map of kinds a functor keeping every kind "
              "of the ground)",
              direct.empty() && categorical.admitted(), direct.empty() ? categorical.verdict.reason : direct.front());
    const auto record = bh::record_of(old_t, c.acts);
    const auto carried = bh::carry(record, old_t, new_t);
    const auto lan = bh::carried_as_extension(record, old_t, new_t);
    int pleas = 0;
    for (const auto& f : carried.record.facts) pleas += f.kind == "plea";
    s.require("the community's record carried along it: the objection is now a plea, and the carried record is the left Kan extension of the record along the map of kinds",
              pleas == 1 && carried.problems.empty() && lan.holds(), lan.reason);
    const bh::Part moved = bh::in_force(bh::read_part(replaced(v2, R"("counts as": "claim")", R"("counts as": "request")")), true);
    const bh::Part broken = bh::in_force(bh::read_part(replaced(v2, R"("counts as": "plea")", R"("counts as": "claim")")), true);
    const bh::Part narrowed = bh::in_force(bh::read_part(replaced(v2, "pages-read above 10", "pages-read above 30")), true);
    const auto f1 = bh::revision_faults(old_t, moved), f2 = bh::revision_faults(old_t, broken), f3 = bh::revision_faults(old_t, narrowed);
    auto has = [](const std::vector<std::string>& v, const std::string& part) { return std::any_of(v.begin(), v.end(), [&](const std::string& x) { return x.find(part) != std::string::npos; }); };
    s.require("revisions are refused that move a kind to another kind of the ground, break the order counts as, or narrow a category, and kategoria refuses the first two",
              has(f1, "reduces to claim, and its image plea to request") && has(f2, "no functor of the order counts as") && has(f3, "not shown contained") &&
                  !bh::admit_revision(old_t, moved).admitted() && !bh::admit_revision(old_t, broken).admitted());
    // The direct decision and kategoria's agree on random revisions of kinds.
    calx::random::Stream r(43, 9, 0, 0);
    int agree = 0, total = 0, admitted = 0;
    for (int k = 0; k < 60; k++) {
      const std::vector<std::string> onto = {"claim", "plea", "objection", "request", "appeal"};
      std::string next = replaced(kPart, R"("version": 1)", R"("version": 2, "revises": {"from": 1, "kinds": {"objection": "plea"}})");
      next = replaced(next, R"("kind": "objection")", R"("kind": "plea")");
      next = replaced(next, R"("counts as": "claim")", "\"counts as\": \"" + onto[static_cast<std::size_t>(between(r, 0, 1)) * 3] + "\"");
      next = replaced(next, R"("counts as": "objection")", "\"counts as\": \"" + onto[static_cast<std::size_t>(between(r, 0, 1))] + "\"");
      const bh::Part n = bh::in_force(bh::read_part(next), true);
      if (!bh::faults(n).empty()) continue;
      total++;
      const bool d = bh::revision_faults(old_t, n).empty(), cat = bh::admit_revision(old_t, n).admitted();
      agree += d == cat;
      admitted += d;
    }
    s.require("on random revisions of the declared kinds (" + std::to_string(total) + "), the fold's direct decision and kategoria's agree every time (" + std::to_string(admitted) + " admitted)",
              total > 30 && agree == total && admitted > 0 && admitted < total);
  }

  // ---- 10. A community revises its taxonomy by its own procedure ------------------------------------------------------------------
  {
    State st = founded(decl);
    const auto before = chronicle::step(st, act("cy", "objection", 1, {{"id", "k1"}, {"about", "entry 7"}}));
    const std::string v2 = replaced(replaced(replaced(kPart, R"("version": 1)", R"("version": 2, "revises": {"from": 1, "kinds": {"objection": "plea"}})"), R"("kind": "objection")", R"("kind": "plea")"),
                                    R"("counts as": "objection")", R"("counts as": "plea")");
    chronicle::step(st, act("sam", "propose", 2, {{"id", "p1"}, {"change.behaviour", v2}}));
    for (const char* v : {"sam", "rae", "cy"}) chronicle::step(st, act(v, "vote", 2, {{"id", "p1"}, {"value", "yes"}}));
    const std::string claims_before = std::to_string(st.claims.size());
    const auto amended = chronicle::step(st, act("sam", "amend", 3, {{"id", "p1"}}));
    const auto gone = chronicle::step(st, act("cy", "objection", 4, {{"id", "k2"}, {"about", "entry 8"}}));
    const auto plea = chronicle::step(st, act("cy", "plea", 4, {{"id", "k3"}, {"about", "entry 8"}}));
    const auto replayed = chronicle::replay(st.parties, decl, st.acts, st.journal);
    s.require("the assembly adopts the revision by its own rule; afterwards an objection is no kind in force and a plea is a claim, the record replaying with each act judged "
              "under the taxonomy in force when it was admitted, the claims untouched by the revision itself",
              before.admitted && amended.admitted && st.rules.behaviour.version == 2 && !gone.admitted && plea.admitted && replayed.holds() && claims_before == "1",
              amended.reason + " / " + gone.reason + " / " + replayed.reason);
    State bad = founded(decl);
    chronicle::step(bad, act("sam", "propose", 2, {{"id", "p1"}, {"change.behaviour", replaced(v2, "pages-read above 20", "pages-read above 30")}}));
    for (const char* v : {"sam", "rae", "cy"}) chronicle::step(bad, act(v, "vote", 2, {{"id", "p1"}, {"value", "yes"}}));
    const auto refused = chronicle::step(bad, act("sam", "amend", 3, {{"id", "p1"}}));
    s.require("a revision that narrows a category is refused though the assembly adopted it", !refused.admitted && refused.reason.find("not shown contained") != std::string::npos, refused.reason);
  }
  return s.done();
}
