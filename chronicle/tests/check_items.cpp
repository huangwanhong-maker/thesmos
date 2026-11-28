// Checks of items, routes and rulings (decision 0011): the base's third version (apply, question and petition declared as kinds that count
// as request of their types) is a revision of the second, admitted both ways, records carried as the left Kan extension, and an act of a
// declared directive kept as written with its reading a typed request; applications decided by the position's boundary rule (by rule,
// open, by a body at once or by proposal, by an office, by lot from a committed seed, after a conversion), positions assigned and their
// assignments ended by an office, a party leaving every position; questions on routes, a ruling registered and a second question on its
// subject refused with the ruling cited, a review of a ruling upheld or overruled only from the body that reviews rulings, an overdue mark
// after the term, recusal; petitions met by a checked deposit, adopted through a proposal under their route's decision, carried; G8' held
// by admission for a constitution that declares its routes, and routes read from what an older one declares; on random chronicles at
// most one ruling in force on a subject (Proposition 28), applications decided as an independent reading of the boundary rule says and
// a draw as the seed gives it (Proposition 29), and every item on a route that reaches someone able to answer it, shown to its author
// (Proposition 27); and the record replays.

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
namespace ko = kategoria::ontologies;
namespace ch = thesmos::chronicle;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields);
  return a;
}
const char* kRoutes = R"({
  "applications": {"term": 2, "review": "assembly"},
  "questions": {"body": "stewards", "term": 2, "review": "reviewers", "subjects": ["norm", "rule", "holding", "term"]},
  "petitions": {"source": {"by": "deposit", "within": "sources"}, "norm": {"by": "amendment", "level": "operational"},
                "platform": {"by": "carried", "to": "a league", "carrier": "steward"}}
})";
// The illustrative workshop with positions entered in every mode, a body of reviewers, a conversion, and (when asked) its routes.
constitutions::Constitution host(bool routes = true, const std::string& routes_text = kRoutes, long long writers = 2) {
  auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  auto set = [&](const std::string& part, const std::string& key, const std::string& value) { doc.get(part)->set(key, symploke::jsonld::parse(value)); };
  set("positions", "writer", R"({"held by": ["person", "operative"]})");
  set("positions", "clerk", R"({"held by": ["person"]})");
  set("positions", "juror", R"({"held by": ["person"]})");
  set("positions", "fellow", R"({"held by": ["person"]})");
  set("boundary", "writer", R"({"entry": {"on application": {"while": "holders fewer than )" + std::to_string(writers) + R"("}}, "exit": "free"})");
  set("boundary", "clerk", R"({"entry": {"assigned by": "steward"}, "exit": {"by the office": "steward"}})");
  set("boundary", "juror", R"({"entry": {"by lot among": ["contributor"]}, "exit": "free"})");
  set("boundary", "fellow", R"({"entry": {"after": "fellowship"}, "exit": "free"})");
  set("bodies", "reviewers", R"({"of": ["reviewer"]})");
  doc.set("conversions", symploke::jsonld::parse(R"({"fellowship": {"from": "conduct", "decided by": "admit", "to position": ["fellow"], "at least": 1}})"));
  if (routes) doc.set("routes", symploke::jsonld::parse(routes_text));
  return constitutions::read(symploke::jsonld::dump(doc));
}
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "cy", "dee", "eve", "ana", "fay", "gus"}) ps.add({p, Kind::person});
  ps.add({"ana/scribe", Kind::operative, "ana", "provider-a"});
  return ps;
}
State founded(const constitutions::Constitution& c) {
  State s(parties(), c);
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"cy", "contributor"}, {"dee", "contributor"}};
  ch::step(s, f);
  return s;
}
Act applying(const std::string& who, const std::string& id, const std::string& position, long long turn, std::map<std::string, std::string> more = {}) {
  more["id"] = id, more["position"] = position;
  return act(who, "apply", turn, more);
}
Act asking(const std::string& who, const std::string& id, const std::string& about, const std::string& subject, long long turn, const std::string& reviews = {}) {
  Act a = act(who, "question", turn, {{"id", id}, {"about", about}, {"subject", subject}});
  if (!reviews.empty()) a.fields["reviews"] = reviews;
  return a;
}
Act answering(const std::string& who, const std::string& id, const std::string& outcome, long long turn, std::map<std::string, std::string> more = {}) {
  more["id"] = id, more["outcome"] = outcome;
  return act(who, "answer", turn, more);
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. The third version: the declared directives --------------------------------------------------------------------------------
  {
    const auto direct = bh::revision_faults(bh::second_base(), bh::third_base());
    const auto by_kategoria = bh::admit_revision(bh::second_base(), bh::third_base());
    s.require("the base's third version, declaring apply, question and petition as kinds that count as request of their types, is a revision of the second admitted "
              "directly and by kategoria, which renames nothing",
              direct.empty() && by_kategoria.admitted() && bh::third_base().declared_kind("apply") && bh::primitive_of(bh::third_base(), "petition") == "request",
              (direct.empty() ? std::string() : direct.front()) + " / " + by_kategoria.verdict.reason);
    calx::random::Stream r(20261127, 30, 0, 0);
    int carried = 0;
    for (int trial = 0; trial < 50; trial++) {
      ko::Record rec;
      const auto& v2 = bh::second_base();
      for (int k = 0; k < 12; k++) {
        ko::Fact f;
        f.id = "fact" + std::to_string(k), f.text = f.id;
        const auto& p = v2.primitives[r.below(v2.primitives.size())];
        if (r.below(3) == 0) f.kind = "manifest_conduct", f.axes["grade"] = r.below(2) ? "A" : "C";
        else f.kind = bh::ident(p.kind), f.axes["force"] = std::string(acts::word(p.force));
        rec.facts.push_back(std::move(f));
      }
      const auto m = bh::carry(rec, v2, bh::third_base());
      carried += m.lost.empty() && m.problems.empty() && bh::carried_as_extension(rec, v2, bh::third_base()).holds();
    }
    s.require("on 50 random records of the second version, each is carried to the third losing nothing and is the left Kan extension along its map of kinds (Proposition 26)",
              carried == 50);
    State st = founded(host());
    const auto o = ch::step(st, applying("ana", "ap1", "writer", 1));
    s.require("an application is kept as written, an act of the declared kind apply, and read as a request of type application, which opens its item",
              o.admitted && st.acts.back().kind == "apply" && st.readings.back().kind == "request" && st.readings.back().field("type") == "application" && st.items.count("ap1"),
              o.reason);
  }

  // ---- 2. Applications decided by the boundary rule ----------------------------------------------------------------------------------
  State st = founded(host());
  long long turn = 1;
  {
    const auto a1 = ch::step(st, applying("ana", "w1", "writer", turn)), a2 = ch::step(st, applying("eve", "w2", "writer", turn)), a3 = ch::step(st, applying("fay", "w3", "writer", turn));
    s.require("by rule, while the writers are fewer than two: the first two applications admitted at once, the third declined in the same act with the condition that failed",
              a1.admitted && a2.admitted && a3.admitted && st.holds("ana", "writer") && st.holds("eve", "writer") && !st.holds("fay", "writer") && st.items.at("w3").status == "declined" &&
                  has(st.items.at("w3").finding, "holders fewer than 2: the position has 2"),
              st.items.at("w3").finding);
    const auto open = ch::step(st, applying("fay", "c1", "contributor", turn));
    s.require("open entry: an application admitted at once", open.admitted && st.holds("fay", "contributor") && st.items.at("c1").status == "admitted", open.reason);
    ch::step(st, applying("eve", "r1", "reviewer", turn));
    ch::step(st, applying("gus", "r2", "reviewer", turn));
    const auto by_eve = ch::step(st, answering("eve", "r1", "declined", ++turn));
    const auto admitted = ch::step(st, act("sam", "admit", turn, {{"position", "reviewer"}, {"whom", "eve"}, {"application", "r1"}}));
    const auto declined = ch::step(st, answering("sam", "r2", "declined", turn, {{"finding", "no seat"}}));
    s.require("by a body that decides by any one: an application stays open, a steward admits it, closing it admitted, or declines it; the applicant cannot answer her own",
              !by_eve.admitted && admitted.admitted && st.holds("eve", "reviewer") && st.items.at("r1").status == "admitted" && declined.admitted && st.items.at("r2").status == "declined",
              by_eve.reason + " / " + admitted.reason + " / " + declined.reason);
    ch::step(st, applying("sam", "m1", "member", turn));
    const auto self_answer = ch::step(st, answering("sam", "m1", "declined", turn));
    const auto self_admit = ch::step(st, act("sam", "admit", turn, {{"position", "member"}, {"whom", "sam"}, {"application", "m1"}}));
    s.require("recusal: a steward may neither decline nor admit his own application", !self_answer.admitted && !self_admit.admitted && has(self_admit.reason, "party to the application"),
              self_answer.reason + " / " + self_admit.reason);
    // By proposal: the steward appointed by the assembly.
    ch::step(st, applying("cy", "s1", "steward", turn));
    ch::step(st, act("rae", "propose", turn, {{"id", "p1"}, {"decision", "appoint steward"}, {"change.admit", "cy"}, {"change.position", "steward"}}));
    for (const char* v : {"sam", "rae", "dee", "fay"}) ch::step(st, act(v, "vote", turn, {{"id", "p1"}, {"value", "yes"}}));
    const auto early = ch::step(st, act("sam", "admit", turn, {{"position", "steward"}, {"whom", "cy"}, {"proposal", "p1"}, {"application", "s1"}}));
    const auto appointed = ch::step(st, act("sam", "admit", ++turn, {{"position", "steward"}, {"whom", "cy"}, {"proposal", "p1"}, {"application", "s1"}}));
    s.require("by a body that decides by proposal: the admission waits for the proposal's tempo, then closes the application admitted",
              !early.admitted && appointed.admitted && st.holds("cy", "steward") && st.items.at("s1").status == "admitted", early.reason + " / " + appointed.reason);
    // By an office.
    const auto assigned = ch::step(st, act("sam", "admit", turn, {{"position", "clerk"}, {"whom", "rae"}}));
    ch::step(st, applying("gus", "k1", "clerk", turn));
    const auto not_office = ch::step(st, act("dee", "admit", turn, {{"position", "clerk"}, {"whom", "gus"}, {"application", "k1"}}));
    const auto by_office = ch::step(st, act("sam", "admit", turn, {{"position", "clerk"}, {"whom", "gus"}, {"application", "k1"}}));
    const auto ended_by_other = ch::step(st, act("dee", "assign", turn, {{"position", "clerk"}, {"whom", "rae"}}));
    const auto ended = ch::step(st, act("sam", "assign", turn, {{"position", "clerk"}, {"whom", "rae"}}));
    s.require("by an office: the steward assigns a clerk with no application and another on his application, and ends an assignment, the exit being by the office; "
              "others may do neither",
              assigned.admitted && !not_office.admitted && by_office.admitted && st.items.at("k1").status == "admitted" && st.holds("gus", "clerk") && !ended_by_other.admitted &&
                  ended.admitted && !st.holds("rae", "clerk"),
              not_office.reason + " / " + ended_by_other.reason + " / " + ended.reason);
    // By lot, among contributors, from a committed seed.
    ch::step(st, applying("dee", "j1", "juror", turn));
    ch::step(st, applying("fay", "j2", "juror", turn));
    const auto outsider = ch::step(st, applying("gus", "j3", "juror", turn));
    const std::string seed = "the steward's seed 7";
    ch::step(st, act("sam", "report", turn, {{"seed digest", archeion::fixity::sha256_hex(seed)}, {"position", "juror"}}));
    ch::step(st, applying("cy", "j4", "juror", turn));  // opened after the seed was committed: no candidate of this draw
    const std::string winner = archeion::fixity::sha256_hex(seed + "\x1e" "j1") < archeion::fixity::sha256_hex(seed + "\x1e" "j2") ? "dee" : "fay";
    const std::string loser = winner == "dee" ? "fay" : "dee";
    const auto wrong = ch::step(st, act("sam", "admit", turn, {{"position", "juror"}, {"whom", loser}, {"seed", seed}}));
    const auto unseeded = ch::step(st, act("sam", "admit", turn, {{"position", "juror"}, {"whom", winner}, {"seed", "another seed"}}));
    const auto drawn = ch::step(st, act("sam", "admit", turn, {{"position", "juror"}, {"whom", winner}, {"seed", seed}}));
    s.require("by lot among contributors: one who holds no contributor's position is declined; the draw from the committed seed gives the position to the application whose "
              "digest with the seed is least, the other declined as not drawn; a draw naming another, or revealing another seed, is refused",
              outsider.admitted && st.items.at("j3").status == "declined" && !wrong.admitted && !unseeded.admitted && drawn.admitted && st.holds(winner, "juror") &&
                  st.items.at(winner == "dee" ? "j2" : "j1").finding == "not drawn",
              wrong.reason + " / " + unseeded.reason + " / " + drawn.reason);
    // After a conversion: a commitment discharged first.
    ch::step(st, applying("fay", "f1", "fellow", turn));
    ch::step(st, act("fay", "offer", turn, {{"id", "t1"}, {"to", "sam"}, {"matter", "catalogue"}}));
    ch::step(st, act("sam", "take", turn, {{"id", "t1"}}));
    ch::step(st, act("fay", "deliver", turn, {{"id", "t1"}, {"artefact", "entries"}}));
    ch::step(st, act("sam", "accept", turn, {{"id", "t1"}}));
    const auto again = ch::step(st, applying("fay", "f2", "fellow", turn));
    s.require("after a conversion: declined while the applicant has discharged no commitment, admitted once she has",
              st.items.at("f1").status == "declined" && has(st.items.at("f1").finding, "asks 1 commitments discharged") && again.admitted && st.items.at("f2").status == "admitted" &&
                  st.holds("fay", "fellow"),
              st.items.at("f1").finding + " / " + again.reason);
    const auto gone = ch::step(st, act("fay", "leave", turn, {{"position", "all"}}));
    bool none = true;
    for (const auto& [key, since] : st.positions) none = none && key.first != "fay";
    s.require("a party leaves every position at once, its commitments staying as they are", gone.admitted && none && st.commitments.at("t1").status == commitments::Status::discharged, gone.reason);
  }

  // ---- 3. Questions, rulings and precedent --------------------------------------------------------------------------------------------
  {
    const long long t0 = ++turn;
    const auto q1 = ch::step(st, asking("dee", "q1", "norm", "sources consulted", t0));
    const auto own = ch::step(st, answering("dee", "q1", "ruled", t0, {{"reading", "anyone"}}));
    const auto ruled = ch::step(st, answering("sam", "q1", "ruled", t0, {{"reading", "a member consults the sources and may send them anywhere"}}));
    const auto again = ch::step(st, asking("fay", "q2", "norm", "sources consulted", t0));
    s.require("a question opens on its route, its term two turns; the steward rules, the asker may not; a second question on the subject is refused with the ruling cited",
              q1.admitted && st.items.at("q1").due == t0 + 2 && !own.admitted && ruled.admitted && st.rulings.at("q1").in_force && !again.admitted && has(again.reason, "ruling q1"),
              own.reason + " / " + ruled.reason + " / " + again.reason);
    ch::step(st, asking("fay", "q3", "holding", "sources", t0));
    const auto misreferred = ch::step(st, answering("sam", "q3", "referred", t0, {{"finding", "q1"}}));
    const auto answered = ch::step(st, answering("sam", "q3", "answered", t0, {{"finding", "kept by the steward"}}));
    const auto about = ch::step(st, asking("fay", "q4", "work", "catalogue", t0));
    s.require("a question is referred only to a ruling on its own subject, answered otherwise, and refused when its subject is none the route takes",
              !misreferred.admitted && answered.admitted && !about.admitted && has(about.reason, "questions here are about"), misreferred.reason + " / " + about.reason);
    const auto review = ch::step(st, asking("dee", "q5", "norm", "sources consulted", t0, "q1"));
    const auto by_steward = ch::step(st, answering("sam", "q5", "ruled", t0, {{"reading", "members only, sent nowhere"}, {"overrules", "q1"}}));
    const auto stray = ch::step(st, asking("fay", "q6", "rule", "the term of a claim", t0));
    const auto overrule_elsewhere = ch::step(st, answering("sam", "q6", "ruled", t0, {{"reading", "one round"}, {"overrules", "q1"}}));
    const auto by_review = ch::step(st, answering("rae", "q5", "ruled", t0, {{"reading", "members only, sent nowhere"}, {"overrules", "q1"}}));
    const auto after = ch::step(st, asking("fay", "q7", "norm", "sources consulted", t0));
    s.require("a review of a ruling goes to the body that reviews rulings: an overruling refused from the steward, refused on a question that asks no review, admitted from "
              "the reviewers, the old ruling superseded and the new one cited thereafter",
              review.admitted && !by_steward.admitted && !overrule_elsewhere.admitted && by_review.admitted && !st.rulings.at("q1").in_force && st.rulings.at("q1").overruled_by == "q5" &&
                  st.rulings.at("q5").in_force && !after.admitted && has(after.reason, "ruling q5"),
              by_steward.reason + " / " + overrule_elsewhere.reason + " / " + by_review.reason + " / " + after.reason);
    ch::step(st, asking("fay", "q8", "norm", "sources consulted", t0, "q5"));
    const auto upheld = ch::step(st, answering("rae", "q8", "upheld", t0));
    const auto self_q = ch::step(st, asking("sam", "q9", "term", "a round", t0));
    const auto self_a = ch::step(st, answering("sam", "q9", "answered", t0));
    s.require("a review upholds a ruling, which stands; and a steward may not answer his own question", upheld.admitted && st.rulings.at("q5").in_force && self_q.admitted && !self_a.admitted,
              upheld.reason + " / " + self_a.reason);
    const auto profile_now = ch::profile(st);
    const State later = [&] {
      State x = st;
      ch::step(x, act("sam", "say", t0 + 3));
      return x;
    }();
    const auto profile_late = ch::profile(later);
    const auto brief = ch::brief(later, "cy");
    bool sees_overdue = false;
    for (const auto& w : brief.work) sees_overdue = sees_overdue || (w.id == "q6" && has(w.what, "overdue since turn"));
    bool own_item = false;
    for (const auto& i : ch::brief(later, "fay").items) own_item = own_item || (i.id == "q7" ? false : i.id == "q6" && i.status == "open");
    s.require("an unanswered question is overdue after its term, marked on every reader's profile, in the answering steward's briefing, and shown to its asker with its status",
              profile_now.items.at("question").second == 0 && profile_late.items.at("question").second >= 2 && sees_overdue && own_item && !brief.rulings.empty() &&
                  profile_late.rulings.size() == 2);
  }

  // ---- 4. Petitions ------------------------------------------------------------------------------------------------------------------
  {
    const long long t0 = ++turn;
    const auto pe1 = ch::step(st, act("dee", "petition", t0, {{"id", "pe1"}, {"kind", "source"}, {"about", "a series of rainfall"}}));
    const auto deposit = ch::step(st, act("dee", "deposit", t0, {{"holding", "rainfall"}, {"within", "sources"}, {"digest", archeion::fixity::sha256_hex("rainfall")}, {"petition", "pe1"}}));
    const bool waiting = st.items.at("pe1").status == "open";
    const auto checked = ch::step(st, act("rae", "attest", t0, {{"holding", "rainfall"}}));
    s.require("a petition met by deposit: the deposit within the sources awaits its check, and the check by another origin meets the petition",
              pe1.admitted && deposit.admitted && waiting && checked.admitted && st.items.at("pe1").status == "met", deposit.reason + " / " + checked.reason);
    ch::step(st, act("fay", "petition", t0, {{"id", "pe2"}, {"kind", "norm"}, {"about", "open the position member"}}));
    const auto wrong_decision = ch::step(st, act("rae", "propose", t0, {{"id", "p2"}, {"decision", "appoint steward"}, {"petition", "pe2"}, {"change.open", "member"}}));
    const auto carried = ch::step(st, act("rae", "propose", t0, {{"id", "p3"}, {"petition", "pe2"}, {"change.open", "member"}}));
    for (const char* v : {"sam", "rae", "dee", "fay", "cy"}) ch::step(st, act(v, "vote", t0, {{"id", "p3"}, {"value", "yes"}}));
    const auto amended = ch::step(st, act("sam", "amend", t0 + 1, {{"id", "p3"}}));
    s.require("a petition adopted through a proposal under its route's decision: one under another decision is refused; the amendment applies the change and adopts the "
              "petition with its proposal",
              !wrong_decision.admitted && carried.admitted && amended.admitted && st.items.at("pe2").status == "adopted" && st.rules.boundary.at("member").entry.mode == "open",
              wrong_decision.reason + " / " + carried.reason + " / " + amended.reason);
    ch::step(st, act("fay", "petition", t0 + 1, {{"id", "pe3"}, {"kind", "platform"}, {"about", "a longer term for claims"}}));
    const auto carried_out = ch::step(st, answering("cy", "pe3", "carried", t0 + 1));
    const auto undeclared = ch::step(st, act("fay", "petition", t0 + 1, {{"id", "pe4"}, {"kind", "transmission"}}));
    s.require("a petition carried by the office that carries it; a petition of a kind with no route refused (G8')",
              carried_out.admitted && st.items.at("pe3").status == "carried" && !undeclared.admitted && has(undeclared.reason, "no route for petitions of kind transmission"),
              carried_out.reason + " / " + undeclared.reason);
  }

  // ---- 5. G8' by admission, and routes read from an older constitution ------------------------------------------------------------------
  {
    auto faults_of = [](const constitutions::Constitution& c) {
      std::vector<std::string> out;
      for (const auto& f : constitutions::admission(c)) out.push_back(std::to_string(f.condition) + ": " + f.reason);
      return out;
    };
    auto any = [](const std::vector<std::string>& v, const std::string& part) {
      for (const auto& f : v)
        if (f.find(part) != std::string::npos) return true;
      return false;
    };
    const auto clean = faults_of(host());
    const auto no_questions = faults_of(host(true, R"({"applications": {}, "petitions": {}})"));
    const auto unreviewed = faults_of(host(true, R"({"applications": {}, "questions": "none", "petitions": {"data": {"by": "deposit", "within": "catalogue"}}})"));
    const auto disagree = faults_of(host(true, R"({"applications": {}, "questions": "none", "petitions": {}, "claims": {"body": "assembly"}})"));
    s.require("G8' by admission: the routes admitted; refused under condition 4 a constitution whose routes lack one for questions, a petition met by deposit within a holding "
              "that declares no review, and routes that restate the route for claims otherwise",
              clean.empty() && any(no_questions, "no route for questions") && any(unreviewed, "declares review") && any(disagree, "otherwise than the part claims"),
              clean.empty() ? std::string() : clean.front());
    State empty = founded(host(true, R"({"applications": {}, "questions": "none", "petitions": {}})"));
    const auto refused = ch::step(empty, asking("dee", "e1", "norm", "x", 1));
    const auto petition = ch::step(empty, act("dee", "petition", 1, {{"id", "e2"}, {"kind", "norm"}}));
    State older = founded(host(false));
    const auto by_claims = ch::step(older, asking("dee", "o1", "norm", "x", 1));
    const auto answered = ch::step(older, answering("sam", "o1", "ruled", 1, {{"reading", "y"}}));
    const auto old_petition = ch::step(older, act("dee", "petition", 1, {{"id", "o2"}, {"kind", "anything"}}));
    const auto untyped = ch::step(older, act("dee", "request", 1, {{"id", "o3"}, {"type", "nomination"}}));
    s.require("routes declared empty are admitted and their directives refused with the declaration; an older constitution with no routes sends questions to the route for "
              "claims and petitions to its operational amendment, and refuses an item of a type no declared kind opens",
              faults_of(empty.rules).empty() && !refused.admitted && has(refused.reason, "no route for questions") && !petition.admitted && by_claims.admitted && answered.admitted &&
                  old_petition.admitted && !untyped.admitted,
              refused.reason + " / " + by_claims.reason + " / " + old_petition.reason + " / " + untyped.reason);
    // A community that declares a kind of directive of its own needs its route.
    auto with_kind = [&](bool route) {
      auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
      doc.set("behaviour", symploke::jsonld::parse(R"({"format": "noetarium.behaviour/1", "name": "w", "version": 1, "extends": "base 3",
        "kinds": [{"kind": "nomination", "force": "directive", "counts as": "request", "fixed": {"type": "nomination"}}]})"));
      doc.set("routes", symploke::jsonld::parse(std::string(R"({"applications": {}, "questions": "none", "petitions": {})") +
                                                 (route ? R"(, "nomination": {"body": "stewards", "term": 1}})" : "}")));
      return constitutions::read(symploke::jsonld::dump(doc));
    };
    State nominating = founded(with_kind(true));
    const auto nominated = ch::step(nominating, act("dee", "nomination", 1, {{"id", "n1"}}));
    const auto settled = ch::step(nominating, answering("sam", "n1", "answered", 1));
    s.require("a community's own directive opens items of its type on the route it declares, and is refused admission without one (G8')",
              any(faults_of(with_kind(false)), "opens items of type nomination") && faults_of(with_kind(true)).empty() && nominated.admitted && settled.admitted &&
                  nominating.items.at("n1").status == "answered",
              nominated.reason + " / " + settled.reason);
  }

  // ---- 6. Proposition 28: at most one ruling in force on a subject ------------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 31, 0, 0);
    int held = 0, agree = 0, ruled = 0, overruled = 0;
    const std::vector<std::string> subjects = {"a", "b", "c"}, askers = {"cy", "dee", "fay", "gus"}, answerers = {"sam", "rae", "cy"};
    for (int trial = 0; trial < 100; trial++) {
      State x = founded(host());
      ch::step(x, act("sam", "admit", 1, {{"position", "member"}, {"whom", "fay"}}));
      std::map<std::string, std::string> in_force;  // the oracle: subject -> the ruling in force
      bool ok = true, oracle = true;
      int n = 0;
      for (long long t = 2; t < 40; t++) {
        const std::string subject = subjects[r.below(3)], id = "q" + std::to_string(++n);
        const bool review = r.below(3) == 0 && in_force.count(subject);
        const auto o = ch::step(x, asking(askers[r.below(4)], id, "norm", subject, t, review ? in_force[subject] : std::string()));
        oracle = oracle && o.admitted == (!in_force.count(subject) || review);
        if (!o.admitted) continue;
        const std::string by = answerers[r.below(3)];
        const bool overrule = review && r.below(2);
        std::map<std::string, std::string> f = {{"reading", "reading " + id}};
        if (overrule) f["overrules"] = in_force[subject];
        const auto a = ch::step(x, answering(by, id, review && !overrule ? "upheld" : "ruled", t, f));
        // The oracle: the steward (sam) rules on a subject not ruled; the reviewer (rae) alone reviews; nobody answers their own question.
        const std::string asker = x.items.at(id).author;
        const bool own = by == asker;
        const bool expect = !own && (review ? by == "rae" : by == "sam");
        oracle = oracle && a.admitted == expect;
        if (a.admitted && (!review || overrule)) {
          ruled++, overruled += overrule;
          in_force[subject] = id;
        }
        std::map<std::string, int> count;
        for (const auto& [rid, ru] : x.rulings) count[ru.subject] += ru.in_force;
        for (const auto& [sub, k] : count) ok = ok && k <= 1;
        for (const auto& [sub, rid] : in_force) ok = ok && x.rulings.at(rid).in_force;
      }
      held += ok, agree += oracle;
    }
    s.require("on 100 random chronicles of questions, rulings and reviews (" + std::to_string(ruled) + " rulings, " + std::to_string(overruled) + " overrulings), at most "
                  "one ruling stands on a subject at every prefix, every question and answer admitted exactly as an independent reading of the routes says (Proposition 28)",
              held == 100 && agree == 100 && overruled > 10);
  }

  // ---- 7. Proposition 29: applications decided by the boundary rule ---------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 32, 0, 0);
    int agree = 0, bounded = 0, draws = 0;
    const std::vector<std::string> people = {"ana", "eve", "fay", "gus", "cy", "dee"};
    for (int trial = 0; trial < 100; trial++) {
      const long long cap = between(r, 1, 4);
      State x = founded(host(true, kRoutes, cap));
      bool oracle = true, within = true;
      int n = 0;
      for (long long t = 1; t < 30; t++) {
        const std::string who = people[r.below(people.size())];
        if (r.below(4) == 0 && x.holds(who, "writer")) {
          ch::step(x, act(who, "leave", t, {{"position", "writer"}}));
          continue;
        }
        long long holders = 0;
        for (const auto& [key, since] : x.positions) holders += key.second == "writer";
        bool pending = false;
        for (const auto& [id, i] : x.items) pending = pending || (i.status == "open" && i.author == who && i.position == "writer");
        const bool holds = x.holds(who, "writer");
        const std::string id = "w" + std::to_string(++n);
        const auto o = ch::step(x, applying(who, id, "writer", t));
        oracle = oracle && o.admitted == (!holds && !pending);
        if (o.admitted) oracle = oracle && (x.items.at(id).status == (holders < cap ? "admitted" : "declined"));
        long long now = 0;
        for (const auto& [key, since] : x.positions) now += key.second == "writer";
        within = within && now <= cap;
      }
      // A draw among contributors for the juror's seat, against the seed's own reading.
      State y = founded(host());
      ch::step(y, act("sam", "admit", 1, {{"position", "member"}, {"whom", "fay"}}));
      std::vector<std::string> ids;
      for (const char* who : {"cy", "dee"}) ids.push_back(std::string("j-") + who), ch::step(y, applying(who, ids.back(), "juror", 1));
      const std::string seed = "seed " + std::to_string(trial);
      ch::step(y, act("sam", "report", 2, {{"seed digest", archeion::fixity::sha256_hex(seed)}, {"position", "juror"}}));
      std::string best, winner;
      for (const auto& id : ids)
        if (const std::string d = archeion::fixity::sha256_hex(seed + "\x1e" + id); best.empty() || d < best) best = d, winner = id;
      const auto d = ch::step(y, act("sam", "admit", 2, {{"position", "juror"}, {"whom", winner.substr(2)}, {"seed", seed}}));
      draws += d.admitted && y.holds(winner.substr(2), "juror");
      agree += oracle, bounded += within;
    }
    s.require("on 100 random chronicles of applications under a rule 'holders fewer than n' (n from 1 to 4), each is admitted, and admitted or declined, exactly as an "
                  "independent reading of the rule says, the holders never more than n; and on 100 draws the position goes where the seed's digest says (Proposition 29)",
              agree == 100 && bounded == 100 && draws == 100);
  }

  // ---- 8. Proposition 27: every item on a route that reaches someone able to answer it ----------------------------------------------
  {
    calx::random::Stream r(20261127, 33, 0, 0);
    int routed = 0, opened = 0, shown = 0;
    const std::vector<std::string> people = {"ana", "eve", "fay", "gus", "cy", "dee", "rae"};
    for (int trial = 0; trial < 60; trial++) {
      State x = founded(host());
      for (long long t = 1; t < 15; t++) {
        const std::string who = people[r.below(people.size())], id = "i" + std::to_string(t);
        Act a;
        switch (r.below(4)) {
          case 0: a = applying(who, id, r.below(2) ? "reviewer" : "clerk", t); break;
          case 1: a = asking(who, id, r.below(2) ? "norm" : "rule", "subject " + std::to_string(r.below(5)), t); break;
          case 2: a = act(who, "petition", t, {{"id", id}, {"kind", r.below(2) ? "norm" : "platform"}}); break;
          default: a = applying(who, id, "writer", t);
        }
        if (!ch::step(x, a).admitted) continue;
        const ch::Item& item = x.items.at(id);
        opened++;
        if (item.status != "open") {
          routed++, shown++;
          continue;
        }
        // Someone among the parties may answer it, and its author's briefing shows it with its status.
        bool someone = false;
        for (const auto& [pid, party] : x.parties.all()) someone = someone || ch::may_answer(x, item, pid, pid);
        routed += someone;
        for (const auto& i : ch::brief(x, item.author).items) shown += i.id == id;
      }
    }
    s.require("on 60 random chronicles (" + std::to_string(opened) + " items opened), every item either closed in the act that opened it or open on a route on which some "
                  "party may answer it, and shown to its author with its status (Proposition 27, G8')",
              routed == opened && shown == opened && opened > 300);
  }

  // ---- 9. Replay -----------------------------------------------------------------------------------------------------------------------
  {
    const auto v = ch::replay(parties(), host(), st.acts, st.journal);
    s.require("the record of applications, questions, rulings, reviews and petitions replays as sealed", v.holds(), v.reason);
  }

  return s.done();
}
