// Checks of crossings: an act sent from one community to another is entered on both sides, and the receipt the receiver returns is
// verified by the sender against the receiver's head; a receipt that names another act, or an attestation outside the head, fails;
// a notary's witnessed head is consistent with the chronicle's later heads, and a chronicle that rewrote an entry it had shown is
// found; a party leaves with the acts that concern it, each provable to anyone who knows the head.

#include <thesmos/crossings/crossings.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;

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
State community(const std::string& prefix, const constitutions::Constitution& c) {
  Parties ps;
  for (const char* p : {"steward", "reviewer", "member"}) ps.add({prefix + p, Kind::person});
  State s(ps, c);
  Act found = act(prefix + "steward", "found", 0);
  found.holders = {{prefix + "steward", "steward"}, {prefix + "reviewer", "reviewer"}, {prefix + "member", "contributor"}};
  chronicle::step(s, found);
  return s;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto workshop = constitutions::read(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  State a = community("a/", workshop), b = community("b/", workshop);

  // A crossing act: a's steward reports to the community b; b's reviewer attests it.
  for (int t = 1; t <= 5; t++) chronicle::step(a, act("a/member", "commit", t, {{"id", "c" + std::to_string(t)}, {"to", "a/steward"}}));
  const Act notice = act("a/steward", "report", 6, {{"to community", "b"}, {"matter", "a correction to entry 17"}});
  s.require("the crossing act is entered in the sender's chronicle", chronicle::step(a, notice).admitted);
  const std::string act_seal = a.journal.entries.back().seal;
  const auto got = crossings::receive(b, "b", "b/reviewer", 6, "a", act_seal, notice);
  s.require("the receiver enters its attestation of the act and returns a receipt", got.admitted, got.reason);
  s.require("the sender verifies the receipt against the receiver's head: each side holds the other's proof", crossings::verify(got.receipt, a).holds(), crossings::verify(got.receipt, a).reason);
  const auto refused = crossings::receive(b, "b", "b/member", 7, "a", act_seal, notice);
  s.require("an attestation by a party whom the receiver's rules do not empower is refused", !refused.admitted, refused.reason);
  {
    crossings::Receipt forged = got.receipt;
    forged.act_digest = crossings::digest(act("a/steward", "report", 6, {{"to community", "b"}, {"matter", "something else"}}));
    crossings::Receipt moved = got.receipt;
    moved.receipt_envelope += "x";
    s.require("a receipt that names another act, or an attestation altered after it was entered, fails",
              crossings::verify(forged, a).fails() && crossings::verify(moved, a).fails());
  }

  // A notary witnesses a's head, then a later head; a rewritten chronicle is found.
  const crossings::Head witnessed = crossings::head("a", a);
  for (int t = 7; t <= 12; t++) chronicle::step(a, act("a/member", "commit", t, {{"id", "d" + std::to_string(t)}, {"to", "a/steward"}}));
  const crossings::Head later = crossings::head("a", a);
  s.require("a later head is consistent with the head a notary witnessed (" + std::to_string(witnessed.size) + " then " + std::to_string(later.size) + " entries)",
            crossings::consistent(witnessed, later, crossings::consistency(a, witnessed.size)).holds());
  {
    State rewritten(a.parties, workshop);
    for (std::size_t k = 0; k < a.acts.size(); k++) {
      Act x = a.acts[k];
      if (k == 3) x.fields["to"] = "a/reviewer";  // a commitment shown to the notary, now owed to another
      chronicle::step(rewritten, x);
    }
    const crossings::Head now = crossings::head("a", rewritten);
    s.require("a chronicle that rewrote an entry it had shown gives no proof of consistency with the witnessed head",
              rewritten.acts.size() == a.acts.size() && crossings::consistent(witnessed, now, crossings::consistency(rewritten, witnessed.size)).fails());
  }

  // Exit with the record.
  const auto portion = crossings::extract("a", a, "a/member");
  s.require("the member leaves with the acts that concern it (" + std::to_string(portion.items.size()) + "), each under the head anyone may know",
            portion.items.size() == 12 && crossings::verify(portion, later).holds());
  {
    auto forged = portion;
    forged.items[2].envelope += "x";
    s.require("an act altered in the record taken away fails against the head", crossings::verify(forged, later).fails());
    s.require("and a portion checked against another head fails", crossings::verify(portion, witnessed).fails());
  }
  return s.done();
}
