// Checks of the facets of a norm: a rule of review before acceptance, followed in 19 of 20 episodes and cited in ten decisions
// ahead of any adoption, is marked "operative, unadopted" and loses the mark when adopted; a rule adopted and never invoked is
// marked "adopted, inoperative".

#include <thesmos/norms/norms.hpp>

#include <calx/verification/check.hpp>

#include <set>
#include <string>

using namespace thesmos;
using acts::Act;

namespace {
Act act(std::string author, std::string kind, std::map<std::string, std::string> fields) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.fields = std::move(fields);
  return a;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  std::vector<Act> chronicle;
  std::set<std::string> reviewed;
  for (int i = 0; i < 20; i++) {
    const std::string id = "c" + std::to_string(i);
    chronicle.push_back(act("agent", "deliver", {{"id", id}}));
    if (i != 7) {
      chronicle.push_back(act("reviewer", "assess", {{"object", id}}));
      reviewed.insert(id);
    }
    chronicle.push_back(act("steward", "accept", {{"id", id}, {"cites", i % 2 == 0 ? "review before acceptance" : ""}}));
  }
  const norms::Norm review{"review before acceptance", [](const Act& a) { return a.kind == "accept"; }, [&](const Act& a) { return reviewed.count(a.field("id")) > 0; }};
  const auto f = norms::facets(review, chronicle);
  s.require("the regularity is 19 of 20", f.regularity && *f.regularity == norms::Rational(19, 20));
  s.require("a rule invoked in ten decisions with no act of adoption is marked operative, unadopted", f.invoked == 10 && f.marks == std::vector<std::string>{"operative, unadopted"});
  chronicle.push_back(act("steward", "amend", {{"norm", "review before acceptance"}}));
  s.require("after its adoption the mark goes", norms::facets(review, chronicle).marks.empty());
  const norms::Norm weekly{"weekly summary", [](const Act&) { return false; }, [](const Act&) { return false; }};
  chronicle.push_back(act("steward", "amend", {{"norm", "weekly summary"}}));
  const auto g = norms::facets(weekly, chronicle);
  s.require("a rule adopted and never invoked is marked adopted, inoperative, and has no regularity", g.marks == std::vector<std::string>{"adopted, inoperative"} && !g.regularity);
  return s.done();
}
