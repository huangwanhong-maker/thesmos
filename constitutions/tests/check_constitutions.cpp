// Checks of constitutions as data: an illustrative constitution read and admitted; a breach of each of the nine conditions of
// form found under its condition; a power held with no rule behind it found as overreach until declared as a reserve; text that
// is not JSON refused with its place; and the report of recognised against executable powers.

#include <thesmos/constitutions/constitutions.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <functional>
#include <sstream>
#include <string>

using namespace thesmos::constitutions;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
std::string first(const std::vector<Fault>& f) { return f.empty() ? std::string() : std::to_string(f.front().condition) + ": " + f.front().reason; }
}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string written = slurp(std::string(THESMOS_DATA) + "/examples/workshop.json");
  s.require("the illustrative constitution is read", !written.empty());
  const Constitution c = read(written);
  s.require("its parts are read: four positions, five decisions, four scenes, a route for claims and the custody of means",
            c.positions.size() == 4 && c.aggregation.size() == 5 && c.scenes.size() == 4 && c.claims.route == "stewards" && c.custody.declared);
  s.require("it is admitted by form", admission(c).empty(), first(admission(c)));

  const std::map<int, std::function<void(Constitution&)>> breaches = {
      {1, [](Constitution& x) { x.choice.push_back(Choice{"ghost", {"say"}, {}, {}, {}}); }},
      {2, [](Constitution& x) { x.aggregation["admit"].count_by.clear(); }},
      {3, [](Constitution& x) { x.amendment.by_level.erase("constitutional"); }},
      {4, [](Constitution& x) { x.claims = Claims{}; }},
      {5, [](Constitution& x) { x.boundary["reviewer"].entry = Entry{"after", "", "undeclared standing", {}}; }},
      {6, [](Constitution& x) { x.custody.powers["deploy"] = {"contributor"}; }},
      {7, [](Constitution& x) { x.preserved_declared = false; }},
      {8, [](Constitution& x) { x.circulation = {"tokens of joy"}; }},
      {9, [](Constitution& x) { x.records_declared = false; }},
  };
  int found = 0;
  for (const auto& [condition, breach] : breaches) {
    Constitution broken = c;
    breach(broken);
    for (const Fault& f : admission(broken))
      if (f.condition == condition) {
        found++;
        break;
      }
  }
  s.require("a breach of each of the nine conditions is found under that condition", found == 9, std::to_string(found) + " of 9");

  Constitution reserve = c;
  reserve.custody.powers["deploy"] = {"steward", "contributor"};
  bool overreach = false;
  for (const Fault& f : admission(reserve)) overreach = overreach || (f.condition == 6 && f.reason.find("overreach") != std::string::npos);
  s.require("a power held with no rule behind it is overreach", overreach);
  reserve.custody.reserve.push_back(Reserve{"contributor", "the founder's first deployment", "three rounds", {"deploy"}});
  s.require("and is admitted once declared as a reserve with its scope and term", admission(reserve).empty(), first(admission(reserve)));

  // Holdings and the norms of access (decision 0006): their form is held within closure (1) and the custody of means (6).
  Constitution held = c, kept = c;
  held.holdings.norms[0].object.value = "attic";
  kept.holdings.holdings.at("guide").custodian = "librarian";
  bool closure = false, custody = false;
  for (const Fault& f : admission(held)) closure = closure || (f.condition == 1 && f.reason.find("attic") != std::string::npos);
  for (const Fault& f : admission(kept)) custody = custody || (f.condition == 6 && f.reason.find("librarian") != std::string::npos);
  s.require("a norm of access naming an undeclared holding is a fault of closure, and a holding kept by no office a fault of the custody of means", closure && custody);

  // The visibility of the parts of a constitution, declared by its policy of records.
  Constitution hidden = c, ghost = c;
  hidden.visibility["claims"] = thesmos::holdings::Visibility::members();
  ghost.visibility["custody"] = thesmos::holdings::Visibility{thesmos::holdings::Visibility::Level::offices, {"treasurer"}};
  bool kept_from_claimants = false, unknown_office = false;
  for (const Fault& f : admission(hidden)) kept_from_claimants = kept_from_claimants || (f.condition == 4 && f.reason.find("public") != std::string::npos);
  for (const Fault& f : admission(ghost)) unknown_office = unknown_office || (f.condition == 1 && f.reason.find("treasurer") != std::string::npos);
  s.require("the workshop keeps its custody of means to its steward and its holders to its members, the rest public; a route for claims kept from the public, "
            "and a part shown to an undeclared office, are refused",
            shown(c, "custody").offices == std::set<std::string>{"steward"} && shown(c, "holders").level == thesmos::holdings::Visibility::Level::members &&
                shown(c, "choice").level == thesmos::holdings::Visibility::Level::everyone && kept_from_claimants && unknown_office);

  bool refused = false;
  try {
    read("{\"positions\": {\"steward\": [}\n}");
  } catch (const std::exception& e) {
    refused = std::string(e.what()).find("line 1") != std::string::npos;
  }
  s.require("text that is not JSON is refused with its line and column", refused);

  // The report of recognised against executable powers (Political Authority paper, Section 9).
  const auto report = powers({{"coordinator", {"deploy", "publish", "admit"}}, {"member", {"vote", "propose"}}},
                             {{"new coordinator", {"coordinator"}}, {"founder", {"member"}}, {"member-1", {"member"}}},
                             {{"new coordinator", {"publish"}}, {"founder", {"deploy", "publish", "admit", "vote", "propose"}}, {"member-1", {"vote", "propose"}}});
  s.require("the founder who kept the credentials can do what no rule lets them do",
            report.at("founder").overreach == std::vector<std::string>{"admit", "deploy", "publish"});
  s.require("the elected coordinator holds rights without the means", report.at("new coordinator").unsupported == std::vector<std::string>{"admit", "deploy"});
  s.require("a member's rights and means agree", report.at("member-1").usable == std::vector<std::string>{"propose", "vote"} &&
                                                     report.at("member-1").overreach.empty() && report.at("member-1").unsupported.empty());
  return s.done();
}
