// Checks of institutional statements: a duty read into its parts and written back; a prohibition read; a text that is not a statement,
// or names no kind of act of the ground, refused with where it stops; a constitution's choice rules read as statements; a duty's
// regularity measured over a chronicle (two of four claims answered in their term); a prohibition's breaches counted.

#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/statements/statements.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace thesmos;
using acts::Act;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.turn = turn;
  a.fields = std::move(fields);
  return a;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto duty = statements::read("steward must answer claim after a claim within 1 round or else the claim stands unanswered on the card.");
  const auto& d = duty.statement;
  s.require("a duty is read into its attribute, deontic, aim, object, condition, term and or-else",
            duty.read && d.attribute == "steward" && d.deontic == "must" && d.aim == "answer" && d.object == "claim" && d.trigger == "claim" && d.within == 1 &&
                d.or_else == "the claim stands unanswered on the card",
            duty.error);
  const auto again = statements::read(d.text());
  s.require("and written back, it reads as the same statement", again.read && again.statement.text() == d.text(), d.text());
  const auto never = statements::read("notary   must never vote.");
  s.require("a prohibition is read, spaces as written", never.read && never.statement.deontic == "must never" && never.statement.aim == "vote" && never.statement.attribute == "notary");
  const auto should = statements::read("steward should answer claim");
  const auto levitate = statements::read("steward must levitate");
  s.require("a text that is not a statement is refused with where it stops, and so is an aim that is no kind of act of the ground",
            !should.read && should.error.find("must") != std::string::npos && !levitate.read, should.error + " / " + levitate.error);

  const auto workshop = constitutions::read(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  const auto rules = statements::of(workshop);
  const statements::Statement* answer = nullptr;
  for (const auto& r : rules)
    if (r.deontic == "must" && r.aim == "answer") answer = &r;
  s.require("a constitution's choice rules are read as statements: the steward's duty to answer a claim within one round among them",
            answer && answer->attribute == "steward" && answer->trigger == "claim" && answer->within == 1 && rules.size() > 20, answer ? answer->text() : "none");

  // The duty measured over a chronicle of the illustrative commons.
  kernel::Parties ps;
  for (const char* p : {"steward", "reviewer", "resident"}) ps.add({p, kernel::Kind::person});
  chronicle::State st(ps, workshop);
  Act found = act("steward", "found", 0);
  found.holders = {{"steward", "steward"}, {"reviewer", "reviewer"}};
  bool all = chronicle::step(st, found).admitted;
  for (const Act& a : {act("resident", "claim", 1, {{"id", "k1"}, {"against", "the commons"}}), act("steward", "answer", 1, {{"id", "k1"}, {"finding", "corrected"}}),
                       act("resident", "claim", 2, {{"id", "k2"}, {"against", "the commons"}}), act("resident", "claim", 3, {{"id", "k3"}, {"against", "the commons"}}),
                       act("steward", "answer", 3, {{"id", "k2"}, {"finding", "corrected"}}), act("resident", "claim", 4, {{"id", "k4"}, {"against", "the commons"}}),
                       act("steward", "answer", 6, {{"id", "k3"}, {"finding", "corrected late"}})})
    all = all && chronicle::step(st, a).admitted;
  const auto holds = [&](const std::string& party, const std::string& position) { return st.holds(party, position); };
  const auto f = norms::facets(statements::norm(*answer, holds, st.acts), st.acts);
  s.require("its regularity is measured from the record: two of four claims answered within their term", all && f.episodes == 4 && f.regularity && *f.regularity == norms::Rational(1, 2));

  // A prohibition's breaches, counted over acts as recorded elsewhere (the fold itself refuses them).
  const std::vector<Act> recorded = {act("notary-1", "vote", 1, {{"id", "p"}}), act("envoy-1", "vote", 1, {{"id", "p"}}), act("notary-1", "attest", 2)};
  const auto holds_notary = [](const std::string& party, const std::string& position) { return position == "notary" && party.rfind("notary", 0) == 0; };
  const auto g = norms::facets(statements::norm(never.statement, holds_notary, recorded), recorded);
  s.require("a prohibition's episodes are the acts it forbids: one notary's vote, never conforming", g.episodes == 1 && g.regularity && *g.regularity == norms::Rational(0));
  return s.done();
}
