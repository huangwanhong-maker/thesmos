// Checks of transmission by operatives (decision 0013): the base taxonomy's fifth version, a revision of the fourth along which records
// carry (Proposition 37); a community that declares that operatives transmit to their provider: an operative's reading reaches its
// provider and is admitted when the norm names it, refused with the reason when it does not; a provider declared local reaches nothing;
// persons, and operatives with no declared provider, are unaffected; a permission silent on transmission permits no operative's reading;
// a service the act declares and the provider reached must both be covered; a prohibition naming a provider refuses that provider's
// agents alone; the option off leaves decision 0006's verdicts as they were; a part that does not extend the base keeps the rule; a
// counts-as rule under an unknown declaration is a fault; the briefing shows what a party's readings reach; and the record replays.

#include <thesmos/behaviour/ontology.hpp>
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
namespace bh = thesmos::behaviour;
namespace ch = thesmos::chronicle;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
// The invented workshop with two holdings of drafts and notes, read by members with transmission to the providers named, and the option
// that operatives transmit to their provider on or off; a part behaviour of its own when asked.
constitutions::Constitution host(bool transmit, const std::string& part = {}) {
  auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  doc.get("holdings")->set("drafts", symploke::jsonld::parse(R"({"kind": "document", "custodian": "steward", "visibility": "members"})"));
  doc.get("holdings")->set("notes", symploke::jsonld::parse(R"({"kind": "document", "custodian": "steward", "visibility": "members"})"));
  for (const char* n : {R"({"id": "drafts read by members", "who": "any position", "may": "consult", "object": "drafts", "transmit": ["provider-a", "provider-b"]})",
                        R"({"id": "notes read anywhere", "who": "any position", "may": "consult", "object": "notes", "transmit": "any"})",
                        R"({"id": "notes never to provider-c", "who": "any position", "must not": "consult", "object": "notes", "transmit": ["provider-c"]})"})
    doc.get("access")->get("norms")->items.push_back(symploke::jsonld::parse(n));
  if (transmit) doc.get("access")->set("operatives transmit to their provider", symploke::jsonld::Json::boolean(true));
  if (!part.empty()) doc.set("behaviour", symploke::jsonld::parse(part));
  return constitutions::read(symploke::jsonld::dump(doc));
}
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "ana", "bea", "cal"}) ps.add({p, Kind::person});
  ps.add({"a-op", Kind::operative, "ana", "provider-a"});
  ps.add({"c-op", Kind::operative, "cal", "provider-c"});
  ps.add({"l-op", Kind::operative, "bea", "local"});
  ps.add({"n-op", Kind::operative, "bea"});
  return ps;
}
State founded(const constitutions::Constitution& c) {
  State s(parties(), c);
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"ana", "contributor"}};
  ch::step(s, f);
  for (const auto& [principal, op] : std::vector<std::pair<std::string, std::string>>{{"ana", "a-op"}, {"cal", "c-op"}, {"bea", "l-op"}, {"bea", "n-op"}}) {
    Act m = act(principal, "mandate", 0);
    mandates::Mandate g;
    g.id = "m-" + op, g.principal = principal, g.holder = op, g.from = 0, g.to = 1000, g.kinds = {"join", "consult", "use"};
    m.grant = g;
    ch::step(s, m);
    ch::step(s, act(op, "join", 0, {{"position", "contributor"}}, "m-" + op));
  }
  return s;
}
Act reads(const std::string& who, const std::string& holding, long long turn, std::map<std::string, std::string> more = {}) {
  more["holding"] = holding;
  return act(who, "consult", turn, more, who.size() > 3 && who.substr(who.size() - 3) == "-op" ? "m-" + who : std::string());
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. The fifth version ----
  {
    const auto direct = bh::revision_faults(bh::fourth_base(), bh::base());
    const auto by_kategoria = bh::admit_revision(bh::fourth_base(), bh::base());
    calx::random::Stream r(20261127, 37, 0, 0);
    int carried = 0;
    const auto& v4 = bh::fourth_base();
    for (int trial = 0; trial < 50; trial++) {
      kategoria::ontologies::Record rec;
      for (int k = 0; k < 12; k++) {
        kategoria::ontologies::Fact f;
        f.id = "fact" + std::to_string(k), f.text = f.id;
        if (r.below(4) == 0) f.kind = "question", f.axes["force"] = "directive";
        else {
          const auto& p = v4.primitives[r.below(v4.primitives.size())];
          f.kind = bh::ident(p.kind), f.axes["force"] = std::string(acts::word(p.force));
        }
        rec.facts.push_back(std::move(f));
      }
      const auto m = bh::carry(rec, v4, bh::base());
      carried += m.lost.empty() && m.problems.empty() && bh::carried_as_extension(rec, v4, bh::base()).holds();
    }
    const bh::CountsAs* rule = bh::base().counts_as.empty() ? nullptr : &bh::base().counts_as.front();
    s.require("the base's fifth version, with the author's kind and provider carried, the category of a reading by an operative and the counts-as rule to its provider "
              "under a declaration, is a revision of the fourth admitted directly and by kategoria, renaming nothing; 50 random records of the fourth carry to it as "
              "the left Kan extension (Proposition 37)",
              direct.empty() && by_kategoria.admitted() && carried == 50 && rule && rule->where_declared == "operatives transmit to their provider" &&
                  bh::base().category("reading by an operative"),
              (direct.empty() ? std::string() : direct.front()) + " / " + by_kategoria.verdict.reason);
  }

  // ---- 2. Readings that reach their providers ----
  State st = founded(host(true));
  {
    const auto a = ch::step(st, reads("a-op", "drafts", 1));
    const std::string reached = st.accesses.empty() ? std::string() : st.accesses.back().reached;
    const auto c = ch::step(st, reads("c-op", "drafts", 1));
    s.require("where the community declares that operatives transmit to their provider, an agent of provider-a reads the drafts, its reading reaching provider-a, which the "
              "norm names; an agent of provider-c is refused, its reading reaching a provider outside those the norm permits",
              a.admitted && reached == "provider-a" && !c.admitted && has(c.reason, "read by an operative, it would reach its provider, provider-c, outside those the norm permits"),
              a.reason + " / " + c.reason);
    const auto l = ch::step(st, reads("l-op", "drafts", 1));
    const std::string local = l.admitted ? st.accesses.back().reached : "?";
    const auto n = ch::step(st, reads("n-op", "drafts", 1));
    const auto p = ch::step(st, reads("ana", "drafts", 1));
    s.require("an agent whose provider is declared local reaches nothing and reads; so do an agent with no declared provider and a person",
              l.admitted && local.empty() && n.admitted && p.admitted, l.reason + " / " + n.reason + " / " + p.reason);
    const auto silent = ch::step(st, reads("a-op", "catalogue", 1));
    const auto person = ch::step(st, reads("ana", "catalogue", 1));
    s.require("a permission silent on transmission permits no operative's reading once operatives transmit to their provider, and a person's as before",
              !silent.admitted && has(silent.reason, "outside the community, which the norm does not permit") && person.admitted, silent.reason);
    const auto both = ch::step(st, reads("a-op", "drafts", 2, {{"transmit", "provider-b"}}));
    const auto elsewhere = ch::step(st, reads("a-op", "drafts", 2, {{"transmit", "a search service"}}));
    s.require("a service the act declares and the provider its reading reaches must both be covered: provider-b and provider-a are, a search service is not",
              both.admitted && !elsewhere.admitted && has(elsewhere.reason, "a search service"), both.reason + " / " + elsewhere.reason);
    const auto na = ch::step(st, reads("a-op", "notes", 2)), nc = ch::step(st, reads("c-op", "notes", 2)), np = ch::step(st, reads("ana", "notes", 2));
    s.require("a prohibition naming provider-c, at the specificity of a permission to send anywhere, refuses provider-c's agent alone: provider-a's agent and a person read",
              na.admitted && !nc.admitted && has(nc.reason, "notes never to provider-c") && np.admitted, nc.reason);
    const auto bc = ch::brief(st, "c-op", "m-c-op");
    bool refused = false;
    for (const auto& e : bc.entries)
      if (bc.targets[e.target].id == "drafts" && e.aim == holdings::Aim::consult) refused = !e.decision.permitted && e.reaches == "provider-c";
    s.require("the briefing tells provider-c's agent that its readings reach provider-c and that the drafts' norm refuses them, and names the counts-as rule in force",
              bc.reaches == "provider-c" && refused && !bc.counts_as.empty() && has(bc.counts_as.front(), "operatives transmit to their provider"));
    State again;
    const auto v = ch::replay(parties(), host(true), st.acts, st.journal, &again);
    s.require("the record replays with its seals, and its state's digest is the same", v.holds() && ch::relational_digest(again) == ch::relational_digest(st), v.reason);
  }

  // ---- 3. The option off, a part of the community's own, a fault ----
  {
    State off = founded(host(false));
    const auto c = ch::step(off, reads("c-op", "drafts", 1));
    const auto silent = ch::step(off, reads("a-op", "catalogue", 1));
    const auto bc = ch::brief(off, "c-op", "m-c-op");
    s.require("with the option off, decision 0006's verdicts stand: provider-c's agent and the silent permission both read, nothing is reached, and the briefing names "
              "no provider and no rule in force",
              c.admitted && silent.admitted && off.accesses.back().reached.empty() && bc.reaches.empty() && bc.counts_as.empty(), c.reason + " / " + silent.reason);
    State own = founded(host(true, R"({"format": "noetarium.behaviour/1", "name": "own", "version": 1})"));
    const auto o = ch::step(own, reads("c-op", "drafts", 1));
    const auto bad = constitutions::admission(host(true, R"({"format": "noetarium.behaviour/1", "name": "own", "version": 1, "extends": "base 5",
      "conduct": [{"category": "a reading", "where": "kind is 'consult'"}],
      "counts as": [{"rule": "a reading reaches its maker", "conduct": "a reading", "as": {"transmission": "author"}, "where declared": "readers are watched"}]})"));
    bool named = false;
    for (const auto& f : bad) named = named || has(f.reason, "readers are watched");
    s.require("a part of the community's own that does not extend the base keeps the base's rule under its declaration, and provider-c's agent is refused; a counts-as rule "
              "under a declaration no constitution makes is a fault",
              !o.admitted && has(o.reason, "provider-c") && named, o.reason);
  }
  return s.done();
}
