// Checks of the taxonomy in levels (decision 0010): the base's second version is a revision of the first, admitted by the direct
// decision and by kategoria, its map sending normative acts to the constituted acts of level N, conduct to level O and observations to
// level M and fixing every kind of the ground; revisions into levels that move a root elsewhere, rename another root, or lose a level are
// refused; on 100 random records of the first version, each carried along the revision loses nothing, fits the second and is the left
// Kan extension of the record along the map of kinds (Proposition 22); a diplomatic protest described at five levels, by two
// structures at S, its descriptions joined by bridges, fits the taxonomy in kategoria's terms and is found as two instances of the
// practice protest; descriptions and taxonomies at fault are found; no rule whose condition reads epistemic behaviour alone is admitted,
// directly, by amendment, or on 300 random conditions (Proposition 23, invariant G10); and a chronicle and its behaviour journal described
// at levels, one occurrence at E, O, M and N at once, each act through an agent found as an instance of its practice.

#include <thesmos/behaviour/levels.hpp>
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
namespace ko = kategoria::ontologies;
namespace ch = thesmos::chronicle;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
const std::string& workshop_text() {
  static const std::string t = slurp(std::string(THESMOS_DATA) + "/examples/workshop.json");
  return t;
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
bool has(const std::vector<std::string>& faults, const std::string& text) {
  for (const auto& f : faults)
    if (f.find(text) != std::string::npos) return true;
  return false;
}
std::string first(const std::vector<std::string>& v) { return v.empty() ? std::string() : v.front(); }
bh::Part with(const std::string& part) { return bh::in_force(bh::read_part(std::string_view(part)), true); }
}  // namespace

