// Checks of posted work in the fold (decision 0012): the base taxonomy's fourth version, with items as an effect primitive and their
// reopening by law, a revision of the third; an application decided by the condition that reads posted work; the full life of a section
// (posted, taken by one writer at a time, delivered, its check failed and passed, returned by a review of another provider, delivered
// again, reviewed, accepted by the steward), a review refused from the writer's provider and from the writer's own principal, an
// acceptance refused before the review; a taking that lapses at its term and the item taken by another; a return whose redelivery runs
// out reopening the item; a review item open only for another provider than the writer's, and the critic's application decided by it;
// an item withdrawn by its poster, and not once taken; an award by an office; an item waiting for what must come first; a kind that
// declares no redelivery, its life then amended by the assembly, the fault become an amendment; a holding reviewed by another provider
// with verdicts that check and verdicts that do not; the briefing's and profile's view of posted work; on 200 random chronicles no item
// held by more takers than its kind admits (Proposition 30) and every accepted item reviewed by another provider than its writer's
// (Proposition 31); and the record replays.

#include <thesmos/behaviour/ontology.hpp>
#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/profile.hpp>

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
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
const char* kKinds = R"({"kinds": {
  "section": {"posted by": ["steward"], "taken by": "commit", "takers": 1, "eligible": {"who": "writer"}, "term": 6,
              "deliverable": {"within": "sections", "kind": "document", "check": "section standard", "checked by": "steward"},
              "review": {"by": "another provider", "verdicts": ["adopt", "minor", "revise"], "accepts": ["adopt", "minor"]},
              "accepted by": "steward", "returned": {"redeliver within": 2, "then": "reopen"}},
  "review": {"posted by": ["steward"], "taken by": "commit", "takers": 1,
             "eligible": {"who": "critic", "provider other than": "the writer of the item reviewed"}, "term": 3,
             "deliverable": {"within": "reviews", "kind": "document"}, "review": {"by": "none"}, "accepted by": "steward"},
  "overview": {"posted by": ["steward"], "taken by": "award", "takers": 1, "eligible": {"who": "writer"}, "term": 12,
               "review": {"by": "another provider", "verdicts": ["adopt", "revise"], "accepts": ["adopt"]}, "accepted by": "steward"},
  "summary": {"posted by": ["steward"], "taken by": "commit", "eligible": {"who": "writer"}, "review": {"by": "none"}, "accepted by": "steward",
              "after": {"kinds": ["section"], "state": "accepted"}},
  "chapter": {"posted by": ["steward"], "taken by": "commit", "eligible": {"who": "writer"},
              "review": {"by": "another provider", "verdicts": ["adopt", "revise"], "accepts": ["adopt"]}, "accepted by": "steward"}
}})";
constitutions::Constitution host() {
  auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  auto set = [&](const std::string& part, const std::string& key, const std::string& value) { doc.get(part)->set(key, symploke::jsonld::parse(value)); };
  set("positions", "writer", R"({"held by": ["person", "operative"]})");
  set("positions", "critic", R"({"held by": ["person", "operative"]})");
  set("boundary", "writer", R"({"entry": {"on application": {"while": "work open of kind section"}}, "exit": "free"})");
  set("boundary", "critic", R"({"entry": {"on application": {"while": "work open for the applicant's provider"}}, "exit": "free"})");
  set("holdings", "sections", R"({"kind": "document", "custodian": "steward", "review": "another provider", "verdicts": ["adopt", "minor", "revise"], "checks on": ["adopt", "minor"]})");
  set("holdings", "reviews", R"({"kind": "document", "custodian": "steward"})");
  doc.get("access")->get("norms")->items.push_back(symploke::jsonld::parse(R"({"id": "sections deposited", "who": "writer", "may": "deposit", "object": "sections"})"));
  for (const char* rule : {R"({"who": "writer", "may": ["commit", "deliver", "offer", "hand over"]})", R"({"who": "critic", "may": ["commit", "deliver", "assess", "attest"]})"})
    doc.get("choice")->items.push_back(symploke::jsonld::parse(rule));
  doc.set("work", symploke::jsonld::parse(kKinds));
  return constitutions::read(symploke::jsonld::dump(doc));
}
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "ana", "bea", "cal", "dan", "cy"}) ps.add({p, Kind::person});
  ps.add({"w-a", Kind::operative, "ana", "provider-a"});
  ps.add({"w-b", Kind::operative, "bea", "provider-b"});
  ps.add({"r-a", Kind::operative, "cal", "provider-a"});
  ps.add({"r-b", Kind::operative, "dan", "provider-b"});
  return ps;
}
Act mandate(const std::string& principal, const std::string& holder, long long turn) {
  Act a = act(principal, "mandate", turn);
  mandates::Mandate m;
  m.id = "m-" + holder, m.principal = principal, m.holder = holder, m.from = 0, m.to = 1000;
  m.kinds = {"apply", "commit", "deliver", "offer", "assess", "attest", "deposit", "hand over", "cancel"};
  a.grant = m;
  return a;
}
State founded() {
  State s(parties(), host());
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"cy", "contributor"}};
  ch::step(s, f);
  for (const auto& [p, h] : std::vector<std::pair<std::string, std::string>>{{"ana", "w-a"}, {"bea", "w-b"}, {"cal", "r-a"}, {"dan", "r-b"}}) ch::step(s, mandate(p, h, 0));
  return s;
}
// An operative's act under its mandate.
Act by(const std::string& op, const std::string& kind, long long turn, std::map<std::string, std::string> fields = {}) { return act(op, kind, turn, std::move(fields), "m-" + op); }
Act post(const std::string& id, const std::string& kind, long long turn, std::map<std::string, std::string> more = {}) {
  more["id"] = id, more["work"] = kind;
  return act("sam", "request", turn, more);
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. The fourth version ------------------------------------------------------------------------------------------------------
  {
    const auto direct = bh::revision_faults(bh::third_base(), bh::fourth_base());
    const auto by_kategoria = bh::admit_revision(bh::third_base(), bh::fourth_base());
    calx::random::Stream r(20261127, 42, 0, 0);
    int carried = 0;
    for (int trial = 0; trial < 50; trial++) {
      kategoria::ontologies::Record rec;
      const auto& v3 = bh::third_base();
      for (int k = 0; k < 12; k++) {
        kategoria::ontologies::Fact f;
        f.id = "fact" + std::to_string(k), f.text = f.id;
        if (r.below(4) == 0) f.kind = "apply", f.axes["force"] = "directive";
        else {
          const auto& p = v3.primitives[r.below(v3.primitives.size())];
          f.kind = bh::ident(p.kind), f.axes["force"] = std::string(acts::word(p.force));
        }
        rec.facts.push_back(std::move(f));
      }
      const auto m = bh::carry(rec, v3, bh::fourth_base());
      carried += m.lost.empty() && m.problems.empty() && bh::carried_as_extension(rec, v3, bh::fourth_base()).holds();
    }
    s.require("the base's fourth version, with items an effect primitive and their reopening an effect by law, is a revision of the third admitted directly and by "
              "kategoria, renaming nothing; 50 random records of the third carry to it as the left Kan extension (Proposition 32)",
              direct.empty() && by_kategoria.admitted() && bh::fourth_base().effect("item") && bh::fourth_base().primitive("request")->reads.size() > 5 && carried == 50,
              (direct.empty() ? std::string() : direct.front()) + " / " + by_kategoria.verdict.reason);
  }

  // ---- 2. The full life of a section ------------------------------------------------------------------------------------------------
  State st = founded();
  long long turn = 1;
  {
    const auto early = ch::step(st, by("w-a", "apply", turn, {{"id", "a1"}, {"position", "writer"}}));
    const auto posted = ch::step(st, post("s1", "section", turn, {{"matter", "chapter one"}, {"due", "40"}}));
    const auto later = ch::step(st, by("w-a", "apply", turn, {{"id", "a2"}, {"position", "writer"}}));
    ch::step(st, by("w-b", "apply", turn, {{"id", "a3"}, {"position", "writer"}}));
    s.require("the writer's position is entered while work of kind section is open: an application before any is declined with the condition, one after is admitted",
              early.admitted && st.items.at("a1").status == "declined" && has(st.items.at("a1").finding, "work open of kind section") && posted.admitted && later.admitted &&
                  st.holds("w-a", "writer") && st.holds("w-b", "writer"),
              st.items.at("a1").finding + " / " + posted.reason);
    const auto c1 = ch::step(st, by("w-a", "commit", ++turn, {{"id", "c1"}, {"item", "s1"}}));
    const auto c2 = ch::step(st, by("w-b", "commit", turn, {{"id", "c2"}, {"item", "s1"}}));
    const auto stranger = ch::step(st, act("cy", "commit", turn, {{"id", "c0"}, {"item", "s1"}}));
    s.require("one writer takes the section by a commitment naming it; a second is refused while it is taken, and one who is no writer is refused",
              c1.admitted && ch::work_state(st, st.work_items.at("s1"), turn) == "taken" && !c2.admitted && has(c2.reason, "is taken") && !stranger.admitted,
              c2.reason + " / " + stranger.reason);
    ch::step(st, by("w-a", "deliver", ++turn, {{"id", "c1"}, {"artefact", "draft one"}}));
    const auto pending = ch::work_state(st, st.work_items.at("s1"), turn);
    const auto failed = ch::step(st, act("sam", "attest", turn, {{"item", "s1"}, {"result", "fail"}}));
    const bool again = ch::work_state(st, st.work_items.at("s1"), turn) == "taken";
    ch::step(st, by("w-a", "deliver", ++turn, {{"id", "c1"}, {"artefact", "draft two"}}));
    const auto passed = ch::step(st, act("sam", "attest", turn, {{"item", "s1"}, {"result", "pass"}}));
    const auto too_soon = ch::step(st, act("sam", "accept", turn, {{"id", "c1"}}));
    s.require("a delivery awaits the check the steward enters; a failed check leaves the section taken, and a delivery that passes awaits its review; the steward may not "
              "accept it before a verdict",
              pending == "delivered" && failed.admitted && again && passed.admitted && !too_soon.admitted && has(too_soon.reason, "after a verdict its kind accepts"),
              too_soon.reason);
    const auto same_provider = ch::step(st, by("r-a", "assess", turn, {{"object", "s1"}, {"criterion", "review"}, {"value", "adopt"}}));
    const auto own_principal = ch::step(st, act("ana", "assess", turn, {{"object", "s1"}, {"criterion", "review"}, {"value", "adopt"}}));
    const auto returned = ch::step(st, by("r-b", "assess", turn, {{"object", "s1"}, {"criterion", "review"}, {"value", "revise"}}));
    s.require("a review by the writer's provider is refused, and so is one by the writer's own principal; a review by another provider asking for revision returns it",
              !same_provider.admitted && has(same_provider.reason, "another provider") && !own_principal.admitted && has(own_principal.reason, "deliverer's origin") && returned.admitted &&
                  ch::work_state(st, st.work_items.at("s1"), turn) == "returned",
              same_provider.reason + " / " + own_principal.reason);
    ch::step(st, by("w-a", "deliver", ++turn, {{"id", "c1"}, {"artefact", "draft three"}}));
    ch::step(st, act("sam", "attest", turn, {{"item", "s1"}, {"result", "pass"}}));
    ch::step(st, by("r-b", "assess", turn, {{"object", "s1"}, {"criterion", "review"}, {"value", "minor"}}));
    const auto by_reviewer = ch::step(st, by("r-b", "accept", turn, {{"id", "c1"}}));
    const auto accepted = ch::step(st, act("sam", "accept", turn, {{"id", "c1"}}));
    s.require("delivered again within its redelivery, checked and reviewed with a verdict the kind accepts, the section is accepted by the steward and by no one else",
              !by_reviewer.admitted && accepted.admitted && ch::work_state(st, st.work_items.at("s1"), turn) == "accepted" &&
                  st.commitments.at("c1").status == commitments::Status::discharged,
              by_reviewer.reason + " / " + accepted.reason);
  }

  // ---- 3. Lapse, return past its redelivery, withdrawal, review items --------------------------------------------------------------
  {
    const long long t0 = ++turn;
    ch::step(st, post("s2", "section", t0, {{"matter", "chapter two"}}));
    ch::step(st, by("w-b", "commit", t0, {{"id", "c3"}, {"item", "s2"}}));
    const auto late = ch::step(st, by("w-b", "deliver", t0 + 6, {{"id", "c3"}, {"artefact", "late"}}));
    const auto retaken = ch::step(st, by("w-a", "commit", t0 + 6, {{"id", "c4"}, {"item", "s2"}}));
    s.require("a taking with nothing delivered lapses at its term of six turns: the late delivery is refused and the section is taken by another",
              !late.admitted && has(late.reason, "lapsed at turn " + std::to_string(t0 + 6)) && retaken.admitted, late.reason + " / " + retaken.reason);
    turn = t0 + 6;
    ch::step(st, by("w-a", "deliver", turn, {{"id", "c4"}, {"artefact", "draft"}}));
    ch::step(st, act("sam", "attest", turn, {{"item", "s2"}, {"result", "pass"}}));
    ch::step(st, by("r-b", "assess", turn, {{"object", "s2"}, {"criterion", "review"}, {"value", "revise"}}));
    turn += 3;
    const auto out_of_time = ch::step(st, by("w-a", "deliver", turn, {{"id", "c4"}, {"artefact", "too late"}}));
    const auto reopened = ch::step(st, by("w-b", "commit", turn, {{"id", "c5"}, {"item", "s2"}}));
    s.require("a return whose redelivery of two turns has run out ends the taking: the delivery is refused and the section, reopened, is taken by another",
              !out_of_time.admitted && has(out_of_time.reason, "redelivery ran out") && reopened.admitted, out_of_time.reason + " / " + reopened.reason);
    ch::step(st, post("s3", "section", turn));
    ch::step(st, post("s4", "section", turn));
    ch::step(st, by("w-a", "commit", turn, {{"id", "c6"}, {"item", "s4"}}));
    const auto withdrawn = ch::step(st, act("sam", "cancel", turn, {{"item", "s3"}}));
    const auto held = ch::step(st, act("sam", "cancel", turn, {{"item", "s4"}}));
    s.require("its poster withdraws an item no one holds, and not one that is taken", withdrawn.admitted && ch::work_state(st, st.work_items.at("s3"), turn) == "withdrawn" && !held.admitted,
              held.reason);
    // A review of the accepted section, open only to another provider than its writer's.
    ch::step(st, post("rv1", "review", turn, {{"of", "s1"}}));
    const auto ra = ch::step(st, by("r-a", "apply", turn, {{"id", "a4"}, {"position", "critic"}}));
    const auto rb = ch::step(st, by("r-b", "apply", turn, {{"id", "a5"}, {"position", "critic"}}));
    const auto take = ch::step(st, by("r-b", "commit", turn, {{"id", "c7"}, {"item", "rv1"}}));
    ch::step(st, by("r-b", "deliver", turn, {{"id", "c7"}, {"artefact", "a review"}}));
    const auto took = ch::step(st, act("sam", "accept", turn, {{"id", "c7"}}));
    s.require("the critic's position is entered while a review is open for the applicant's provider: declined for the writer's provider, admitted for another; the review "
              "item taken, delivered and accepted with no review of its own",
              ra.admitted && st.items.at("a4").status == "declined" && rb.admitted && st.holds("r-b", "critic") && take.admitted && took.admitted &&
                  ch::work_state(st, st.work_items.at("rv1"), turn) == "accepted",
              st.items.at("a4").finding + " / " + take.reason + " / " + took.reason);
  }

  // ---- 4. An award, an item that waits, and a life amended ---------------------------------------------------------------------------
  {
    ch::step(st, post("o1", "overview", ++turn));
    const auto commit = ch::step(st, by("w-a", "commit", turn, {{"id", "c8"}, {"item", "o1"}}));
    const auto offer = ch::step(st, by("w-a", "offer", turn, {{"id", "c8"}, {"item", "o1"}}));
    const bool open_after_offer = ch::work_state(st, st.work_items.at("o1"), turn) == "open";
    const auto awarded = ch::step(st, act("sam", "take", turn, {{"id", "c8"}}));
    s.require("an overview is awarded: the writer's own commitment refused, its offer leaves the item open, and the steward's taking of the offer gives it the item",
              !commit.admitted && offer.admitted && open_after_offer && awarded.admitted && ch::work_state(st, st.work_items.at("o1"), turn) == "taken", commit.reason + " / " + awarded.reason);
    ch::step(st, post("sum", "summary", turn));
    const bool waits = ch::work_state(st, st.work_items.at("sum"), turn) == "waiting";
    const auto too_early = ch::step(st, by("w-b", "commit", turn, {{"id", "c9"}, {"item", "sum"}}));
    s.require("a summary waits until every section posted and not withdrawn is accepted, and cannot be taken meanwhile", waits && !too_early.admitted && has(too_early.reason, "waiting"),
              too_early.reason);
    // A kind with no redelivery: a returned chapter reopens at once; the assembly amends the kind's life.
    ch::step(st, post("ch1", "chapter", turn));
    ch::step(st, by("w-a", "commit", turn, {{"id", "c10"}, {"item", "ch1"}}));
    ch::step(st, by("w-a", "deliver", turn, {{"id", "c10"}, {"artefact", "chapter"}}));
    ch::step(st, by("r-b", "assess", turn, {{"object", "ch1"}, {"criterion", "review"}, {"value", "revise"}}));
    const auto refused = ch::step(st, by("w-a", "deliver", turn, {{"id", "c10"}, {"artefact", "revised chapter"}}));
    ch::step(st, act("rae", "propose", turn, {{"id", "life"}, {"change.work", R"({"chapter": {"posted by": ["steward"], "taken by": "commit", "eligible": {"who": "writer"},
      "review": {"by": "another provider", "verdicts": ["adopt", "revise"], "accepts": ["adopt"]}, "accepted by": "steward", "returned": {"redeliver within": 2}}})"}}));
    for (const char* v : {"sam", "rae", "cy"}) ch::step(st, act(v, "vote", turn, {{"id", "life"}, {"value", "yes"}}));
    const auto amended = ch::step(st, act("sam", "amend", ++turn, {{"id", "life"}}));
    ch::step(st, post("ch2", "chapter", turn));
    ch::step(st, by("w-a", "commit", turn, {{"id", "c11"}, {"item", "ch2"}}));
    ch::step(st, by("w-a", "deliver", turn, {{"id", "c11"}, {"artefact", "chapter"}}));
    ch::step(st, by("r-b", "assess", turn, {{"object", "ch2"}, {"criterion", "review"}, {"value", "revise"}}));
    const auto redelivered = ch::step(st, by("w-a", "deliver", turn + 1, {{"id", "c11"}, {"artefact", "revised chapter"}}));
    s.require("where the kind declares no redelivery a returned chapter reopens at once and the writer's revision is refused, the fault the first attempt met; the assembly "
              "amends the kind's life, and a chapter posted thereafter is delivered again after its return, while the earlier chapter keeps the life it was posted under",
              !refused.admitted && has(refused.reason, "declares no redelivery") && amended.admitted && st.rules.work.kinds.at("chapter").redeliver_within == 2 && redelivered.admitted &&
                  ch::work_state(st, st.work_items.at("ch1"), turn + 1) == "open",
              refused.reason + " / " + amended.reason + " / " + redelivered.reason);
    turn += 1;
  }

  // ---- 5. A holding reviewed by another provider, with verdicts ---------------------------------------------------------------------
  {
    const auto deposited = ch::step(st, by("w-b", "deposit", turn, {{"holding", "part one"}, {"within", "sections"}, {"digest", archeion::fixity::sha256_hex("part one")}}));
    const auto same = ch::step(st, by("r-b", "attest", turn, {{"holding", "part one"}, {"verdict", "adopt"}}));
    const auto revise = ch::step(st, act("sam", "attest", turn, {{"holding", "part one"}, {"verdict", "revise"}}));
    const bool still = st.holdings.at("part one").pending;
    const auto adopt = ch::step(st, act("sam", "attest", turn, {{"holding", "part one"}, {"verdict", "adopt"}}));
    s.require("a deposit within a holding reviewed by another provider: refused from the depositor's provider; a verdict that does not check leaves it pending, recorded; "
              "a verdict that checks checks it",
              deposited.admitted && !same.admitted && has(same.reason, "provider") && revise.admitted && still && adopt.admitted && !st.holdings.at("part one").pending,
              deposited.reason + " / " + same.reason + " / " + revise.reason + " / " + adopt.reason);
  }

  // ---- 6. The briefing's and profile's view ----------------------------------------------------------------------------------------
  {
    ch::step(st, post("s5", "section", turn, {{"due", std::to_string(turn + 1)}}));
    const auto wa = ch::brief(st, "w-a", "m-w-a"), rb = ch::brief(st, "r-b", "m-r-b"), sam = ch::brief(st, "sam");
    auto any = [](const ch::Briefing& b, const std::string& kind, const std::string& id) {
      for (const auto& w : b.work)
        if (w.kind == kind && w.id == id) return true;
      return false;
    };
    State later = st;
    ch::step(later, act("sam", "say", turn + 3));
    const auto p = ch::profile(later);
    bool overdue = false, listed = false;
    for (const auto& x : p.posted_work) overdue = overdue || (x.id == "s5" && x.overdue), listed = listed || (x.id == "s1" && x.state == "accepted");
    s.require("the briefing shows a writer the section it may take and the chapter it must deliver, a reviewer of another provider the delivery awaiting its review, and the "
              "steward the deliveries awaiting its check and its acceptance; the profile lists posted work with its state and marks an item overdue past its due turn",
              any(wa, "take", "s5") && any(wa, "deliver", "c6") && any(rb, "review", "ch2") && any(sam, "accept", "c11") == false && listed && overdue,
              [&] {
                std::string out;
                for (const auto* b : {&wa, &rb, &sam})
                  for (const auto& w : b->work) out += b->party + ":" + w.kind + ":" + w.id + " ";
                return out + (listed ? "listed " : "") + (overdue ? "overdue" : "");
              }());
  }

  // ---- 7. Propositions 30 and 31 on random chronicles --------------------------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 41, 0, 0);
    int bounded = 0, reviewed = 0, accepted_items = 0, refused_reviews = 0;
    const std::vector<std::string> writers = {"w-a", "w-b"}, reviewers = {"r-a", "r-b", "ana", "dan"};
    for (int trial = 0; trial < 200; trial++) {
      State x = founded();
      for (const auto& w : writers) {
        ch::step(x, post("seed-" + w, "section", 1));
        ch::step(x, by(w, "apply", 1, {{"id", "ap-" + w}, {"position", "writer"}}));
      }
      bool within = true, by_other = true;
      int n = 0;
      for (long long t = 2; t < 60; t++) {
        const int what = static_cast<int>(r.below(12));
        const std::string w = writers[r.below(2)];
        // Items chosen where the act could bear: open ones to take, a pending delivery to check, a delivery awaiting review to review.
        std::vector<std::string> open, pending, awaiting, all;
        for (const auto& [id, i] : x.work_items) {
          all.push_back(id);
          const std::string st = ch::work_state(x, i, t);
          if (st == "open") open.push_back(id);
          if (!i.deliveries.empty() && i.deliveries.back().check == "pending") pending.push_back(id);
          if (st == "delivered" && !i.deliveries.empty() && i.deliveries.back().check == "passed") awaiting.push_back(id);
        }
        auto pick = [&](const std::vector<std::string>& v) { return v.empty() ? all[r.below(all.size())] : v[r.below(v.size())]; };
        if (what == 0) ch::step(x, post("i" + std::to_string(t), "section", t));
        else if (what <= 2) ch::step(x, by(w, "commit", t, {{"id", "c" + std::to_string(++n)}, {"item", pick(open)}}));
        else if (what <= 4) {
          for (const auto& [c, id] : x.work_of)
            if (x.commitments.at(c).debtor == x.parties.at(w).controller || x.commitments.at(c).by == w) ch::step(x, by(w, "deliver", t, {{"id", c}, {"artefact", "v" + std::to_string(t)}}));
        } else if (what == 5) ch::step(x, act("sam", "attest", t, {{"item", pick(pending)}, {"result", r.below(5) ? "pass" : "fail"}}));
        else if (what <= 8) {
          const std::string who = reviewers[r.below(4)];
          const std::map<std::string, std::string> f = {{"object", pick(awaiting)}, {"criterion", "review"}, {"value", r.below(3) ? "adopt" : "revise"}};
          const auto o = ch::step(x, who.rfind("r-", 0) == 0 ? by(who, "assess", t, f) : act(who, "assess", t, f));
          refused_reviews += !o.admitted;
        } else {
          for (const auto& [c, id] : x.work_of) ch::step(x, act("sam", "accept", t, {{"id", c}}));
        }
        // Proposition 30: no item held by more takers than its kind admits, lapses and returns counted as the clock gives them.
        for (const auto& [id, i] : x.work_items) {
          const work::Kind& k = i.life;
          long long holding = 0;
          for (const auto& tk : i.takings) {
            const std::string ts = work::taking_state(k, i, tk, t);
            holding += work::active(ts) || ts == "accepted";
          }
          within = within && holding <= k.takers;
        }
      }
      // Proposition 31: every accepted taking has, on its last delivery, a review with a verdict the kind accepts by another provider than the deliverer's.
      for (const auto& [id, i] : x.work_items)
        for (const auto& tk : i.takings) {
          if (!tk.accepted) continue;
          accepted_items++;
          std::size_t last = 0;
          bool found = false;
          for (std::size_t d = 0; d < i.deliveries.size(); d++)
            if (i.deliveries[d].commitment == tk.commitment && i.deliveries[d].check != "failed") last = d, found = true;
          bool ok = false;
          for (const auto& rv : i.reviews)
            ok = ok || (found && rv.delivery == last && (rv.verdict == "adopt" || rv.verdict == "minor") && rv.provider != i.deliveries[last].provider &&
                        rv.origin != i.deliveries[last].origin);
          by_other = by_other && ok;
        }
      bounded += within, reviewed += by_other;
    }
    s.require("on 200 random chronicles of postings, takings, deliveries, checks, reviews and acceptances, no item is ever held by more takers than its kind admits "
                  "(Proposition 30), and every one of " + std::to_string(accepted_items) + " accepted items was reviewed on its last delivery, with a verdict the kind accepts, by "
                  "another provider and origin than its writer's (Proposition 31; " + std::to_string(refused_reviews) + " reviews refused)",
              bounded == 200 && reviewed == 200 && accepted_items > 20 && refused_reviews > 100);
  }

  // ---- 8. Replay -----------------------------------------------------------------------------------------------------------------------
  {
    const auto v = ch::replay(parties(), host(), st.acts, st.journal);
    s.require("the record of posted work, its amendment of a kind's life among it, replays as sealed", v.holds(), v.reason);
  }
  return s.done();
}
