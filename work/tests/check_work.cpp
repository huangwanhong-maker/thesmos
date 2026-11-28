// Checks of the module work (decision 0012): kinds of work read from their written form and written as the design gives them; their
// faults named (an undeclared poster, taker, holding, office or decision, verdicts that do not include those accepted, a kind after
// itself or after an undeclared kind, an unknown way of taking); and the life of an item computed from its record and the turn: open,
// taken, lapsed at its term, delivered awaiting its check, its check failed and taken again, reviewed, returned within its redelivery
// and ended after it, or at once where the kind declares none, accepted, waiting for what must come first, withdrawn, several takers,
// and the overdue mark; and on 500 random records of takings, deliveries and reviews, the life as an independent reading says.

#include <thesmos/work/work.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace thesmos;
namespace w = thesmos::work;

namespace {
const char* kPart = R"({"kinds": {
  "section": {"posted by": ["steward"], "taken by": "commit", "takers": 1, "eligible": {"who": "writer"}, "term": 6,
              "deliverable": {"within": "sections", "kind": "document", "check": "section standard", "checked by": "steward"},
              "review": {"by": "another provider", "verdicts": ["adopt", "minor", "revise"], "accepts": ["adopt", "minor"]},
              "accepted by": "steward", "returned": {"redeliver within": 2, "then": "reopen"}},
  "review": {"posted by": ["steward"], "taken by": "commit", "takers": 1,
             "eligible": {"who": "reviewer", "provider other than": "the writer of the item reviewed"}, "term": 3,
             "deliverable": {"within": "reviews", "kind": "document"}, "review": {"by": "none"}, "accepted by": "steward"},
  "overview": {"posted by": ["steward"], "taken by": "award", "takers": 1, "eligible": {"who": "writer"}, "term": 12,
               "deliverable": {"within": "sections", "kind": "document"}, "review": {"by": "another provider", "verdicts": ["adopt", "revise"], "accepts": ["adopt"]},
               "accepted by": "adopt overview", "after": {"kinds": ["section"], "state": "accepted"}}
}})";
w::Names names() {
  w::Names n;
  n.positions = {"steward", "writer", "reviewer"};
  n.holdings = {"sections", "reviews"};
  n.decisions = {"adopt overview"};
  return n;
}
std::string first(const std::vector<std::string>& v) { return v.empty() ? std::string() : v.front(); }
bool has(const std::vector<std::string>& v, const std::string& part) {
  for (const auto& x : v)
    if (x.find(part) != std::string::npos) return true;
  return false;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto part = symploke::jsonld::parse(kPart);
  const w::Kinds kinds = w::read(&part);
  const w::Kind& section = kinds.kinds.at("section");

  // ---- 1. Kinds read, and their faults -------------------------------------------------------------------------------------------
  s.require("the kinds of the design's example are read: a section taken by commitment, one at a time, for six turns, checked, reviewed by another provider, accepted by "
            "the steward, delivered again within two turns of a return; a review by another provider than the writer's; an overview awarded after the sections",
            kinds.kinds.size() == 3 && section.taken_by == "commit" && section.term == 6 && section.check == "section standard" && section.review_by == "another provider" &&
                section.accepts == std::vector<std::string>{"adopt", "minor"} && section.redeliver_within == 2 && kinds.kinds.at("review").other_provider &&
                kinds.kinds.at("overview").taken_by == "award" && kinds.kinds.at("overview").after_kinds == std::vector<std::string>{"section"} &&
                w::faults(kinds, names()).empty(),
            first(w::faults(kinds, names())));
  {
    auto broken = [&](const std::string& text) {
      const auto j = symploke::jsonld::parse(text);
      return w::faults(w::read(&j), names());
    };
    const auto poster = broken(R"({"kinds": {"x": {"posted by": ["dean"], "accepted by": "steward"}}})");
    const auto taker = broken(R"({"kinds": {"x": {"posted by": ["steward"], "eligible": {"who": "author"}, "accepted by": "steward"}}})");
    const auto holding = broken(R"({"kinds": {"x": {"posted by": ["steward"], "deliverable": {"within": "drafts"}, "accepted by": "steward"}}})");
    const auto check = broken(R"({"kinds": {"x": {"posted by": ["steward"], "deliverable": {"check": "a standard"}, "accepted by": "steward"}}})");
    const auto verdicts = broken(R"({"kinds": {"x": {"posted by": ["steward"], "review": {"by": "another origin", "verdicts": ["yes"], "accepts": ["fine"]}, "accepted by": "steward"}}})");
    const auto accepter = broken(R"({"kinds": {"x": {"posted by": ["steward"], "accepted by": "the board"}}})");
    const auto after = broken(R"({"kinds": {"x": {"posted by": ["steward"], "accepted by": "steward", "after": {"kinds": ["x", "y"]}}}})");
    const auto taking = broken(R"({"kinds": {"x": {"posted by": ["steward"], "taken by": "lottery", "accepted by": "steward"}}})");
    s.require("faults named: a poster, a taker's position, a holding, a check's office and an accepting office or decision the constitution lacks, verdicts that do not "
              "include those accepted, a kind after itself and after an undeclared kind, an unknown way of taking",
              has(poster, "posted by dean") && has(taker, "holders of author") && has(holding, "within drafts") && has(check, "names no office") && has(verdicts, "accepts the verdict fine") &&
                  has(accepter, "accepted by the board") && has(after, "comes after itself") && has(after, "kind y, which is not declared") && has(taking, "not lottery"));
  }

  // ---- 2. The life of an item ----------------------------------------------------------------------------------------------------
  {
    w::Item i;
    i.id = "s1", i.kind = "section", i.posted = 1, i.due = 20;
    const bool open = w::state(section, i, 1) == "open";
    i.takings.push_back({"c1", "writer-a", "author", "author", "provider-a", 2, false, false, -1});
    const bool taken = w::state(section, i, 3) == "taken" && w::state(section, i, 7) == "taken";
    const bool lapsed = w::state(section, i, 8) == "open" && w::taking_state(section, i, i.takings[0], 8) == "lapsed";
    s.require("an item posted is open; taken, it runs for its term of six turns, and lapses at turn eight with nothing delivered, the item open again",
              open && taken && lapsed);
    i.deliveries.push_back({"c1", "draft 1", "writer-a", "author", "provider-a", 5, "pending"});
    const bool pending = w::state(section, i, 9) == "delivered";
    i.deliveries.back().check = "failed";
    const bool failed = w::state(section, i, 7) == "taken";
    i.deliveries.push_back({"c1", "draft 2", "writer-a", "author", "provider-a", 7, "passed"});
    const bool delivered = w::state(section, i, 30) == "delivered";
    s.require("a delivery awaits its check, and holds the taking past its term; a failed check leaves the item taken; a delivery that passes awaits its review",
              pending && failed && delivered);
    i.reviews.push_back({"reviewer-b", "board", "provider-b", "revise", 8, 1});
    const bool returned = w::state(section, i, 9) == "returned" && w::state(section, i, 10) == "returned";
    const bool reopened = w::state(section, i, 11) == "open";
    s.require("a verdict the kind does not accept returns the item, which may be delivered again within two turns and reopens after them", returned && reopened);
    i.deliveries.push_back({"c1", "draft 3", "writer-a", "author", "provider-a", 10, "passed"});
    i.reviews.push_back({"reviewer-b", "board", "provider-b", "minor", 11, 2});
    const bool reviewed = w::state(section, i, 12) == "reviewed";
    i.takings[0].accepted = true, i.takings[0].accepted_at = 12;
    s.require("delivered again, a verdict the kind accepts leaves it reviewed, and its acceptance accepts it", reviewed && w::state(section, i, 13) == "accepted");
    w::Kind strict = section;
    strict.redeliver_within = -1;
    w::Item j = i;
    j.takings[0].accepted = false, j.reviews.pop_back(), j.deliveries.pop_back();
    s.require("where the kind declares no redelivery, a returned item reopens at once: the fault the first attempt met, now a rule of the kind",
              w::state(strict, j, 9) == "open" && w::taking_state(strict, j, j.takings[0], 9) == "ended");
    w::Item k;
    k.id = "o1", k.kind = "overview";
    const auto& overview = kinds.kinds.at("overview");
    w::Item withdrawn = k;
    withdrawn.withdrawn = true;
    w::Kind pair = section;
    pair.takers = 2;
    w::Item shared;
    shared.id = "s2", shared.kind = "section";
    shared.takings.push_back({"c2", "writer-a", "author", "author", "provider-a", 1, false, false, -1});
    const bool still_open = w::state(pair, shared, 2) == "open";
    shared.takings.push_back({"c3", "writer-c", "editor", "editor", "provider-c", 2, false, false, -1});
    s.require("an item waits while what comes first is not accepted; a withdrawn item is withdrawn; with two takers an item stays open after one taking and is taken after "
              "two; an item past its due turn and not accepted is overdue",
              w::state(overview, k, 1, false) == "waiting" && w::state(overview, k, 1, true) == "open" && w::state(overview, withdrawn, 1) == "withdrawn" && still_open &&
                  w::state(pair, shared, 3) == "taken" && w::overdue(section, j, 21) && !w::overdue(section, i, 21));
  }

  // ---- 3. The life on random records ------------------------------------------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 40, 0, 0);
    int agree = 0;
    for (int trial = 0; trial < 500; trial++) {
      w::Kind k = section;
      k.term = static_cast<long long>(1 + r.below(6)), k.redeliver_within = r.below(3) == 0 ? -1 : static_cast<long long>(r.below(4));
      if (r.below(3) == 0) k.check.clear();
      if (r.below(4) == 0) k.review_by = "none";
      w::Item i;
      i.id = "x", i.kind = "section";
      i.takings.push_back({"c", "t", "b", "b", "p", 0, false, false, -1});
      long long turn = 0;
      const int deliveries = static_cast<int>(r.below(4));
      for (int d = 0; d < deliveries; d++) {
        turn += static_cast<long long>(r.below(4));
        const std::string check = k.check.empty() ? "none" : r.below(4) == 0 ? "failed" : r.below(5) == 0 ? "pending" : "passed";
        i.deliveries.push_back({"c", "a", "t", "b", "p", turn, check});
        if (check != "failed" && check != "pending" && k.review_by != "none" && r.below(2)) {
          turn += static_cast<long long>(r.below(3));
          i.reviews.push_back({"rv", "o", "q", r.below(2) ? "adopt" : "revise", turn, i.deliveries.size() - 1});
        }
      }
      const long long at = turn + static_cast<long long>(r.below(6));
      // The independent reading: the last delivery that did not fail, its review on it, and the clock.
      int last = -1;
      for (int d = 0; d < static_cast<int>(i.deliveries.size()); d++)
        if (i.deliveries[static_cast<std::size_t>(d)].check != "failed") last = d;
      std::string expect;
      if (last < 0) expect = at >= k.term ? "open" : "taken";
      else {
        const auto& d = i.deliveries[static_cast<std::size_t>(last)];
        const w::Review* rv = nullptr;
        for (const auto& x : i.reviews)
          if (x.delivery == static_cast<std::size_t>(last)) rv = &x;
        if (d.check == "pending" || k.review_by == "none" || !rv) expect = "delivered";
        else if (rv->verdict == "adopt") expect = "reviewed";
        else expect = k.redeliver_within >= 0 && at <= rv->turn + k.redeliver_within ? "returned" : "open";
      }
      agree += w::state(k, i, at) == expect;
    }
    s.require("on 500 random records of a taking with its deliveries, checks and reviews, under random terms, redeliveries, checks and reviews, the item's state is as an "
              "independent reading of the record and the clock says",
              agree == 500, std::to_string(agree));
  }
  return s.done();
}