int main() {
  calx::verification::Suite s;
  const bh::Part& v1 = bh::first_base();
  const bh::Part& v2 = bh::second_base();

  // ---- 1. The second version, a revision of the first ---------------------------------------------------------------------------------
  {
    const auto direct = bh::revision_faults(v1, v2);
    const auto by_kategoria = bh::admit_revision(v1, v2);
    bool renamed = false;
    for (const auto& c : by_kategoria.changes) renamed = renamed || c == "the kind normative_act becomes constituted_act";
    s.require("the base's second version is a revision of the first, admitted directly and by kategoria: the map of kinds a functor of their orders, keeping registers and every "
              "kind of the ground, normative acts sent to the constituted acts of N, conduct to O, observations to M",
              direct.empty() && by_kategoria.admitted() && renamed && bh::image(v2, "conduct") == "ontological conduct" && bh::image(v2, "observation") == "manifest conduct",
              first(direct) + " / " + by_kategoria.verdict.reason);
    const auto v = bh::verify(v2);
    const auto printed = ko::print(bh::ontology_of(v2));
    const auto back = ko::read(printed);
    s.require("kategoria admits the second version as an ontology with one root, five levels, six facets as axes and the nine bridges as thirteen relations between levels, and "
              "printed in kategoria's language it reads back to itself",
              v.admitted() && back.ok() && back.ontology == bh::ontology_of(v2) && bh::ontology_of(v2).relations.size() == 13 && v2.levels.size() == 5 && v2.facets.size() == 6,
              v.verdict.reason);
    bh::Part elsewhere = v2, renamed_event = v2, short_of_s = v2;
    elsewhere.revised_kinds["conduct"] = "social conduct";
    renamed_event.revised_kinds["event"] = "occurrence";
    short_of_s.levels.erase(short_of_s.levels.begin() + 3);
    const auto a = bh::revision_faults(v1, elsewhere), b = bh::revision_faults(v1, renamed_event), c = bh::revision_faults(v1, short_of_s);
    const bool k1 = !bh::admit_revision(v1, elsewhere).admitted();
    s.require("revisions into levels are refused that send conduct to social conduct (kategoria too: its register would change), rename an event, or lose the level S",
              has(a, "recasts the conduct") && k1 && has(b, "renames no root") && has(c, "lacks S"), first(a) + " / " + first(b) + " / " + first(c));
    // A community's revision under the second version renames no level.
    const bh::Part old = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2"})");
    bh::Part next = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 2, "extends": "base 2", "revises": {"from": 1, "kinds": {"ontological conduct": "doing"}}})");
    s.require("a community's revision may not rename a level, which only the base's revision into levels recasts", has(bh::revision_faults(old, next), "renames the root or level"),
              first(bh::revision_faults(old, next)));
  }

  // ---- 2. Proposition 22: records of the first version carried along the revision -----------------------------------------------------
  {
    calx::random::Stream r(20261127, 20, 0, 0);
    const auto o1 = bh::ontology_of(v1), o2 = bh::ontology_of(v2);
    int fits = 0, carried = 0, extension = 0, renamed = 0;
    std::vector<std::string> ops;
    for (const auto& v : v1.feature("operation")->vocabulary) ops.push_back(bh::ident(v));
    const std::vector<std::string> grades = {"A", "B", "C"};
    for (int trial = 0; trial < 100; trial++) {
      ko::Record rec;
      const int n = 5 + static_cast<int>(r.below(20));
      for (int k = 0; k < n; k++) {
        ko::Fact f;
        f.id = "fact" + std::to_string(k), f.text = f.id;
        switch (r.below(6)) {
          case 0:
          case 1: {
            const auto& p = v1.primitives[r.below(v1.primitives.size())];
            f.kind = bh::ident(p.kind), f.axes["force"] = std::string(acts::word(p.force));
            break;
          }
          case 2: {
            const std::vector<std::string> kinds = {"conduct", "bench_run", "heavy_model_call", "a_reading"};
            f.kind = kinds[r.below(kinds.size())];
            if (r.below(2)) f.axes["operation"] = ops[r.below(ops.size())];
            break;
          }
          case 3: f.kind = "observation", f.axes["grade"] = grades[r.below(3)]; break;
          case 4: f.kind = r.below(2) ? "event" : "measured_state"; break;
          default: f.kind = "notice", f.axes["force"] = "assertive";
        }
        rec.facts.push_back(std::move(f));
      }
      fits += ko::fits(o1, rec).holds();
      const auto m = bh::carry(rec, v1, v2);
      carried += m.lost.empty() && m.problems.empty();
      renamed += static_cast<int>(m.kept.size());
      extension += bh::carried_as_extension(rec, v1, v2).holds();
    }
    s.require("on 100 random records of the first version (acts with their force, conduct in its categories, observations with their grade, events and states), each fits the "
                  "first, is carried along the revision losing nothing and fitting the second (" + std::to_string(renamed) + " facts recast), and is the left Kan extension of "
                  "the record along the map of kinds (Proposition 22)",
              fits == 100 && carried == 100 && extension == 100 && renamed > 0);
  }

  // ---- 3. A protest described at five levels ------------------------------------------------------------------------------------------
  const bh::Part t = with(R"({"format": "noetarium.behaviour/1", "name": "basin", "version": 1, "extends": "base 2",
    "kinds": [{"kind": "protest", "force": "expressive", "counts as": "say"}]})");
  bh::Descriptions ds;
  ds.items = {{"instructions", "protest 1", "E", "", "", "", "", "", {{"stage", "formation of intent"}, {"intention", "intended"}, {"scope", "directed at a structure"}}},
              {"note", "protest 1", "O", "", "", "", "", "", {{"stage", "execution"}, {"modality", "communicative"}, {"agency", "collective"}}},
              {"text", "protest 1", "M", "", "", "A", "", "the note's digest", {{"modality", "communicative"}}},
              {"reading by the other side", "protest 1", "S", "", "the other side", "", "", "", {{"intention", "intended"}}},
              {"reading by its public", "protest 1", "S", "", "its public", "", "", "", {{"intention", "foreseen"}}},
              {"formal protest", "protest 1", "N", "protest", "the law of the basin", "", "", "", {{"stage", "completion"}}},
              {"acquiescence prevented", "protest 1", "N", "qualified conduct", "the law of the basin", "", "a protest preserving the right", "", {}}};
  ds.links = {{"enactment", "instructions", "note"},
              {"manifestation", "note", "text"},
              {"social attribution", "text", "reading by the other side"},
              {"social attribution", "text", "reading by its public"},
              {"codified counts-as", "note", "formal protest"},
              {"recognition by evidence", "text", "acquiescence prevented"},
              {"feedback", "formal protest", "reading by its public"}};
  {
    const auto f = bh::faults(t, ds);
    const auto fit = ko::fits(bh::ontology_of(t), bh::record_of(t, ds));
    const auto found = bh::instances(t, ds, *t.practice("protest"));
    bool whole = found.size() == 2;
    for (const auto& i : found) whole = whole && i.size() == 5 && i.at("note") == "note" && i.at("formal protest") == "formal protest";
    s.require("a diplomatic protest described at E, O, M, S and N, read by two structures at S and qualified at N, has no fault, fits the taxonomy in kategoria's terms, and is two "
              "instances of the practice protest, one for each reading",
              f.empty() && fit.holds() && whole && bh::levels_of(ds, "protest 1") == std::set<std::string>{"E", "M", "N", "O", "S"}, first(f) + " / " + fit.reason);
  }
  {
    auto broken = [&](auto change) {
      bh::Descriptions d = ds;
      change(d);
      return bh::faults(t, d);
    };
    const auto no_structure = broken([](bh::Descriptions& d) { d.items[3].structure.clear(); });
    const auto modality_at_e = broken([](bh::Descriptions& d) { d.items[0].facets["modality"] = "physical"; });
    const auto formation_at_n = broken([](bh::Descriptions& d) { d.items[5].facets["stage"] = "formation of intent"; });
    const auto execution_at_e = broken([](bh::Descriptions& d) { d.items[0].facets["stage"] = "execution"; });
    const auto ungraded = broken([](bh::Descriptions& d) { d.items[2].grade.clear(); });
    const auto unlabelled = broken([](bh::Descriptions& d) { d.items[6].label.clear(); });
    const auto kindless = broken([](bh::Descriptions& d) { d.items[5].kind = "constituted act"; });
    const auto wrong_link = broken([](bh::Descriptions& d) { d.links.push_back({"enactment", "note", "formal protest"}); });
    bh::Descriptions misjoined = ds;
    misjoined.links.push_back({"enactment", "note", "formal protest"});
    const auto kfit = ko::fits(bh::ontology_of(t), bh::record_of(t, misjoined));
    s.require("faults found: a social description naming no structure, modality said of epistemic behaviour, a normative description at the formation of intent, epistemic "
              "behaviour at execution, a manifestation without its grade, a qualification without its label, a constituted act naming no kind, an enactment from O to N, "
              "which kategoria refuses too",
              has(no_structure, "names no structure") && has(modality_at_e, "does not describe level E") && has(formation_at_n, "formation of intent, which is epistemic") &&
                  has(execution_at_e, "epistemic and at the stage") && has(ungraded, "no grade") && has(unlabelled, "no label") && has(kindless, "names no kind of act") &&
                  has(wrong_link, "does not join") && !kfit.holds(),
              kfit.reason);
  }

  // ---- 4. Taxonomies in levels at fault ------------------------------------------------------------------------------------------------
  {
    bh::Part misled = v2;
    misled.practices[0].links.push_back({"enactment", "text", "note"});
    bh::Part feature_at_s = v2;
    feature_at_s.features.push_back({"esteem", "categorical", "", {"low", "high"}, 1, 1, "a survey", "C", "S"});
    const bh::Part levelled = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "levels": [{"level": "X", "name": "extra", "register": "Relational"}]})");
    const bh::Part petition = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "practices": [{"practice": "petition", "slots": [{"role": "request", "level": "O"}, {"role": "record", "level": "M"}, {"role": "standing", "level": "S", "optional": true},
                                                          {"role": "ruling", "level": "N"}],
                     "links": [{"bridge": "manifestation", "from": "request", "to": "record"}, {"bridge": "social attribution", "from": "record", "to": "standing"},
                               {"bridge": "recognition by evidence", "from": "record", "to": "ruling"}]}]})");
    s.require("faults found: a practice linking a manifestation to a conduct by enactment, a feature of social conduct, and a community declaring a level of its own; a "
              "community's own practice admitted beside the base's",
              has(bh::faults(misled), "does not join those levels") && has(bh::faults(feature_at_s), "features describe conduct (O) or epistemic behaviour (E)") &&
                  has(bh::faults(levelled), "which are the base's") && bh::faults(petition).empty() && petition.practice("petition") && petition.practice("protest"),
              first(bh::faults(petition)));
  }

  // ---- 5. Proposition 23: no effect attaches to epistemic behaviour alone (G10) ----------------------------------------------------------
  {
    const auto epistemic = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "conduct": [{"category": "a planned reading", "where": "field.epistemic-kind is 'plan'"}],
      "counts as": [{"rule": "a plan reaches the provider", "conduct": "a planned reading", "as": {"transmission": "field.provider"}}]})");
    const auto mixed = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "conduct": [{"category": "a planned reading", "where": "kind is 'consult' and field.epistemic-kind is 'plan'"}],
      "counts as": [{"rule": "a plan reaches the provider", "conduct": "a planned reading", "as": {"transmission": "field.provider"}}]})");
    const auto confession = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "kinds": [{"kind": "confession", "force": "expressive", "counts as": "say", "valid when": ["field.epistemic-kind is present"]}]})");
    const auto account = with(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "kinds": [{"kind": "account", "force": "expressive", "counts as": "say", "valid when": ["field.about is present and field.epistemic-kind is present"]}]})");
    auto c = constitutions::read(workshop_text());
    const std::string by_amendment = chronicle::detail::change_rules(c, {{"behaviour", R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2",
      "conduct": [{"category": "a planned reading", "where": "field.epistemic-kind is 'plan'"}],
      "counts as": [{"rule": "a plan reaches the provider", "conduct": "a planned reading", "as": {"transmission": "field.provider"}}]})"}});
    s.require("refused: a counts-as rule through a category that reads epistemic behaviour alone, and a declared kind whose condition reads it alone, directly and by amendment; "
              "admitted: the same rule and kind when their conditions read conduct too, so that intent qualifies conduct and never stands for it",
              has(bh::faults(epistemic), "epistemic behaviour alone") && bh::faults(mixed).empty() && has(bh::faults(confession), "epistemic behaviour alone") && bh::faults(account).empty() &&
                  by_amendment.find("epistemic behaviour alone") != std::string::npos,
              first(bh::faults(mixed)) + " / " + first(bh::faults(account)) + " / " + by_amendment);
    calx::random::Stream r(20261127, 21, 0, 0);
    const std::vector<std::pair<std::string, bool>> atoms = {{"field.epistemic-kind is 'plan'", true},    {"field.disclosed-under is present", true},
                                                             {"epistemic-kind is 'reasoning'", true},      {"kind is 'consult'", false},
                                                             {"field.about is present", false},            {"operation is 'read'", false},
                                                             {"author holds 'steward'", false},            {"tokens above 10", false}};
    int agree = 0, refused = 0;
    for (int trial = 0; trial < 300; trial++) {
      std::string where;
      bool all_e = true, carried = true;
      const int n = 1 + static_cast<int>(r.below(3));
      for (int k = 0; k < n; k++) {
        const auto& [text, e] = atoms[r.below(atoms.size())];
        where += (k ? " and " : "") + text;
        all_e = all_e && e;
        carried = carried && (text.rfind("field.", 0) == 0 || text.rfind("kind ", 0) == 0 || text.rfind("author holds", 0) == 0);
      }
      const auto p = bh::read_predicate(where);
      const bool classified = bh::epistemic_only(p, v2) == all_e;
      bool admitted_right = true;
      if (carried) {
        const auto part = with(std::string(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 2", "conduct": [{"category": "c", "where": ")") + where +
                               R"("}], "counts as": [{"rule": "r", "conduct": "c", "as": {"transmission": "field.provider"}}]})");
        const bool g10 = has(bh::faults(part), "epistemic behaviour alone");
        admitted_right = g10 == all_e;
        refused += g10;
      }
      agree += classified && admitted_right;
    }
    s.require("on 300 random conditions over epistemic, conduct and normative features, a condition reads epistemic behaviour alone exactly when every atom does, and a counts-as "
                  "rule through it is refused exactly then (" + std::to_string(refused) + " refused; Proposition 23)",
              agree == 300 && refused > 10);
  }

  // ---- 6. A chronicle and its journal described at levels -------------------------------------------------------------------------------
  {
    auto doc = symploke::jsonld::parse(workshop_text());
    doc.get("choice")->items.push_back(symploke::jsonld::parse(R"({"who": "steward", "may": ["agree"]})"));
    Parties ps;
    for (const char* p : {"sam", "rae", "ana"}) ps.add({p, Kind::person});
    ps.add({"ana/scribe", Kind::operative, "ana", "provider-a"});
    State st(ps, constitutions::read(symploke::jsonld::dump(doc)));
    ch::Conduct journal;
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}, {"rae", "reviewer"}};
    ch::step(st, f);
    Act m = act("ana", "mandate", 1);
    mandates::Mandate grant;
    grant.id = "m-scribe", grant.principal = "ana", grant.holder = "ana/scribe", grant.kinds = {"say"}, grant.from = 0, grant.to = 100;
    m.grant = grant;
    ch::step(st, m);
    const std::string terms = R"({"format": "noetarium.disclosure/1", "id": "the scribe's thinking", "principal": "ana", "agents": ["ana/scribe"],
      "scope": {"kinds": ["reasoning", "model call"]}, "purpose": {"admits": ["review"]}, "custody": {"holding": "thinking", "custodian": "steward", "retention": 5},
      "access": {"principal": true}, "withdrawal": {"notice": 1}})";
    ch::step(st, act("ana", "agree", 2, {{"agreement", "the scribe's thinking"}, {"for", "principal"}, {"terms", terms}}));
    ch::step(st, act("sam", "agree", 2, {{"agreement", "the scribe's thinking"}, {"for", "community"}, {"terms", terms}}));
    bh::Episode e;
    e.party = "ana/scribe", e.bearer = "ana", e.provider = "provider-a", e.operation = "message", e.kind = "say", e.turn = 3;
    ch::Epistemic thought;
    thought.party = "ana/scribe", thought.kind = "reasoning", thought.turn = 3, thought.content = "say that the catalogue is ready", thought.size = 30;
    const auto d1 = ch::disclose(st, journal, thought, "000000000003");
    const auto c1 = ch::constitute(st, journal, e, act("ana/scribe", "say", 3, {}, "m-scribe"));
    thought.kind = "plan", thought.turn = 4;
    e.turn = 4;
    const auto d2 = ch::disclose(st, journal, thought, "000000000004");
    const auto c2 = ch::constitute(st, journal, e, act("ana/scribe", "say", 4, {}, "m-scribe"));
    e.operation = "read", e.kind = "", e.turn = 5;
    ch::observe(journal, e, "000000000005");
    const auto ds2 = ch::describe(st, journal);
    const auto& tt = st.rules.behaviour;
    const auto f2 = bh::faults(tt, ds2);
    const auto fit = ko::fits(bh::ontology_of(tt), bh::record_of(tt, ds2));
    const auto found = bh::instances(tt, ds2, *tt.practice("act through an agent"));
    std::size_t with_reasoning = 0;
    std::set<std::string> occurrences;
    for (const auto& i : found) {
      with_reasoning += i.count("reasoning");
      occurrences.insert(ds2.find(i.at("act"))->occurrence);
    }
    const std::string first_occ = ds2.find(found.empty() ? "" : found.front().at("act")) ? ds2.find(found.front().at("act"))->occurrence : "";
    s.require("the chronicle and its journal described at levels have no fault and fit the taxonomy in kategoria's terms; each of the two acts through the agent is an instance of "
              "the practice, the first with the reasoning disclosed under the agreement, the second's plan undisclosed and so at M alone; the first occurrence is described at "
              "E, O, M and N at once",
              d1.recorded && c1.admitted && !d2.recorded && c2.admitted && f2.empty() && fit.holds() && found.size() == 2 && with_reasoning == 1 && occurrences.size() == 2 &&
                  bh::levels_of(ds2, first_occ) == std::set<std::string>{"E", "M", "N", "O"},
              first(f2) + " / " + fit.reason + " / " + std::to_string(found.size()));
    const auto joined = ch::joined(st, journal);
    const auto kept = ch::disclosed_by_agreement(st, journal);
    s.require("with epistemic entries among them, the chronicle and the journal stay joined act to episode (Proposition 15), and the journal keeps epistemic behaviour only by "
              "agreement (Proposition 24)",
              joined.holds() && kept.holds(), joined.reason + " / " + kept.reason);
  }

  return s.done();
}
